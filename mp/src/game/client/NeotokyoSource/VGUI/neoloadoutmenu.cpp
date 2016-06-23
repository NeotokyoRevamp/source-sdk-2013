#include "cbase.h"
#include "neoloadoutmenu.h"
#include "clientmode_neonormal.h"
#include "c_neoplayer.h"
#include "IGameUIFuncs.h"
#include "neoimagebutton.h"

static CLoadoutWeaponClass s_DevLoadoutWeapons[ 14 ]
{
	{ "MPN45", "vgui/loadout/loadout_mpn", "weapon_mpn", "AMMO_PRI" },
	{ "SRM", "vgui/loadout/loadout_srm", "weapon_srm", "AMMO_PRI" },
	{ "SRM (silenced)", "vgui/loadout/loadout_srms", "weapon_srm_s", "AMMO_PRI" },
	{ "Jitte", "vgui/loadout/loadout_jitte", "weapon_jitte", "AMMO_PRI" },
	{ "Jitte (with scope)", "vgui/loadout/loadout_jittes", "weapon_jittescoped", "AMMO_PRI" },
	{ "ZR68-C (compact)", "vgui/loadout/loadout_zr68c", "weapon_zr68c", "AMMO_PRI" },
	{ "ZR68-S (silenced)", "vgui/loadout/loadout_zr68s", "weapon_zr68s", "AMMO_PRI" },
	{ "ZR68-L (accurized)", "vgui/loadout/loadout_zr68l", "weapon_zr68l", "AMMO_PRI" },
	{ "MX", "vgui/loadout/loadout_mx", "weapon_mx", "AMMO_PRI" },
	{ "PZ252", "vgui/loadout/loadout_pz", "weapon_pz", "AMMO_PRI" },
	{ "Murata Supa-7", "vgui/loadout/loadout_supa7", "weapon_supa7", "AMMO_PRI" },
	{ "Mosok", "vgui/loadout/loadout_mosok", "weapon_m41", "AMMO_PRI" },
	{ "Mosok (with scope)", "vgui/loadout/loadout_mosokl", "weapon_m41l", "AMMO_PRI" },
	{ "", "", "", "AMMO_PRI" }
};

static CLoadoutWeaponClass s_ReconLoadoutWeapons[ 10 ]
{
	{ "MPN45", "vgui/loadout/loadout_mpn", "weapon_mpn", "AMMO_PRI" },
	{ "SRM", "vgui/loadout/loadout_srm", "weapon_srm", "AMMO_PRI" },
	{ "Jitte", "vgui/loadout/loadout_jitte", "weapon_jitte", "AMMO_PRI" },
	{ "SRM (silenced)", "vgui/loadout/loadout_srms", "weapon_srm_s", "AMMO_PRI" },
	{ "Jitte (with scope)", "vgui/loadout/loadout_jittes", "weapon_jittescoped", "AMMO_PRI" },
	{ "ZR68-L (accurized)", "vgui/loadout/loadout_zr68l", "weapon_zr68l", "AMMO_PRI" },
	{ "ZR68C", "vgui/loadout/loadout_zr68c", "weapon_zr68c", "AMMO_PRI" },
	{ "Murata Supa-7", "vgui/loadout/loadout_supa7", "weapon_supa7", "AMMO_10G_SHELL" },
	{ "Mosok Silenced", "vgui/loadout/loadout_mosokl", "weapon_m41s", "AMMO_PRI" },
	{ "", "", "", "AMMO_PRI" }
};

static CLoadoutWeaponClass s_AssaultLoadoutWeapons[ 13 ]
{
	{ "MPN45", "vgui/loadout/loadout_mpn", "weapon_mpn", "AMMO_PRI" },
	{ "SRM", "vgui/loadout/loadout_srm", "weapon_srm", "AMMO_PRI" },
	{ "Jitte", "vgui/loadout/loadout_jitte", "weapon_jitte", "AMMO_PRI" },
	{ "ZR68-C (compact)", "vgui/loadout/loadout_zr68c", "weapon_zr68c", "AMMO_PRI" },
	{ "ZR68-S (silenced)", "vgui/loadout/loadout_zr68s", "weapon_zr68s", "AMMO_PRI" },
	{ "Murata Supa-7", "vgui/loadout/loadout_supa7", "weapon_supa7", "AMMO_10G_SHELL" },
	{ "Mosok", "vgui/loadout/loadout_mosok", "weapon_m41", "AMMO_PRI" },
	{ "Mosok Silenced", "vgui/loadout/loadout_mosokl", "weapon_m41s", "AMMO_PRI" },
	{ "MX", "vgui/loadout/loadout_mx", "weapon_mx", "AMMO_PRI" },
	{ "MX Silenced", "vgui/loadout/loadout_mxs", "weapon_mx_silenced", "AMMO_PRI" },
	{ "AA13", "vgui/loadout/loadout_aa13", "weapon_aa13", "AMMO_10G_SHELL" },
	{ "SRS", "vgui/loadout/loadout_srs", "weapon_srs", "AMMO_PRI" },
	{ "", "", "", "AMMO_PRI" }
};

static CLoadoutWeaponClass s_SupportLoadoutWeapons[ 10 ]
{
	{ "MPN45", "vgui/loadout/loadout_mpn", "weapon_mpn", "AMMO_PRI" },
	{ "SRM", "vgui/loadout/loadout_srm", "weapon_srm", "AMMO_PRI" },
	{ "ZR68-C (compact)", "vgui/loadout/loadout_zr68c", "weapon_zr68c", "AMMO_PRI" },
	{ "Mosok", "vgui/loadout/loadout_mosok", "weapon_m41", "AMMO_PRI" },
	{ "Murata Supa-7", "vgui/loadout/loadout_supa7", "weapon_supa7", "AMMO_10G_SHELL" },
	{ "MX", "vgui/loadout/loadout_mx", "weapon_mx", "AMMO_PRI" },
	{ "Mosok Silenced", "vgui/loadout/loadout_mosokl", "weapon_m41s", "AMMO_PRI" },
	{ "MX Silenced", "vgui/loadout/loadout_mxs", "weapon_mx_silenced", "AMMO_PRI" },
	{ "PZ252", "vgui/loadout/loadout_pz", "weapon_pz", "AMMO_5_7MM" },
	{ "", "", "", "AMMO_PRI" }
};

static std::vector< std::string > s_LoadoutMenuButtons
{
	"Button1",
	"Button2",
	"Button3",
	"Button4",
	"Button5",
	"Button6",
	"Button7",
	"Button8",
	"Button9",
	"Button10",
	"Button11",
	"Button12",
};

CLoadoutWeaponClass::CLoadoutWeaponClass( const char* weaponName, const char* vguiImage, const char* weaponEntityName, const char* ammoType )
{
	m_szWeaponName = weaponName;
	m_szVguiImage = vguiImage;
	m_szWeaponEntityName = weaponEntityName;
	m_szAmmoType = ammoType;
}

CLoadoutWeaponClass::~CLoadoutWeaponClass()
{
}

// Gotta rework on these functions, I have a feeling they wont work
int GetNumberOfLoadoutWeapons( int teamNum, int classType, int rank, bool isDev )
{
	CLoadoutWeaponClass** pLoadout = nullptr;

	if ( isDev )
		pLoadout = (CLoadoutWeaponClass**) &s_DevLoadoutWeapons;
	else if ( classType == CLASS_RECON )
		pLoadout = (CLoadoutWeaponClass**) &s_ReconLoadoutWeapons;
	else if ( classType == CLASS_ASSAULT )
		pLoadout = (CLoadoutWeaponClass**) &s_AssaultLoadoutWeapons;
	else if ( classType == CLASS_SUPPORT )
		pLoadout = (CLoadoutWeaponClass**) &s_SupportLoadoutWeapons;
	else
		return 0;

	int ammount = 0;
	int i = 0;

	for ( CLoadoutWeaponClass* l = *pLoadout; l; l = pLoadout[ i ] )
	{
		if ( rank >= i )
			ammount++;
		i++;
	}

	return ammount;
}

std::string GetLoadoutVguiWeaponName( int teamNum, int classType, int rank, int num, bool isDev )
{
	CLoadoutWeaponClass** pLoadout = nullptr;

	if ( isDev )
		pLoadout = (CLoadoutWeaponClass**) &s_DevLoadoutWeapons;
	else if ( classType == CLASS_RECON )
		pLoadout = (CLoadoutWeaponClass**) &s_ReconLoadoutWeapons;
	else if ( classType == CLASS_ASSAULT )
		pLoadout = (CLoadoutWeaponClass**) &s_AssaultLoadoutWeapons;
	else if ( classType == CLASS_SUPPORT )
		pLoadout = (CLoadoutWeaponClass**) &s_SupportLoadoutWeapons;
	else
		return "";

	if ( !*pLoadout )
		return "";

	if ( num >= GetNumberOfLoadoutWeapons( teamNum, classType, rank, isDev ) )
		return "";

	return pLoadout[ num ]->m_szVguiImage;
}

CNeoLoadoutMenu::CNeoLoadoutMenu( IViewPort *pViewPort ) : CNeoFrame( PANEL_TEAM )
{
	m_pViewPort = pViewPort;

	m_iJumpKey = 0;
	m_iScoreboardKey = 0;

	m_bNeedsUpdate = false;

	LoadControlSettings( "Resource/UI/LoadoutMenu.res" );
	InvalidateLayout();

	m_fCreationTime = gpGlobals->curtime;
}

CNeoLoadoutMenu::~CNeoLoadoutMenu()
{
}

void CNeoLoadoutMenu::ApplySchemeSettings( vgui::IScheme* pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );
}

void CNeoLoadoutMenu::OnCommand( const char* command )
{
	if ( GetClientModeNEONormal()->IsNEODev() )
		engine->ClientCmd( "iamantdev" );

	if ( !V_stricmp( command, "vguicancel" ) || !V_stricmp( command, "Close" ) )
	{
		Close();
		gViewPortInterface->ShowBackGround( false );
	}

	else if ( !V_stricmp( command, "playerstate_reverse" ) )
	{
		engine->ClientCmd( command );
		Close();
		gViewPortInterface->ShowBackGround( false );
	}		

	else if ( V_stristr( command, "loadout" ) )
	{
		engine->ClientCmd( command );
		BaseClass::OnCommand( command );		

		Close();
		gViewPortInterface->ShowBackGround( false );

		engine->ClientCmd( "playerready" );
		BaseClass::OnCommand( "playerready" );

		return;
	}
	
	BaseClass::OnCommand( command );
}

void CNeoLoadoutMenu::OnKeyCodePressed( vgui::KeyCode code )
{
	if ( !m_iScoreboardKey || m_iScoreboardKey != code )
		BaseClass::OnKeyCodePressed( code );
}

void CNeoLoadoutMenu::SetWeaponImages()
{
	C_NEOPlayer* localPlayer = C_NEOPlayer::GetLocalNEOPlayer();

	int iLoadoutWeapons = GetNumberOfLoadoutWeapons( localPlayer->GetTeamNumber(), localPlayer->m_iClassType, localPlayer->m_iRank, GetClientModeNEONormal()->IsNEODev() );

	int iLoadoutWeapons2 = GetNumberOfLoadoutWeapons( localPlayer->GetTeamNumber(), localPlayer->m_iClassType, 4, GetClientModeNEONormal()->IsNEODev() );

	for ( int i = 0; i < 12; i++ )
	{
		vgui::CNImageButton* imageButton = (vgui::CNImageButton*) vgui::Panel::FindChildByName( s_LoadoutMenuButtons[ i ].c_str() );

		std::string buttonName;

		if ( iLoadoutWeapons < i )
		{
			// This will set everything as loadout_none first. I don't think I wrote it properly, gotta test
			if ( iLoadoutWeapons2 < i )
			{
				imageButton->SetTexture( "vgui/loadout/loadout_none" );
				imageButton->SetEnabled( false );
				imageButton->SetVisible( true );
				continue;
			}

			// Then it will set the image the "_no" version of every class specific weapon
			buttonName = GetLoadoutVguiWeaponName( localPlayer->GetTeamNumber(), localPlayer->m_iClassType, 4, i, GetClientModeNEONormal()->IsNEODev() );
			buttonName.append( "_no" );

			imageButton->SetEnabled( false );
			imageButton->SetVisible( true );
		}

		// Now we can set the regular textures to the weapons that are available to us
		else
		{
			imageButton->SetEnabled( true );
			imageButton->SetVisible( true );

			buttonName = GetLoadoutVguiWeaponName( localPlayer->GetTeamNumber(), localPlayer->m_iClassType, localPlayer->m_iRank, i, GetClientModeNEONormal()->IsNEODev() );
		}

		imageButton->SetTexture( buttonName.c_str() );
	}
}

void CNeoLoadoutMenu::Update()
{
	C_NEOPlayer* localPlayer = C_NEOPlayer::GetLocalNEOPlayer();

	if ( localPlayer && (localPlayer->GetTeamNumber() == TEAM_JINRAI || localPlayer->GetTeamNumber() == TEAM_NSF) )
		m_bNeedsUpdate = false;
}

void CNeoLoadoutMenu::ShowPanel( bool bShow )
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