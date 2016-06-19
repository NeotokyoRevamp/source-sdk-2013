#include "cbase.h"
#include "clientmode_neonormal.h"
#include "VGUI\neoviewport.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"


// Instance the singleton and expose the interface to it.
IClientMode* GetClientModeNormal()
{
	static ClientModeNEONormal g_ClientModeNormal;
	return &g_ClientModeNormal;
}

ClientModeNEONormal* GetClientModeNEONormal()
{
	return (ClientModeNEONormal*) GetClientModeNormal();
}


ClientModeNEONormal::ClientModeNEONormal()
{
	m_bUnknown = false;
	m_bIsNEODev = false;
}

ClientModeNEONormal::~ClientModeNEONormal()
{
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void ClientModeNEONormal::InitViewport()
{
	BaseClass::InitViewport();

	m_pViewport = new NEOViewport();
	m_pViewport->Start( gameuifuncs, gameeventmanager );
}

void ClientModeNEONormal::Init()
{
	BaseClass::Init();
}

void ClientModeNEONormal::Shutdown()
{
	BaseClass::Shutdown();
}

void ClientModeNEONormal::LevelInit( const char *newmap )
{
	BaseClass::LevelInit( newmap );
}

void ClientModeNEONormal::LevelShutdown()
{
	BaseClass::LevelShutdown();
}

float ClientModeNEONormal::GetViewModelFOV()
{
	return 75.f;
}