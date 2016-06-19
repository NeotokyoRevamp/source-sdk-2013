#include "cbase.h"
#include "NEOViewport.h"
#include "neo_shareddefs.h"

void NEOViewport::ApplySchemeSettings( vgui::IScheme *pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );

	gHUD.InitColors( pScheme );

	SetPaintBackgroundEnabled( false );
}

IViewPortPanel* NEOViewport::CreatePanelByName( const char *szPanelName )
{
	IViewPortPanel* newpanel = nullptr;

	if ( V_strcmp( PANEL_TEAM, szPanelName ) == 0 )
	{
		//newpanel = new CNeoTeamMenu( this );
	}

	else if ( V_strcmp( PANEL_SCOREBOARD, szPanelName ) == 0 )
	{
		//newpanel = new CNEOClientScoreBoardDialog( this );
	}

	else if ( V_strcmp( PANEL_CLASS, szPanelName ) == 0 )
	{
		//newpanel = new CNeoClassMenu( this );
	}

	else if ( V_strcmp( PANEL_LOADOUT_DEV, szPanelName ) == 0 )
	{
		//newpanel = new CNeoLoadoutMenu_Dev( this );
	}

	else if ( V_strcmp( PANEL_LOADOUT, szPanelName ) == 0 )
	{
		//newpanel = new CNeoLoadoutMenu( this );
	}

	else
	{
		// create a generic base panel, don't add twice
		newpanel = BaseClass::CreatePanelByName( szPanelName );
	}

	return newpanel;
}

void NEOViewport::CreateDefaultPanels()
{
	BaseClass::CreateDefaultPanels();

	AddNewPanel( CreatePanelByName( PANEL_TEAM ), "PANEL_TEAM" );
	AddNewPanel( CreatePanelByName( PANEL_CLASS ), "PANEL_CLASS" );
	AddNewPanel( CreatePanelByName( PANEL_LOADOUT_DEV ), "PANEL_LOADOUT_DEV" );
	AddNewPanel( CreatePanelByName( PANEL_LOADOUT ), "PANEL_LOADOUT" );
}

void NEOViewport::Start( IGameUIFuncs *pGameUIFuncs, IGameEventManager2 *pGameEventManager )
{
	BaseClass::Start( pGameUIFuncs, pGameEventManager );
}

int NEOViewport::GetDeathMessageStartHeight()
{
	int y = YRES( 2 );

	auto specGui = gViewPortInterface->FindPanelByName( PANEL_SPECGUI );
		
	if ( specGui && specGui->IsVisible() )
		y += YRES( 52 );

	return y;
}