#include "cbase.h"
#include "neoteammenu.h"
#include "neo_shareddefs.h"
#include "c_team.h" // c_neoteam.h
#include "c_neoplayer.h"
#include "IGameUIFuncs.h"
#include <vgui/ILocalize.h>
#include <vgui_controls\Button.h>
#include <vgui_controls\Label.h>


CNeoTeamMenu::CNeoTeamMenu( IViewPort *pViewPort ) : CNeoFrame( PANEL_TEAM )
{
	m_pViewPort = pViewPort;

	m_iJumpKey = 0;
	m_iScoreboardKey = 0;

	LoadControlSettings( "Resource/UI/TeamMenu.res" );
	InvalidateLayout();

	m_fCreationTime = gpGlobals->curtime;
}

CNeoTeamMenu::~CNeoTeamMenu()
{
}

void CNeoTeamMenu::ApplySchemeSettings( vgui::IScheme* pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );
}

void CNeoTeamMenu::OnCommand( const char* command )
{
	if ( V_stricmp( command, "vguicancel" ) && V_stricmp( command, "Close" ) )
		engine->ClientCmd( command );

	Close();
	gViewPortInterface->ShowBackGround( false );
	
	BaseClass::OnCommand( command );
}

void CNeoTeamMenu::OnKeyCodePressed( vgui::KeyCode code )
{
	if ( m_iJumpKey && m_iJumpKey == code )
		engine->ClientCmd( "jointeam 0" );

	if ( !m_iScoreboardKey || m_iScoreboardKey != code )
		BaseClass::OnKeyCodePressed( code );
}

void CNeoTeamMenu::Update()
{
	wchar_t buffer[ 1024 ];
	wchar_t buffer2[ 6 ];

	vgui::Label* jplayerCountLabel = dynamic_cast< vgui::Label* >( FindChildByName( "jplayercountlabel" ) );
	vgui::Label* jScoreLabel = dynamic_cast< vgui::Label* >(FindChildByName( "jscorelabel" ));
	vgui::Label* nplayerCountLabel = dynamic_cast< vgui::Label* >(FindChildByName( "nplayercountlabel" ));
	vgui::Label* nScoreLabel = dynamic_cast< vgui::Label* >(FindChildByName( "nscorelabel" ));

	/*C_NEOTeam*/ C_Team* jinraiTeam = GetGlobalTeam( TEAM_JINRAI );

	if ( jinraiTeam )
	{		
		if ( jinraiTeam->GetNumPlayers() > 0 )
		{
			g_pVGuiLocalize->ConvertANSIToUnicode( "PLAYERS: ", buffer, sizeof( buffer ) );

			V_snwprintf( buffer2, V_wcslen( buffer2 ), L"%i", jinraiTeam->GetNumPlayers() );

			V_wcscat( buffer, buffer2, V_wcslen( buffer ) );
		}

		else
			g_pVGuiLocalize->ConvertANSIToUnicode( "PLAYERS: 0", buffer, sizeof( buffer ) );

		jplayerCountLabel->SetText( buffer );

		//if ( jinraiTeam->GetTeamScore() > 0 ) // This seems to be a virtual from C_NEOTeam, gotta reverse it
		//{
			g_pVGuiLocalize->ConvertANSIToUnicode( "SCORE: ", buffer, sizeof( buffer ) );

			V_snwprintf( buffer2, V_wcslen( buffer2 ), L"%i"/*, jinraiTeam->GetTeamScore()*/ );

			V_wcscat( buffer, buffer2, V_wcslen( buffer ) );
		//}

		//else
			g_pVGuiLocalize->ConvertANSIToUnicode( "SCORE: 0", buffer, sizeof( buffer ) );

		jScoreLabel->SetText( buffer );
	}

	/*C_NEOTeam*/ C_Team* nsfTeam = GetGlobalTeam( TEAM_NSF );

	if ( nsfTeam )
	{
		if ( nsfTeam->GetNumPlayers() > 0 )
		{
			g_pVGuiLocalize->ConvertANSIToUnicode( "PLAYERS: ", buffer, sizeof( buffer ) );

			V_snwprintf( buffer2, V_wcslen( buffer2 ), L"%i", nsfTeam->GetNumPlayers() );

			V_wcscat( buffer, buffer2, V_wcslen( buffer ) );
		}

		else
			g_pVGuiLocalize->ConvertANSIToUnicode( "PLAYERS: 0", buffer, sizeof( buffer ) );

		nplayerCountLabel->SetText( buffer );

		//if ( nsfTeam->GetTeamScore() > 0 ) 
		//{
			g_pVGuiLocalize->ConvertANSIToUnicode( "SCORE: ", buffer, sizeof( buffer ) );

			V_snwprintf( buffer2, V_wcslen( buffer2 ), L"%i"/*, nsfTeam->GetTeamScore()*/ );

			V_wcscat( buffer, buffer2, V_wcslen( buffer ) );
		//}

		//else
			g_pVGuiLocalize->ConvertANSIToUnicode( "SCORE: 0", buffer, sizeof( buffer ) );

		nScoreLabel->SetText( buffer );
	}
}

void CNeoTeamMenu::ShowPanel( bool bShow )
{
	gViewPortInterface->ShowPanel( PANEL_SCOREBOARD, false );

	if ( IsVisible() == bShow )
		return;

	if ( bShow )
	{
		Activate();
		SetMouseInputEnabled( true );

		if ( !m_iJumpKey )
			m_iJumpKey = gameuifuncs->GetButtonCodeForBind( "jump" );

		if ( !m_iScoreboardKey )
			m_iScoreboardKey = gameuifuncs->GetButtonCodeForBind( "showscores" );

		MoveToCenterOfScreen();

		vgui::Button* cancelButton = dynamic_cast< vgui::Button* >( FindChildByName( "CancelButton" ) );

		cancelButton->SetEnabled( C_NEOPlayer::GetLocalNEOPlayer()->GetTeamNumber() != TEAM_UNASSIGNED );
	}

	else
	{
		SetVisible( false );
		SetMouseInputEnabled( false );
	}

	m_pViewPort->ShowBackGround( bShow );
}