//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#include "cbase.h"
#include "hud.h"
#include "hl2mpclientscoreboard.h"
#include "c_team.h"
#include "c_playerresource.h"
#include "c_hl2mp_player.h"
#include "hl2mp_gamerules.h"

#include <KeyValues.h>

#include <vgui/IScheme.h>
#include <vgui/ILocalize.h>
#include <vgui/ISurface.h>
#include <vgui/IVGui.h>
#include <vgui_controls/SectionedListPanel.h>

#include "voice_status.h"

using namespace vgui;

#define TEAM_MAXCOUNT			5

// id's of sections used in the scoreboard
enum EScoreboardSections
{
	SCORESECTION_COMBINE = 1,
	SCORESECTION_REBELS = 2,
	SCORESECTION_FREEFORALL = 3,
	SCORESECTION_SPECTATOR = 4
};

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CHL2MPClientScoreBoardDialog::CHL2MPClientScoreBoardDialog(IViewPort *pViewPort):CClientScoreBoardDialog(pViewPort)
{
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CHL2MPClientScoreBoardDialog::~CHL2MPClientScoreBoardDialog()
{
}

//-----------------------------------------------------------------------------
// Purpose: sets up base sections
//-----------------------------------------------------------------------------
void CHL2MPClientScoreBoardDialog::InitScoreboardSections()
{
	// fill out the structure of the scoreboard
	AddHeader();

	if ( HL2MPRules()->IsTeamplay() )
	{
		// add the team sections
		AddSection( TYPE_TEAM, TEAM_COMBINE );
		AddSection( TYPE_TEAM, TEAM_REBELS );
	}
	else
	{
		AddSection( TYPE_TEAM, TEAM_UNASSIGNED );
	}
	AddSection( TYPE_TEAM, TEAM_SPECTATOR );
}

//-----------------------------------------------------------------------------
// Purpose: resets the scoreboard team info
//-----------------------------------------------------------------------------
void CHL2MPClientScoreBoardDialog::UpdateTeamInfo()
{
	IGameResources *gr = GameResources();
	if ( !gr )
		return;

	int iNumPlayersInGame = 0;

	for ( int j = 1; j <= gpGlobals->maxClients; j++ )
	{	
		if ( g_PR->IsConnected( j ) )
		{
			iNumPlayersInGame++;
		}
	}

	// update the team sections in the scoreboard
	for ( int i = TEAM_SPECTATOR; i < TEAM_MAXCOUNT; i++ )
	{
		wchar_t *teamName = NULL;
		int sectionID = 0;
		C_Team *team = GetGlobalTeam(i);

		if ( team )
		{
			sectionID = GetSectionFromTeamNumber( i );
	
			// update team name
			wchar_t name[64];
			wchar_t string1[1024];
			wchar_t wNumPlayers[6];

			if ( HL2MPRules()->IsTeamplay() == false )
			{
				_snwprintf( wNumPlayers, ARRAYSIZE(wNumPlayers), L"%i", iNumPlayersInGame );
#ifdef WIN32
				_snwprintf( name, ARRAYSIZE(name), L"%s", g_pVGuiLocalize->Find("#ScoreBoard_Deathmatch") );
#else
				_snwprintf( name, ARRAYSIZE(name), L"%S", g_pVGuiLocalize->Find("#ScoreBoard_Deathmatch") );
#endif
				
				teamName = name;

				if ( iNumPlayersInGame == 1)
				{
					g_pVGuiLocalize->ConstructString( string1, sizeof(string1), g_pVGuiLocalize->Find("#ScoreBoard_Player"), 2, teamName, wNumPlayers );
				}
				else
				{
					g_pVGuiLocalize->ConstructString( string1, sizeof(string1), g_pVGuiLocalize->Find("#ScoreBoard_Players"), 2, teamName, wNumPlayers );
				}
			}
			else
			{
				_snwprintf(wNumPlayers, ARRAYSIZE(wNumPlayers), L"%i", team->Get_Number_Players());

				if (!teamName && team)
				{
					g_pVGuiLocalize->ConvertANSIToUnicode(team->Get_Name(), name, sizeof(name));
					teamName = name;
				}

				if (team->Get_Number_Players() == 1)
				{
					g_pVGuiLocalize->ConstructString( string1, sizeof(string1), g_pVGuiLocalize->Find("#ScoreBoard_Player"), 2, teamName, wNumPlayers );
				}
				else
				{
					g_pVGuiLocalize->ConstructString( string1, sizeof(string1), g_pVGuiLocalize->Find("#ScoreBoard_Players"), 2, teamName, wNumPlayers );
				}

				// update stats
				wchar_t val[6];
				V_snwprintf(val, ARRAYSIZE(val), L"%d", team->Get_Score());
				m_pPlayerList->ModifyColumn(sectionID, "frags", val);
				if (team->Get_Ping() < 1)
				{
					m_pPlayerList->ModifyColumn(sectionID, "ping", L"");
				}
				else
				{
					V_snwprintf(val, ARRAYSIZE(val), L"%d", team->Get_Ping());
					m_pPlayerList->ModifyColumn(sectionID, "ping", val);
				}

			}
		
			m_pPlayerList->ModifyColumn(sectionID, "name", string1);
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: adds the top header of the scoreboars
//-----------------------------------------------------------------------------
void CHL2MPClientScoreBoardDialog::AddHeader()
{
	// add the top header
	m_pPlayerList->AddSection(0, "");
	m_pPlayerList->SetSectionAlwaysVisible(0);
	HFont hFallbackFont = scheme()->GetIScheme( GetScheme() )->GetFont( "DefaultVerySmallFallBack", false );
	m_pPlayerList->AddColumnToSection(0, "name", "", 0, scheme()->GetProportionalScaledValueEx( GetScheme(), m_iNameWidth ), hFallbackFont );
	m_pPlayerList->AddColumnToSection(0, "class", "", 0, scheme()->GetProportionalScaledValueEx( GetScheme(), m_iClassWidth ) );
	m_pPlayerList->AddColumnToSection(0, "frags", "#PlayerScore", 0 | SectionedListPanel::COLUMN_RIGHT, scheme()->GetProportionalScaledValueEx( GetScheme(), m_iScoreWidth ) );
	m_pPlayerList->AddColumnToSection(0, "deaths", "#PlayerDeath", 0 | SectionedListPanel::COLUMN_RIGHT, scheme()->GetProportionalScaledValueEx( GetScheme(), m_iDeathWidth ) );
	m_pPlayerList->AddColumnToSection(0, "ping", "#PlayerPing", 0 | SectionedListPanel::COLUMN_RIGHT, scheme()->GetProportionalScaledValueEx( GetScheme(), m_iPingWidth ) );
//	m_pPlayerList->AddColumnToSection(0, "voice", "#PlayerVoice", SectionedListPanel::COLUMN_IMAGE | SectionedListPanel::HEADER_TEXT| SectionedListPanel::COLUMN_CENTER, scheme()->GetProportionalScaledValueEx( GetScheme(), CSTRIKE_VOICE_WIDTH ) );
//	m_pPlayerList->AddColumnToSection(0, "tracker", "#PlayerTracker", SectionedListPanel::COLUMN_IMAGE | SectionedListPanel::HEADER_TEXT, scheme()->GetProportionalScaledValueEx( GetScheme(), CSTRIKE_FRIENDS_WIDTH ) );
}

//-----------------------------------------------------------------------------
// Purpose: Adds a new section to the scoreboard (i.e the team header)
//-----------------------------------------------------------------------------
void CHL2MPClientScoreBoardDialog::AddSection(int teamType, int teamNumber)
{
	HFont hFallbackFont = scheme()->GetIScheme( GetScheme() )->GetFont( "DefaultVerySmallFallBack", false );

	int sectionID = GetSectionFromTeamNumber( teamNumber );
	if ( teamType == TYPE_TEAM )
	{
 		m_pPlayerList->AddSection(sectionID, "", StaticPlayerSortFunc);

		// setup the columns
		if (ShowAvatars())
		{
			m_pPlayerList->AddColumnToSection(sectionID, "avatar", "", SectionedListPanel::COLUMN_IMAGE, m_iAvatarWidth);
		}
		m_pPlayerList->AddColumnToSection(sectionID, "name", "", 0, scheme()->GetProportionalScaledValueEx( GetScheme(), m_iNameWidth ) - m_iAvatarWidth, hFallbackFont );
		m_pPlayerList->AddColumnToSection(sectionID, "class", "" , 0, scheme()->GetProportionalScaledValueEx( GetScheme(), m_iClassWidth ) );
		m_pPlayerList->AddColumnToSection(sectionID, "frags", "", SectionedListPanel::COLUMN_RIGHT, scheme()->GetProportionalScaledValueEx( GetScheme(), m_iScoreWidth ) );
		m_pPlayerList->AddColumnToSection(sectionID, "deaths", "", SectionedListPanel::COLUMN_RIGHT, scheme()->GetProportionalScaledValueEx( GetScheme(), m_iDeathWidth ) );
		m_pPlayerList->AddColumnToSection(sectionID, "ping", "", SectionedListPanel::COLUMN_RIGHT, scheme()->GetProportionalScaledValueEx( GetScheme(), m_iPingWidth ) );

		// set the section to have the team color
		if ( teamNumber )
		{
			if ( GameResources() )
				m_pPlayerList->SetSectionFgColor(sectionID,  GameResources()->GetTeamColor(teamNumber));
		}

		m_pPlayerList->SetSectionAlwaysVisible(sectionID);
	}
	else if ( teamType == TYPE_SPECTATORS )
	{
		m_pPlayerList->AddSection(sectionID, "");
		if (ShowAvatars())
		{
			m_pPlayerList->AddColumnToSection(sectionID, "avatar", "", SectionedListPanel::COLUMN_IMAGE, m_iAvatarWidth);
		}
		m_pPlayerList->AddColumnToSection(sectionID, "name", "#Spectators", 0, scheme()->GetProportionalScaledValueEx( GetScheme(), m_iNameWidth ) - m_iAvatarWidth, hFallbackFont );
	}
}

int CHL2MPClientScoreBoardDialog::GetSectionFromTeamNumber( int teamNumber )
{
	switch ( teamNumber )
	{
	case TEAM_COMBINE:
		return SCORESECTION_COMBINE;
	case TEAM_REBELS:
		return SCORESECTION_REBELS;
	case TEAM_SPECTATOR:
		return SCORESECTION_SPECTATOR;
	default:
		return SCORESECTION_FREEFORALL;
	}
	return SCORESECTION_FREEFORALL;
}

//-----------------------------------------------------------------------------
// Purpose: Adds a new row to the scoreboard, from the playerinfo structure
//-----------------------------------------------------------------------------
bool CHL2MPClientScoreBoardDialog::GetPlayerScoreInfo(int playerIndex, KeyValues *kv)
{
	kv->SetInt("playerIndex", playerIndex);
	kv->SetInt("team", g_PR->GetTeam( playerIndex ) );
	kv->SetString("name", g_PR->GetPlayerName(playerIndex) );
	kv->SetInt("deaths", g_PR->GetDeaths( playerIndex ));
	kv->SetInt("frags", g_PR->GetPlayerScore( playerIndex ));
	kv->SetString("class", "");
	
	if (g_PR->GetPing( playerIndex ) < 1)
	{
		if ( g_PR->IsFakePlayer( playerIndex ) )
		{
			kv->SetString("ping", "BOT");
		}
		else
		{
			kv->SetString("ping", "");
		}
	}
	else
	{
		kv->SetInt("ping", g_PR->GetPing( playerIndex ));
	}
	
	return true;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHL2MPClientScoreBoardDialog::UpdatePlayerInfo()
{
	m_iSectionId = 0; // 0'th row is a header
	int selectedRow = -1;

	CBasePlayer *pPlayer = C_BasePlayer::GetLocalPlayer();

	if ( !pPlayer || !g_PR )
		return;

	// walk all the players and make sure they're in the scoreboard
	for ( int i = 1; i <= gpGlobals->maxClients; i++ )
	{
		IGameResources *gr = GameResources();

		if ( gr && gr->IsConnected( i ) )
		{
			// add the player to the list
			KeyValues *playerData = new KeyValues("data");
			GetPlayerScoreInfo( i, playerData );
			UpdatePlayerAvatar( i, playerData );

			const char *oldName = playerData->GetString( "name", "" );
			char newName[MAX_PLAYER_NAME_LENGTH];

			UTIL_MakeSafeName( oldName, newName, MAX_PLAYER_NAME_LENGTH );

			playerData->SetString("name", newName);

			int itemID = FindItemIDForPlayerIndex( i );
  			int sectionID = GetSectionFromTeamNumber( gr->GetTeam( i ) );
						
			if (itemID == -1)
			{
				// add a new row
				itemID = m_pPlayerList->AddItem( sectionID, playerData );
			}
			else
			{
				// modify the current row
				m_pPlayerList->ModifyItem( itemID, sectionID, playerData );
			}

			if ( gr->IsLocalPlayer( i ) )
			{
				selectedRow = itemID;	// this is the local player, hilight this row
			}

			// set the row color based on the players team
			m_pPlayerList->SetItemFgColor( itemID, gr->GetTeamColor( gr->GetTeam( i ) ) );

			playerData->deleteThis();
		}
		else
		{
			// remove the player
			int itemID = FindItemIDForPlayerIndex( i );
			if (itemID != -1)
			{
				m_pPlayerList->RemoveItem(itemID);
			}
		}
	}

	if ( selectedRow != -1 )
	{
		m_pPlayerList->SetSelectedItem(selectedRow);
	}
}
