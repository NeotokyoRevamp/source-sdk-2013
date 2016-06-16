#ifndef NEO_GAMERULES_H
#define NEO_GAMERULES_H

#ifdef _WIN32
#pragma once
#endif

#include "teamplay_gamerules.h"

#ifdef CLIENT_DLL
	#define CNEOGameRules C_NEOGameRules
	#define CNEOGameRulesProxy C_NEOGameRulesProxy
#endif

class CNEOGameRulesProxy : public CGameRulesProxy
{
public:
	DECLARE_CLASS( CNEOGameRules, CGameRulesProxy );
	DECLARE_NETWORKCLASS();
};

class CNEOGameRules : public CTeamplayRules
{
public:
	DECLARE_CLASS( CNEOGameRules, CTeamplayRules );
	DECLARE_CLIENTCLASS_NOBASE(); // This makes datatables able to access our private vars.

	CNEOGameRules();

public:
	virtual bool ShouldCollide( int collisionGroup0, int collisionGroup1 );

	virtual const CViewVectors* GetViewVectors() const;

private:
	CNetworkVar( int, m_iGameType );

	CNetworkVar( float, m_fRoundTimeLeft );
	CNetworkVar( int, m_iRoundNumber );

	CNetworkVar( int, m_iGameState );
	CNetworkVar( int, m_iGameHud );

	CNetworkVar( int, m_iMVP );

	CNetworkVar( int, m_iAttackingTeam );
	
	CNetworkVar( bool, m_bFreezePeriod );
};

inline CNEOGameRules* NEOGameRules()
{
	return static_cast< CNEOGameRules* >( g_pGameRules );
}

#endif // NEO_GAMERULES_H