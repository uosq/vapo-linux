#pragma once

#include "types.h" // Solely to get at define for QAngle

class CBasePlayer;
class CUserCmd;
class MoveHelper;
class MoveData;

class IPrediction
{
public:
	virtual			~IPrediction( void ) {};

	virtual void	Init( void ) = 0;
	virtual void	Shutdown( void ) = 0;

	// Run prediction
	virtual void	Update
					( 
						int startframe,				// World update ( un-modded ) most recently received
						bool validframe,			// Is frame data valid
						int incoming_acknowledged,	// Last command acknowledged to have been run by server (un-modded)
						int outgoing_command		// Last command (most recent) sent to server (un-modded)
					) = 0;

	// We are about to get a network update from the server.  We know the update #, so we can pull any
	//  data purely predicted on the client side and transfer it to the new from data state.
	virtual void	PreEntityPacketReceived( int commands_acknowledged, int current_world_update_packet ) = 0;
	virtual void	PostEntityPacketReceived( void ) = 0;
	virtual void	PostNetworkDataReceived( int commands_acknowledged ) = 0;

	virtual void	OnReceivedUncompressedPacket( void ) = 0;

	// The engine needs to be able to access a few predicted values
	virtual void	GetViewOrigin( Vector& org ) = 0;
	virtual void	SetViewOrigin( Vector& org ) = 0;
	virtual void	GetViewAngles( QAngle& ang ) = 0;
	virtual void	SetViewAngles( QAngle& ang ) = 0;
	virtual void	GetLocalViewAngles( QAngle& ang ) = 0;
	virtual void	SetLocalViewAngles( QAngle& ang ) = 0;
};

class Prediction : public IPrediction {
public:
	virtual ~Prediction() {};
	virtual void Init() = 0;
	virtual void Shutdown() = 0;
	virtual void Update(int startframe, bool validframe, int incoming_acknowledged, int outgoing_command) = 0;
	virtual void OnReceivedUncompressedPacket() = 0;
	virtual void PreEntityPacketReceived(int commands_acknowledged, int current_world_update_packet) = 0;
	virtual void PostEntityPacketReceived() = 0;
	virtual void PostNetworkDataReceived(int commands_acknowledged) = 0;
	virtual bool IsInPrediction() = 0;
	virtual bool IsFirstTimePredicted() = 0;
	virtual int  GetIncomingPacketNumber() = 0;
	virtual void GetViewOrigin(Vec3& org) = 0;
	virtual void SetViewOrigin(Vec3& org) = 0;
	virtual void GetViewAngles(Vec3& ang) = 0;
	virtual void SetViewAngle(Vec3& ang) = 0;
	virtual void GetLocalViewAngles(Vec3& ang) = 0;
	virtual void SetLocalViewAngles(Vec3& ang) = 0;
	virtual void RunCommand(CBasePlayer* player, CUserCmd* ucmd, MoveHelper* moveHelper) = 0;
	virtual void SetupMove(CBasePlayer* player, CUserCmd* ucmd, MoveHelper* pHelper, MoveData* move) = 0;
	virtual void FinishMove(CBasePlayer* player, CUserCmd* ucmd, MoveData* move) = 0;
	virtual void SetIdealPitch(CBasePlayer* player, const Vec3& origin, const Vec3& angles, const Vec3& viewheight) = 0;
	virtual void _Update(bool received_new_world_update, bool validframe, int incoming_acknowledged, int outgoing_command) = 0;
public:
	int last_ground;
	bool in_prediction;
	bool first_time_predicted;
	bool old_cl_predict_value;
	bool engine_paused;
	int previous_start_frame;
	int commands_predicted;
	int server_commands_acknowledged;
	int previous_ack_had_errors;
	int incoming_packet_number;
	float ideal_pitch;
};
