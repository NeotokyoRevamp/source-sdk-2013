#include "cbase.h"
#include "neoclientscoreboard.h"
#include <igameresources.h>
#include "neo_shareddefs.h"
#include "c_team.h"
#include <vgui_controls\SectionedListPanel.h>
#include "c_neoplayer.h"
#include <vgui/IScheme.h>
#include <vgui/ILocalize.h>
#include <vgui_controls/SectionedListPanel.h>

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

using namespace vgui;

enum EScoreboardSections
{
	SCORESECTION_JINRAI = 1,
	SCORESECTION_NSF,
	SCORESECTION_SPECTATOR
};

enum EScoreboardTeams
{
	SCORETEAM_JINRAI = 1,
	SCORETEAM_NSF,
	SCORETEAM_SPECTATOR
};

static const char* s_PlayerRanks[ 6 ]
{
	"Rankless Dog",
	"Private",
	"Corporal",
	"Sergeant",
	"Lieutenant",
	"Major"
};

static const char* s_PlayerClasses[ 6 ]
{
	"Unassigned",
	"Recon",
	"Assault",
	"Support",
	"Operator",
	"VIP"
};

static const char* s_Stars[ 6 ]
{
	"None",
	"Alpha",
	"Bravo",
	"Charlie",
	"Delta",
	"Echo"
};

CNEOClientScoreBoardDialog::CNEOClientScoreBoardDialog( IViewPort *pViewPort ) : CClientScoreBoardDialog( pViewPort )
{
	m_iNumTeams = 2;
}

CNEOClientScoreBoardDialog::~CNEOClientScoreBoardDialog()
{
}

bool CNEOClientScoreBoardDialog::GetPlayerScoreInfo( int playerIndex, KeyValues *outPlayerInfo )
{
	IGameResources* gr = GameResources();

	if ( !gr )
		return false;

	outPlayerInfo->SetInt( "deaths", gr->GetDeaths( playerIndex ) );
	outPlayerInfo->SetInt( "frags", gr->GetFrags( playerIndex ) );
	outPlayerInfo->SetInt( "ping", gr->GetPing( playerIndex ) );
	outPlayerInfo->SetString( "name", gr->GetPlayerName( playerIndex ) );
	outPlayerInfo->SetString( "rank", s_PlayerRanks[ gr->GetRank() ] );
	outPlayerInfo->SetString( "alive", gr->IsAlive( playerIndex ) ? "Alive" : "Dead" );
	outPlayerInfo->SetString( "class", s_PlayerClasses[ gr->GetClass() ] );
	outPlayerInfo->SetString( "star", s_Stars[ gr->GetStar() ] );
	outPlayerInfo->SetInt( "playerIndex", playerIndex );

	return true;
}

void CNEOClientScoreBoardDialog::InitScoreboardSections()
{
	AddHeader();
	AddSection( TYPE_TEAM, SCORETEAM_JINRAI ); // The ids are different from TEAM_* constants so I have my doubts about this
	AddSection( TYPE_TEAM, SCORETEAM_NSF );
	AddSection( TYPE_PLAYERS, SCORETEAM_SPECTATOR );
}

void CNEOClientScoreBoardDialog::UpdateTeamInfo()
{
	wchar_t buffer[ 1024 ];

	for ( int teamIndex = TEAM_JINRAI; teamIndex <= TEAM_NSF; teamIndex++ )
	{
		C_Team* team = GetGlobalTeam( teamIndex );

		if ( team )
		{
			switch ( teamIndex )
			{
				case TEAM_JINRAI:
					V_snwprintf( buffer, sizeof( buffer ), L"JINRAI          Score: %d     Players: %d", team->Get_Score(), team->Get_Number_Players() );
					m_pPlayerList->ModifyColumn( SCORESECTION_JINRAI, "name", buffer );
					break;

				case TEAM_NSF:
					V_snwprintf( buffer, sizeof( buffer ), L"JNSF              Score: %d     Players: %d", team->Get_Score(), team->Get_Number_Players() );
					m_pPlayerList->ModifyColumn( SCORESECTION_NSF, "name", buffer );
					break;

				default:
					Assert( false );
			}
		}
	}
}

void CNEOClientScoreBoardDialog::UpdatePlayerInfo()
{
	m_iSectionId = 0; // 0'th row is a header
	int selectedRow = -1;

	// walk all the players and make sure they're in the scoreboard
	for ( int i = 1; i <= gpGlobals->maxClients; ++i )
	{
		IGameResources *gr = GameResources();
		C_NEOPlayer* localPlayer = C_NEOPlayer::GetLocalNEOPlayer();

		if ( gr && gr->IsConnected( i ) && localPlayer )
		{
			// add the player to the list
			KeyValues *playerData = new KeyValues( "data" );
			GetPlayerScoreInfo( i, playerData );
			UpdatePlayerAvatar( i, playerData );

			const char *oldName = playerData->GetString( "name", "" );
			char newName[ MAX_PLAYER_NAME_LENGTH ];

			UTIL_MakeSafeName( oldName, newName, MAX_PLAYER_NAME_LENGTH );

			playerData->SetString( "name", newName );

			int itemID = FindItemIDForPlayerIndex( i );
			int sectionID = gr->GetTeam( i );

			int team = 0;

			if ( sectionID == TEAM_JINRAI )
				team = 1;
			else if ( sectionID == TEAM_NSF )
				team = 2;
			else if ( sectionID == TEAM_SPECTATOR || !team )
				team = 3;

			int teamNum = localPlayer->GetTeamNumber();

			if ( gr->IsLocalPlayer( i ) )
			{	
				selectedRow = itemID;
			}
			if ( itemID == -1 )
			{
				if ( teamNum != TEAM_INVALID &&  teamNum != sectionID )
				{
					playerData->SetString( "alive", "" );
					playerData->SetString( "class", "" );
					playerData->SetString( "star", "" );
				}

				// add a new row
				itemID = m_pPlayerList->AddItem( sectionID, playerData );
			}
			else
			{
				if ( teamNum != TEAM_INVALID &&  teamNum != sectionID )
				{
					playerData->SetString( "alive", "" );
					playerData->SetString( "class", "" );
					playerData->SetString( "star", "" );
				}

				// modify the current row
				m_pPlayerList->ModifyItem( itemID, sectionID, playerData );
			}

			// set the row color based on the players team
			m_pPlayerList->SetItemFgColor( itemID, gr->GetTeamColor( sectionID ) );

			playerData->deleteThis();
		}
		else
		{
			// remove the player
			int itemID = FindItemIDForPlayerIndex( i );
			if ( itemID != -1 )
			{
				m_pPlayerList->RemoveItem( itemID );
			}
		}
	}

	if ( selectedRow != -1 )
	{
		m_pPlayerList->SetSelectedItem( selectedRow );
	}
}

void CNEOClientScoreBoardDialog::AddHeader()
{
	// add the top header
	m_pPlayerList->AddSection( 0, "" );
	m_pPlayerList->SetSectionAlwaysVisible( 0 );
	m_pPlayerList->AddColumnToSection( 0, "name", "", 0, scheme()->GetProportionalScaledValueEx( GetScheme(), NEO_NAME_WIDTH ) );
	m_pPlayerList->AddColumnToSection( 0, "rank", "Rank", 0, scheme()->GetProportionalScaledValueEx( GetScheme(), NEO_RANK_WIDTH ) );
	m_pPlayerList->AddColumnToSection( 0, "class", "Class", 0, scheme()->GetProportionalScaledValueEx( GetScheme(), NEO_CLASS_WIDTH ) );
	m_pPlayerList->AddColumnToSection( 0, "star", "Star", 0, scheme()->GetProportionalScaledValueEx( GetScheme(), NEO_STAR_WIDTH ) );
	m_pPlayerList->AddColumnToSection( 0, "frags", "XP", 0, scheme()->GetProportionalScaledValueEx( GetScheme(), NEO_FRAGS_WIDTH ) );
	m_pPlayerList->AddColumnToSection( 0, "deaths", "Deaths", 0, scheme()->GetProportionalScaledValueEx( GetScheme(), NEO_DEATH_WIDTH ) );
	m_pPlayerList->AddColumnToSection( 0, "ping", "Ping", 0, scheme()->GetProportionalScaledValueEx( GetScheme(), NEO_PING_WIDTH ) );
	m_pPlayerList->AddColumnToSection( 0, "alive", "Status", 0, scheme()->GetProportionalScaledValueEx( GetScheme(), NEO_ALIVE_WIDTH ) );
}

void CNEOClientScoreBoardDialog::AddSection( int teamType, int teamNumber )
{
	if ( teamType == TYPE_TEAM )
	{
		int sectionId = 0;

		if ( teamNumber == SCORETEAM_JINRAI )
			sectionId = SCORESECTION_JINRAI;
		else if ( teamNumber == SCORETEAM_NSF )
			sectionId = SCORESECTION_NSF;

		m_pPlayerList->AddSection( sectionId, "", StaticPlayerSortFunc );

		// Avatars are always displayed at 32x32 regardless of resolution
		if ( ShowAvatars() )
		{
			m_pPlayerList->AddColumnToSection( sectionId, "avatar", "", SectionedListPanel::COLUMN_IMAGE | SectionedListPanel::COLUMN_RIGHT, m_iAvatarWidth );
		}

		m_pPlayerList->AddColumnToSection( sectionId, "name", "", 0, scheme()->GetProportionalScaledValueEx( GetScheme(), NEO_NAME_WIDTH ) - m_iAvatarWidth );
		m_pPlayerList->AddColumnToSection( sectionId, "rank", "", 0, scheme()->GetProportionalScaledValueEx( GetScheme(), NEO_RANK_WIDTH ) );
		m_pPlayerList->AddColumnToSection( sectionId, "class", "", 0, scheme()->GetProportionalScaledValueEx( GetScheme(), NEO_CLASS_WIDTH ) );
		m_pPlayerList->AddColumnToSection( sectionId, "star", "", 0, scheme()->GetProportionalScaledValueEx( GetScheme(), NEO_STAR_WIDTH ) );
		m_pPlayerList->AddColumnToSection( sectionId, "frags", "", 0, scheme()->GetProportionalScaledValueEx( GetScheme(), NEO_FRAGS_WIDTH ) );
		m_pPlayerList->AddColumnToSection( sectionId, "deaths", "", 0, scheme()->GetProportionalScaledValueEx( GetScheme(), NEO_DEATH_WIDTH ) );
		m_pPlayerList->AddColumnToSection( sectionId, "ping", "", 0, scheme()->GetProportionalScaledValueEx( GetScheme(), NEO_PING_WIDTH ) );
		m_pPlayerList->AddColumnToSection( sectionId, "alive", "", 0, scheme()->GetProportionalScaledValueEx( GetScheme(), NEO_ALIVE_WIDTH ) );
	}
	else if ( teamType == TYPE_SPECTATORS )
	{
		m_pPlayerList->AddSection( SCORESECTION_SPECTATOR, "" );

		// Avatars are always displayed at 32x32 regardless of resolution
		if ( ShowAvatars() )
		{
			m_pPlayerList->AddColumnToSection( SCORESECTION_SPECTATOR, "avatar", "", SectionedListPanel::COLUMN_IMAGE | SectionedListPanel::COLUMN_RIGHT, m_iAvatarWidth );
		}
		m_pPlayerList->AddColumnToSection( SCORESECTION_SPECTATOR, "name", "#Spectators", 0, scheme()->GetProportionalScaledValueEx( GetScheme(), NAME_WIDTH ) - m_iAvatarWidth );
	}
}

void CNEOClientScoreBoardDialog::UpdateServerInfo( KeyValues* kv )
{
	if ( !V_strcmp( kv->GetName(), "hltv_status" ) )
	{
		m_HLTVSpectators = kv->GetInt( "clients" );
		m_HLTVSpectators -= kv->GetInt( "proxies" );
	}

	else if ( !V_strcmp( kv->GetName(), "server_spawn" ) )
	{
		if ( !V_stricmp( kv->GetString( "hostname" ), "Half-Life" ) )
			SetControlString( "ServerName", "Neotokyo" );
		else
			SetControlString( "ServerName", kv->GetString( "hostname" ) );

		SetLabelColorWhite( "ServerName" );
	}

	if ( IsVisible() )
		Update();
}

void CNEOClientScoreBoardDialog::SetLabelColorWhite( const char* labelName )
{
	vgui::Label* label = dynamic_cast<vgui::Label*>(FindChildByName( labelName ));

	if ( !label )
		return;

	label->SetFgColor( Color( 255, 255, 255, 255 ) );
	label->SetBgColor( Color( 0, 0, 0, 0 ) );
	label->MoveToFront();
}