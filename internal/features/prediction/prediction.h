#pragma once

#include "../../sdk/classes/player.h"
#include "../../sdk/interfaces/interfaces.h"
#include "../../sdk/helpers/helper.h"
#include "../../sdk/definitions/cgametrace.h"
#include "../../sdk/definitions/ctracefilters.h"
#include "../../sdk/definitions/bspflags.h"
#include <algorithm>
#include <vector>

#define	COORD_INTEGER_BITS			14
#define COORD_FRACTIONAL_BITS		5
#define COORD_DENOMINATOR			(1<<(COORD_FRACTIONAL_BITS))
#define COORD_RESOLUTION			(1.0/(COORD_DENOMINATOR))

namespace PlayerPrediction
{
	inline void Friction(Vector &velocity, bool isOnGround)
	{
		float speed = velocity.Length();
		if (speed < 0.01f)
			return;

		ConVar* sv_stopspeed_cvar = interfaces::Cvar->FindVar("sv_stopspeed");
		if (!sv_stopspeed_cvar)
			return;
			
		float sv_stopspeed = sv_stopspeed_cvar->GetFloat();
		float drop = 0;

		if (isOnGround)
		{
			ConVar* sv_friction_cvar = interfaces::Cvar->FindVar("sv_friction");
			if (!sv_friction_cvar)
				return;
			
			float sv_friction = sv_friction_cvar->GetFloat();
			float control = speed < sv_stopspeed ? sv_stopspeed : speed;
			drop = control * sv_friction * interfaces::GlobalVars->interval_per_tick;
		}

		float newspeed = speed - drop;
		if (newspeed < 0.01f)
		{
			velocity.Set(0, 0, 0);
			return;
		}
		
		if (newspeed != speed)
		{
			newspeed /= speed;
			velocity *= newspeed;
		}
	}

	inline bool CheckIsOnGround(CTFPlayer* player, Vector origin, Vector mins, Vector maxs)
	{
		Vector down = Vector(origin.x, origin.y, origin.z - 2);

		CGameTrace trace;
		CTraceFilterWorldAndPropsOnly filter;
		filter.pSkip = player;

		helper::engine::TraceHull(origin, down, mins, maxs, MASK_PLAYERSOLID, &filter, &trace);
		return trace.fraction < 1.0f && trace.plane.normal.z >= 0.7f;
	}

	inline void AcceleratePlayer(Vector &velocity, Vector wishdir, float wishspeed, float accel)
	{
		float addspeed, accelspeed, currentspeed;

		currentspeed = velocity.Dot(wishdir);
		addspeed = wishspeed - currentspeed;

		if (addspeed <= 0)
			return;

		accelspeed = accel * interfaces::GlobalVars->interval_per_tick * wishspeed;
		if (accelspeed > addspeed)
			accelspeed = addspeed;

		velocity += wishdir * accelspeed;
	}

	inline void AirAcceleratePlayer(Vector &velocity, Vector wishdir, float wishspeed, float accel, int surf)
	{
		float currentspeed = velocity.Dot(wishdir);
		float addspeed = wishspeed - currentspeed;
		if (addspeed <= 0)
			return;

		float accelspeed = std::min(accel * wishspeed * surf * interfaces::GlobalVars->interval_per_tick, addspeed);
		velocity += wishdir * accelspeed;
	}

	inline void StayOnGround(CTFPlayer* player, Vector &origin, Vector mins, Vector maxs, float stepsize)
	{
		Vector start = Vector(origin.x, origin.y, origin.z + 2.0f);
		Vector end = Vector(origin.x, origin.y, origin.z - stepsize);

		CGameTrace trace;
		CTraceFilterWorldAndPropsOnly filter;
		filter.pSkip = player;

		helper::engine::TraceHull(start, end, mins, maxs, MASK_PLAYERSOLID, &filter, &trace);
		
		if (trace.fraction > 0.0f && trace.fraction < 1.0f && !trace.startsolid && trace.plane.normal.z >= 0.7f)
		{
			float delta = fabs(origin.z - trace.endpos.z);
			if (delta > 0.5f * COORD_RESOLUTION)
				origin = trace.endpos;
		}
	}

	inline bool TryStepMove(CTFPlayer* player, Vector& origin, Vector& velocity, Vector mins, Vector maxs, float stepSize)
	{
		Vector move = velocity * interfaces::GlobalVars->interval_per_tick;

		CTraceFilterWorldAndPropsOnly filter;
		filter.pSkip = player;

		CGameTrace trace;

		// --- slide move
		helper::engine::TraceHull(
			origin,
			origin + move,
			mins,
			maxs,
			MASK_PLAYERSOLID,
			&filter,
			&trace
		);

		Vector slideEnd  = trace.endpos;
		float  slideFrac = trace.fraction;

		// fully unobstructed
		if (slideFrac == 1.0f)
		{
			origin = slideEnd;
			return true;
		}

		// --- step up
		Vector up(0.0f, 0.0f, stepSize);

		CGameTrace traceUp;
		helper::engine::TraceHull(
			origin,
			origin + up,
			mins,
			maxs,
			MASK_PLAYERSOLID,
			&filter,
			&traceUp
		);

		// can't step up
		if (traceUp.fraction < 1.0f)
		{
			if (slideFrac > 0.0f)
				origin = slideEnd;

			return slideFrac > 0.0f;
		}

		Vector stepOrigin = traceUp.endpos;

		// --- forward after step
		CGameTrace traceForward;
		helper::engine::TraceHull(
			stepOrigin,
			stepOrigin + move,
			mins,
			maxs,
			MASK_PLAYERSOLID,
			&filter,
			&traceForward
		);

		// blocked while stepping forward
		if (traceForward.fraction < 1.0f)
		{
			if (slideFrac > 0.0f)
				origin = slideEnd;

			return slideFrac > 0.0f;
		}

		// --- step down
		CGameTrace traceDown;
		helper::engine::TraceHull(
			traceForward.endpos,
			traceForward.endpos - up,
			mins,
			maxs,
			MASK_PLAYERSOLID,
			&filter,
			&traceDown
		);

		// no ground found
		if (traceDown.fraction == 1.0f)
		{
			if (slideFrac > 0.0f)
				origin = slideEnd;

			return slideFrac > 0.0f;
		}

		Vector stepEnd = traceDown.endpos;

		// --- choose better result (2D distance)
		float slideDist = (slideEnd - origin).Length2D();
		float stepDist  = (stepEnd  - origin).Length2D();

		if (stepDist > slideDist)
		{
			origin = stepEnd;
			return true;
		}

		if (slideFrac > 0.0f)
			origin = slideEnd;

		return slideFrac > 0.0f;
	}

	inline void Predict(CTFPlayer* player, float time_seconds, std::vector<Vector> &path)
	{
		Vector velocity(0, 0, 0);
		{	// copy our velocity because netvars are references!
			Vector velocity_netvar = player->GetVelocity();
			velocity.Set(velocity_netvar.x, velocity_netvar.y, velocity_netvar.z);
		}

		Vector origin(0, 0, 0);
		{	// copy our origin because netvars are references!
			Vector origin_netvar = player->GetAbsOrigin();
			origin.Set(origin_netvar.x, origin_netvar.y, origin_netvar.z);
		}

		if (velocity.Length() < 5.0f)
		{
			path.emplace_back(origin);
			return;
		}

		float maxspeed = player->m_flMaxspeed();
		float wishspeed = std::min(velocity.Length(), maxspeed);

		float clock = 0.0f;

		Vector wishdir = velocity / velocity.Length();
		Vector mins = player->m_vecMins();
		Vector maxs = player->m_vecMaxs();

		ConVar* sv_airaccelerate = interfaces::Cvar->FindVar("sv_airaccelerate");
		if (!sv_airaccelerate)
			return;

		ConVar* sv_accelerate = interfaces::Cvar->FindVar("sv_accelerate");
		if (!sv_accelerate)
			return;

		float stepsize = player->m_flStepSize();
		float gravity = 800 * interfaces::GlobalVars->interval_per_tick * 0.5;

		time_seconds = std::min(time_seconds, 5.0f);
		while (clock < time_seconds)
		{
			bool isOnGround = CheckIsOnGround(player, origin, mins, maxs);

			velocity.z -= gravity;

			if (isOnGround)
			{
				velocity.z = 0;
				Friction(velocity, isOnGround);
				AcceleratePlayer(velocity, wishdir, wishspeed, sv_accelerate->GetFloat());
			} 
			else
			{
				AirAcceleratePlayer(velocity, wishdir, wishspeed, sv_accelerate->GetFloat(), 0);
			}

			if (!TryStepMove(player, origin, velocity, mins, maxs, stepsize))
			{
				velocity.x = 0.0f;
				velocity.y = 0.0f;
			}

			//origin += velocity * interfaces::GlobalVars->interval_per_tick;

			velocity.z -= gravity;

			isOnGround = CheckIsOnGround(player, origin, mins, maxs);
			if (!isOnGround)
				StayOnGround(player, origin, mins, maxs, stepsize);

			path.emplace_back((Vector){origin.x, origin.y, origin.z});

			clock += interfaces::GlobalVars->interval_per_tick;
		}
	}
};