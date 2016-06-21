#include "cbase.h"
#include "neoclassmenu.h"
#include "clientmode_neonormal.h"
#include "c_neoplayer.h"
#include "neoimagebutton.h"
#include "IGameUIFuncs.h"

static const char* s_ClassMenuClassesJinrai[ 18 ]
{
	"vgui/cm/none",
	"vgui/cm/none",
	"vgui/cm/none",
	"vgui/cm/jinrai_scout01",
	"vgui/cm/jinrai_scout02",
	"vgui/cm/jinrai_scout03",
	"vgui/cm/jinrai_assault01",
	"vgui/cm/jinrai_assault02",
	"vgui/cm/jinrai_assault03",
	"vgui/cm/jinrai_heavy01",
	"vgui/cm/jinrai_heavy02",
	"vgui/cm/jinrai_heavy03",
	"vgui/cm/none",
	"vgui/cm/none",
	"vgui/cm/none",
	"vgui/cm/none",
	"vgui/cm/none",
	"vgui/cm/none"
};

static const char* s_ClassMenuClassesNsf[ 18 ]
{
	"vgui/cm/none",
	"vgui/cm/none",
	"vgui/cm/none",
	"vgui/cm/nsf_scout01",
	"vgui/cm/nsf_scout02",
	"vgui/cm/nsf_scout03",
	"vgui/cm/nsf_assault01",
	"vgui/cm/nsf_assault02",
	"vgui/cm/nsf_assault03",
	"vgui/cm/nsf_heavy01",
	"vgui/cm/nsf_heavy02",
	"vgui/cm/nsf_heavy03",
	"vgui/cm/none",
	"vgui/cm/none",
	"vgui/cm/none",
	"vgui/cm/none",
	"vgui/cm/none",
	"vgui/cm/none"
};

// This can get really a lot simpler, let's keep it this way for now. Also we need to think on a better name
const char* GetNameOfClass( int teamId, int classId, int variant )
{
	if ( teamId == 2 && classId < 6 && variant < 3 )
	{
		return s_ClassMenuClassesJinrai[ classId + variant + 2 * classId ];
	}

	else if ( teamId == 3 && classId < 6 && variant < 3 )
	{
		return s_ClassMenuClassesNsf[ classId + variant + 2 * classId ];
	}

	return "vgui/cm/none";
}


CNeoClassMenu::CNeoClassMenu( IViewPort *pViewPort ) : CNeoFrame( PANEL_CLASS )
{
	m_pViewPort = pViewPort;

	m_iJumpKey = 0;
	m_iScoreboardKey = 0;

	m_bNeedsUpdate = false;

	LoadControlSettings( "Resource/UI/ClassMenu.res" );
	InvalidateLayout();

	m_fCreationTime = gpGlobals->curtime;
}

CNeoClassMenu::~CNeoClassMenu()
{
}

void CNeoClassMenu::ApplySchemeSettings( vgui::IScheme* pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );
}

void CNeoClassMenu::OnCommand( const char* command )
{
	if ( GetClientModeNEONormal()->IsNEODev() )
		engine->ClientCmd( "iamantdev" );

	if ( !V_stricmp( "vguicancel", command ) || !V_stricmp( "Close", command ) )
	{
		Close();
		gViewPortInterface->ShowBackGround( false );
	}
	
	else if ( !V_stricmp( "playerready", command ) )
	{
		engine->ClientCmd( command );
		Close();
		gViewPortInterface->ShowBackGround( false );
	}

	else if ( !V_stricmp( "SetVariant 0", command ) )
	{
		C_NEOPlayer::GetLocalNEOPlayer()->m_iOldClassType = 0;

		if ( m_bUnknown )
		{
			engine->ClientCmd( command );
			Close();
			gViewPortInterface->ShowBackGround( false );
		}
	}

	else if ( !V_stricmp( "SetVariant 1", command ) )
	{
		C_NEOPlayer::GetLocalNEOPlayer()->m_iOldClassType = 1;

		if ( m_bUnknown )
		{
			engine->ClientCmd( command );
			Close();
			gViewPortInterface->ShowBackGround( false );
		}
	}

	else if ( !V_stricmp( "SetVariant 2", command ) )
	{
		C_NEOPlayer::GetLocalNEOPlayer()->m_iOldClassType = 2;

		if ( m_bUnknown )
		{
			engine->ClientCmd( command );
			Close();
			gViewPortInterface->ShowBackGround( false );
		}
	}

	else if ( !V_stricmp( "setclass 1", command ) )
	{
		m_bUnknown = true;
		CreateImagePanels( 1 );
		engine->ClientCmd( command );
	}

	else if ( !V_stricmp( "setclass 2", command ) )
	{		
		m_bUnknown = true;
		CreateImagePanels( 2 );
		engine->ClientCmd( command );
	}

	else if ( !V_stricmp( "setclass 3", command ) )
	{
		m_bUnknown = true;
		CreateImagePanels( 3 );
		engine->ClientCmd( command );
	}

	engine->ClientCmd( command );
	BaseClass::OnCommand( command );
}

void CNeoClassMenu::OnKeyCodePressed( vgui::KeyCode code )
{
	if ( !m_iScoreboardKey || m_iScoreboardKey != code )
		BaseClass::OnKeyCodePressed( code );
}

void CNeoClassMenu::Update()
{
	C_NEOPlayer* localPlayer = C_NEOPlayer::GetLocalNEOPlayer();

	if ( localPlayer && (localPlayer->GetTeamNumber() == 2 || localPlayer->GetTeamNumber() == 3) )
	{
		m_bNeedsUpdate = false;
		CreateImagePanels( localPlayer->m_iClassType );
	}
}

void CNeoClassMenu::ShowPanel( bool bShow )
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

		int classId = C_NEOPlayer::GetLocalNEOPlayer()->m_iClassType;

		if ( classId == 0 )
		{
			classId = 1;
			engine->ClientCmd( "setclass 1" );
		}

		m_bUnknown = true;

		CreateImagePanels( classId );
		CreateImagePanels( classId );
	}

	else
	{
		SetVisible( false );
		SetMouseInputEnabled( false );
	}

	m_pViewPort->ShowBackGround( bShow );
}

void CNeoClassMenu::CreateImagePanels( int classId )
{
	if ( classId < 1 )
		classId = 1;

	if ( classId > 3 )
		classId = 3;

	C_NEOPlayer* localPlayer = C_NEOPlayer::GetLocalNEOPlayer();

	if ( !localPlayer )
		return;

	if ( localPlayer->GetTeamNumber() != 2 && localPlayer->GetTeamNumber() != 3 )
	{
		m_bNeedsUpdate = true;
		classId = 0;
	}

	vgui::CNImageButton* imagePanel = (vgui::CNImageButton*) FindChildByName( "Model_ImagePanel" );
	imagePanel->SetTexture( GetNameOfClass( localPlayer->GetTeamNumber(), classId, 0 ) );

	vgui::CNImageButton* imagePanel2 = (vgui::CNImageButton*) FindChildByName( "Model_ImagePanel2" );
	imagePanel2->SetTexture( GetNameOfClass( localPlayer->GetTeamNumber(), classId, 0 ) );

	vgui::CNImageButton* imagePanel3 = (vgui::CNImageButton*) FindChildByName( "Model_ImagePanel3" );
	imagePanel3->SetTexture( GetNameOfClass( localPlayer->GetTeamNumber(), classId, 0 ) );
}