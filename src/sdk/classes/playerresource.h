#pragma once

#include "../../sdk/netvars/netvar.h"

class CTFPlayerResource
{
public:
	NETVAR_ARRAY(m_iTotalScore, "CTFPlayerResource->m_iTotalScore", int);
	NETVAR_ARRAY(m_iMaxHealth, "CTFPlayerResource->m_iMaxHealth", int);
	NETVAR_ARRAY(m_iMaxBuffedHealth, "CTFPlayerResource->m_iMaxBuffedHealth", int);
	NETVAR_ARRAY(m_iPlayerClass, "CTFPlayerResource->m_iPlayerClass", int);
	NETVAR_ARRAY(m_bArenaSpectator, "CTFPlayerResource->m_bArenaSpectator", bool);
	NETVAR_ARRAY(m_iActiveDominations, "CTFPlayerResource->m_iActiveDominations", int);
	NETVAR_ARRAY(m_flNextRespawnTime, "CTFPlayerResource->m_flNextRespawnTime", float);
	NETVAR_ARRAY(m_iChargeLevel, "CTFPlayerResource->m_iChargeLevel", int);
	NETVAR_ARRAY(m_iDamage, "CTFPlayerResource->m_iDamage", int);
	NETVAR_ARRAY(m_iDamageAssist, "CTFPlayerResource->m_iDamageAssist", int);
	NETVAR_ARRAY(m_iDamageBoss, "CTFPlayerResource->m_iDamageBoss", int);
	NETVAR_ARRAY(m_iHealing, "CTFPlayerResource->m_iHealing", int);
	NETVAR_ARRAY(m_iHealingAssist, "CTFPlayerResource->m_iHealingAssist", int);
	NETVAR_ARRAY(m_iDamageBlocked, "CTFPlayerResource->m_iDamageBlocked", int);
	NETVAR_ARRAY(m_iCurrencyCollected, "CTFPlayerResource->m_iCurrencyCollected", int);
	NETVAR_ARRAY(m_iBonusPoints, "CTFPlayerResource->m_iBonusPoints", int);
	NETVAR_ARRAY(m_iPlayerLevel, "CTFPlayerResource->m_iPlayerLevel", int);
	NETVAR_ARRAY(m_iStreaks, "CTFPlayerResource->m_iStreaks", int);
	NETVAR_ARRAY(m_iUpgradeRefundCredits, "CTFPlayerResource->m_iUpgradeRefundCredits", int);
	NETVAR_ARRAY(m_iBuybackCredits, "CTFPlayerResource->m_iBuybackCredits", int);
	NETVAR_ARRAY(m_iPartyLeaderRedTeamIndex, "CTFPlayerResource->m_iPartyLeaderRedTeamIndex", int);
	NETVAR_ARRAY(m_iPartyLeaderBlueTeamIndex, "CTFPlayerResource->m_iPartyLeaderBlueTeamIndex", int);
	NETVAR_ARRAY(m_iEventTeamStatus, "CTFPlayerResource->m_iEventTeamStatus", int);
	NETVAR_ARRAY(m_iPlayerClassWhenKilled, "CTFPlayerResource->m_iPlayerClassWhenKilled", int);
	NETVAR_ARRAY(m_iConnectionState, "CTFPlayerResource->m_iConnectionState", int);
	NETVAR_ARRAY(m_flConnectTime, "CTFPlayerResource->m_flConnectTime", float);
};