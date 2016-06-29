#include "cbase.h"
#include "clientmode_neonormal.h"
#include "ivmodemanager.h"
#include "panelmetaclassmgr.h"
#include "neo_gamerules.h"
#include "VGUI\neoviewport.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#define SCREEN_FILE		"scripts/vgui_screens.txt"


class CNEOModeManager : public IVModeManager
{
public:
	virtual void	Init();
	virtual void	SwitchMode( bool commander, bool force ) {}
	virtual void	LevelInit( const char *newmap );
	virtual void	LevelShutdown( void );
	virtual void	ActivateMouse( bool isactive ) {}
};

static CNEOModeManager g_ModeManager;
IVModeManager *modemanager = (IVModeManager *) &g_ModeManager;

void CNEOModeManager::Init()
{
	g_pClientMode = GetClientModeNormal();

	PanelMetaClassMgr()->LoadMetaClassDefinitionFile( SCREEN_FILE );
}

void CNEOModeManager::LevelInit( const char *newmap )
{	
	g_pClientMode->LevelInit( newmap );
}

void CNEOModeManager::LevelShutdown( void )
{
	NEOGameRules()->LevelShutdown();
	g_pClientMode->LevelShutdown();
}


// Instance the singleton and expose the interface to it.
IClientMode* GetClientModeNormal()
{
	return GetClientModeNEONormal();
}

ClientModeNEONormal* GetClientModeNEONormal()
{
	static ClientModeNEONormal g_ClientModeNormal;
	return &g_ClientModeNormal;
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