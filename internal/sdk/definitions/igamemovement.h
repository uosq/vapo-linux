#pragma once

#include "cbasehandle.h"
#include "types.h"

class CBasePlayer;
class CBaseEntity;
typedef CHandle<CBaseEntity> EHANDLE;

class CMoveData
{
public:
	bool			m_bFirstRunOfFunctions : 1;
	bool			m_bGameCodeMovedPlayer : 1;

	EHANDLE	m_nPlayerHandle;	// edict index on server, client entity handle on client

	int				m_nImpulseCommand;	// Impulse command issued.
	QAngle			m_vecViewAngles;	// Command view angles (local space)
	QAngle			m_vecAbsViewAngles;	// Command view angles (world space)
	int				m_nButtons;			// Attack buttons.
	int				m_nOldButtons;		// From host_client->oldbuttons;
	float			m_flForwardMove;
	float			m_flOldForwardMove;
	float			m_flSideMove;
	float			m_flUpMove;
	
	float			m_flMaxSpeed;
	float			m_flClientMaxSpeed;

	// Variables from the player edict (sv_player) or entvars on the client.
	// These are copied in here before calling and copied out after calling.
	Vector			m_vecVelocity;		// edict::velocity		// Current movement direction.
	QAngle			m_vecAngles;		// edict::angles
	QAngle			m_vecOldAngles;
	
// Output only
	float			m_outStepHeight;	// how much you climbed this move
	Vector			m_outWishVel;		// This is where you tried 
	Vector			m_outJumpVel;		// This is your jump velocity

	// Movement constraints	(radius 0 means no constraint)
	Vector			m_vecConstraintCenter;
	float			m_flConstraintRadius;
	float			m_flConstraintWidth;
	float			m_flConstraintSpeedFactor;

	void			SetAbsOrigin( const Vector &vec );
	const Vector	&GetAbsOrigin() const;

private:
	Vector			m_vecAbsOrigin;		// edict::origin
};

inline const Vector &CMoveData::GetAbsOrigin() const
{
	return m_vecAbsOrigin;
}

class IGameMovement
{
public:
	virtual			~IGameMovement( void ) {}
	
	// Process the current movement command
	virtual void	ProcessMovement( CBasePlayer *pPlayer, CMoveData *pMove ) = 0;		
	virtual void	StartTrackPredictionErrors( CBasePlayer *pPlayer ) = 0;
	virtual void	FinishTrackPredictionErrors( CBasePlayer *pPlayer ) = 0;
	virtual void	DiffPrint( char const *fmt, ... ) = 0;

	// Allows other parts of the engine to find out the normal and ducked player bbox sizes
	virtual Vector	GetPlayerMins( bool ducked ) const = 0;
	virtual Vector	GetPlayerMaxs( bool ducked ) const = 0;
	virtual Vector  GetPlayerViewOffset( bool ducked ) const = 0;
};