#include "cbase.h"
#include "neoloadoutmenu_dev.h"
#include "neoloadoutmenu.h"
#include "clientmode_neonormal.h"
#include "c_neoplayer.h"
#include "IGameUIFuncs.h"
#include "neoimagebutton.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

CNeoLoadoutMenu_Dev::CNeoLoadoutMenu_Dev( IViewPort *pViewPort ) : CNeoFrame( PANEL_TEAM )
{
	m_pViewPort = pViewPort;

	m_iJumpKey = 0;
	m_iScoreboardKey = 0;

	m_bNeedsUpdate = false;

	LoadControlSettings( "Resource/UI/LoadoutMenu.res" );
	InvalidateLayout();

	m_fCreationTime = gpGlobals->curtime;
}

CNeoLoadoutMenu_Dev::~CNeoLoadoutMenu_Dev()
{
}

void CNeoLoadoutMenu_Dev::ApplySchemeSettings( vgui::IScheme* pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );
}

void CNeoLoadoutMenu_Dev::OnCommand( const char* command )
{
	if ( GetClientModeNEONormal()->IsNEODev() )
		engine->ClientCmd( "iamantdev" );

	if ( !V_stricmp( command, "vguicancel" ) || !V_stricmp( command, "Close" ) )
	{
		Close();
		gViewPortInterface->ShowBackGround( false );
	}

	else if ( !V_stricmp( command, "playerready" ) )
	{
		engine->ClientCmd( command );
		Close();
		gViewPortInterface->ShowBackGround( false );
	}		

	else if ( V_stristr( command, "loadout" ) )
	{
	}

	FindChildByName( "Done_Button" )->SetVisible( true );

	vgui::CNImageButton* imagePanel = (vgui::CNImageButton*) FindChildByName( "Weapon_ImagePanel" );

	C_NEOPlayer* localPlayer = C_NEOPlayer::GetLocalNEOPlayer();

	imagePanel->SetTexture( GetLoadoutVguiWeaponName( localPlayer->GetTeamNumber(), localPlayer->m_iClassType, localPlayer->m_iRank, 0, GetClientModeNEONormal()->IsNEODev() ).c_str() );
	
	BaseClass::OnCommand( command );
}

void CNeoLoadoutMenu_Dev::OnKeyCodePressed( vgui::KeyCode code )
{
	if ( !m_iScoreboardKey || m_iScoreboardKey != code )
		BaseClass::OnKeyCodePressed( code );
}

void CNeoLoadoutMenu_Dev::SetWeaponImages()
{
	C_NEOPlayer* localPlayer = C_NEOPlayer::GetLocalNEOPlayer();

	int iLoadoutWeapons = GetNumberOfLoadoutWeapons( localPlayer->GetTeamNumber(), localPlayer->m_iClassType, localPlayer->m_iRank, GetClientModeNEONormal()->IsNEODev() );

	for ( int i = 0; i < 14; i++ )
	{
		vgui::CNImageButton* imageButton = (vgui::CNImageButton*) vgui::Panel::FindChildByName( s_LoadoutMenuButtons[ i ].c_str() );

		if ( iLoadoutWeapons < i )
		{
			imageButton->SetEnabled( false );
			imageButton->SetVisible( false );

			imageButton->SetTexture( "" );
		}

		else
		{
			imageButton->SetEnabled( true );
			imageButton->SetVisible( true );

			imageButton->SetTexture( GetLoadoutVguiWeaponName( localPlayer->GetTeamNumber(), localPlayer->m_iClassType, localPlayer->m_iRank, i, GetClientModeNEONormal()->IsNEODev() ).c_str() );
		}
	}
}

void CNeoLoadoutMenu_Dev::Update()
{
	C_NEOPlayer* localPlayer = C_NEOPlayer::GetLocalNEOPlayer();

	if ( localPlayer && (localPlayer->GetTeamNumber() == TEAM_JINRAI || localPlayer->GetTeamNumber() == TEAM_NSF) )
		m_bNeedsUpdate = false;
}

void CNeoLoadoutMenu_Dev::ShowPanel( bool bShow )
{
	gViewPortInterface->ShowPanel( PANEL_SCOREBOARD, false );

	if ( IsVisible() == bShow )
		return;

	if ( bShow )
	{
		SetWeaponImages();

		Activate();
		SetMouseInputEnabled( true );

		if ( !m_iJumpKey )
			m_iJumpKey = gameuifuncs->GetButtonCodeForBind( "jump" );

		if ( !m_iScoreboardKey )
			m_iScoreboardKey = gameuifuncs->GetButtonCodeForBind( "showscores" );

		MoveToCenterOfScreen();
	}

	else
	{
		SetVisible( false );
		SetMouseInputEnabled( false );
	}

	m_pViewPort->ShowBackGround( bShow );
}