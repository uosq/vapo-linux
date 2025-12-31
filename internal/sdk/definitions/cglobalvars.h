#pragma once

struct CGlobalVars
{
	float realtime;
	int framecount;
	float absolute_frametime;
	float curtime;
	float frametime;
	int max_clients;
	int tickcount;
	float interval_per_tick;
	float interpolation_amount;
	int sim_ticks_this_frame;
	int network_protocol;
	void* save_data; //CSaveRestoreData
	bool client;
	int nTimestampNetworkingBase;
	int nTimestampRandomizeWindow;
};

inline static CGlobalVars* globalvars;