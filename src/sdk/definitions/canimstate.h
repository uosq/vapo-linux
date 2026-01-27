//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#pragma once

#include "ai_activity.h"
#include "studio.h"
#include "types.h"

class CTFPlayer;

class CPlayerAnimState
{
public:
	enum
	{
		TURN_NONE = 0,
		TURN_LEFT,
		TURN_RIGHT
	};

						CPlayerAnimState( CTFPlayer *outer );

	Activity			BodyYawTranslateActivity( Activity activity );

	void				Update();

	const QAngle&		GetRenderAngles();
				
	void				GetPoseParameters( float poseParameter[MAXSTUDIOPOSEPARAM] );

	CTFPlayer		*GetOuter();

private:
	void				GetOuterAbsVelocity( Vector& vel );

	int					ConvergeAngles( float goal,float maxrate, float dt, float& current );

	void				EstimateYaw( void );
	void				ComputePoseParam_BodyYaw( void );
	void				ComputePoseParam_BodyPitch( void );
	void				ComputePoseParam_BodyLookYaw( void );

	void				ComputePlaybackRate();

	CTFPlayer		*m_pOuter;

	float				m_flGaitYaw;
	float				m_flStoredCycle;

	// The following variables are used for tweaking the yaw of the upper body when standing still and
	//  making sure that it smoothly blends in and out once the player starts moving
	// Direction feet were facing when we stopped moving
	float				m_flGoalFeetYaw;
	float				m_flCurrentFeetYaw;

	float				m_flCurrentTorsoYaw;

	// To check if they are rotating in place
	float				m_flLastYaw;
	// Time when we stopped moving
	float				m_flLastTurnTime;

	// One of the above enums
	int					m_nTurningInPlace;

	QAngle				m_angRender;
};
