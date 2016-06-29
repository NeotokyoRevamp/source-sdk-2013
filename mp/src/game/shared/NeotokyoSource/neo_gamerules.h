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
	DECLARE_CLASS( CNEOGameRulesProxy, CGameRulesProxy );
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

	// These might be used somewhere, although these are pretty fucking useless (can I swear in c++)
	virtual bool Unknown00( int unk, int unk2 ) { return true; }
	virtual int Unknown01( int unk, int unk2 ) { return 0; }
	virtual bool Unknown02() { return true; }
	virtual float Unknown03( int unk ) { return 0.f; }

	virtual void LevelShutdown();

	virtual void Unknown04(int unk, int unk2) {}

	inline bool IsInFreezePeriod() { return m_bFreezePeriod; }

private:
	void* m_iUnknown;
	int m_iUnknown1;
	int m_iUnknown2;
	int m_iUnknown3;
	void* m_iUnknown4;

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