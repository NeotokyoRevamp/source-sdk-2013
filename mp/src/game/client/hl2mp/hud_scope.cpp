// Source: https://developer.valvesoftware.com/wiki/Adding_a_Scope

#include "cbase.h"
#include "hudelement.h"
#include "hud_macros.h"
#include "iclientmode.h"
#include "c_basehlplayer.h" //alternative #include "c_baseplayer.h"
 
#include <vgui/IScheme.h>
#include <vgui/ISurface.h>
#include <vgui_controls/Panel.h>
 
// memdbgon must be the last include file in a .cpp file!
#include "tier0/memdbgon.h"
 
/**
 * Simple HUD element for displaying a sniper scope on screen
 */
class CHudScope : public vgui::Panel, public CHudElement
{
	DECLARE_CLASS_SIMPLE( CHudScope, vgui::Panel );
 
public:
	CHudScope( const char *pElementName );
 
	void Init();
	void MsgFunc_ShowScope( bf_read &msg );
 
protected:
	virtual void ApplySchemeSettings(vgui::IScheme *scheme);
	virtual void Paint( void );
 
private:
	bool			m_bShow;
    CHudTexture*	m_pScope;
};
 
DECLARE_HUDELEMENT( CHudScope );
DECLARE_HUD_MESSAGE( CHudScope, ShowScope );
 
using namespace vgui;
 
/**
 * Constructor - generic HUD element initialization stuff. Make sure our 2 member variables
 * are instantiated.
 */
CHudScope::CHudScope( const char *pElementName ) : CHudElement(pElementName), BaseClass(NULL, "HudScope")
{
	vgui::Panel *pParent = g_pClientMode->GetViewport();
	SetParent( pParent );
 
	m_bShow = false;
	m_pScope = 0;
 
	// Scope will not show when the player is dead
	SetHiddenBits( HIDEHUD_PLAYERDEAD );
 
        // fix for users with diffrent screen ratio (Lodle)
	int screenWide, screenTall;
	GetHudSize(screenWide, screenTall);
	SetBounds(0, 0, screenWide, screenTall);
 
}
 
/**
 * Hook up our HUD message, and make sure we are not showing the scope
 */
void CHudScope::Init()
{
	HOOK_HUD_MESSAGE( CHudScope, ShowScope );
 
	m_bShow = false;
}
 
/**
 * Load  in the scope material here
 */
void CHudScope::ApplySchemeSettings( vgui::IScheme *scheme )
{
	BaseClass::ApplySchemeSettings(scheme);
 
	SetPaintBackgroundEnabled(false);
	SetPaintBorderEnabled(false);
 
	if (!m_pScope)
	{
		m_pScope = gHUD.GetIcon("scope");
	}
}
 
/**
 * Simple - if we want to show the scope, draw it. Otherwise don't.
 */
void CHudScope::Paint( void )
{
        C_BasePlayer* pPlayer = C_BasePlayer::GetLocalPlayer();
        if (!pPlayer)
        {
                return;
        }
 
	if (m_bShow)
	{
                //Perform depth hack to prevent clips by world
	        //materials->DepthRange( 0.0f, 0.1f );
 
		// Draw the scope as a perfect square. Then, fill both sides according to aspect ratio.
		// This way the aspect ratio will not affect our scope's visibility.
		int x1 = (GetWide() / 2) - (GetTall() * 1.14 / 2); // Scope texture compensates for widescreen stretch, so he have undo that here (height -14%)
		int x2 = GetWide() - (x1 * 2);
		int x3 = GetWide() - x1;

		// TODO: Create colour slide textures for the sides to match the scope's outer colouring
		surface()->DrawSetColor(Color(0,0,0,255));
		surface()->DrawFilledRect(0, 0, x1, GetTall()); // Fill in the left side
 
		surface()->DrawSetColor(Color(0,0,0,255));
		surface()->DrawFilledRect(x3, 0, GetWide(), GetTall()); // Fill in the right side

		m_pScope->DrawSelf(x1, 0, x2, GetTall(), Color(255,255,255,255)); // Draw the scope as a perfect square


 
	        //Restore depth
	        //materials->DepthRange( 0.0f, 1.0f );
 
		// Hide the crosshair
  	        pPlayer->m_Local.m_iHideHUD |= HIDEHUD_CROSSHAIR;
	}
	else if ((pPlayer->m_Local.m_iHideHUD & HIDEHUD_CROSSHAIR) != 0)
	{
		pPlayer->m_Local.m_iHideHUD &= ~HIDEHUD_CROSSHAIR;
	}
}
 
 
/**
 * Callback for our message - set the show variable to whatever
 * boolean value is received in the message
 */
void CHudScope::MsgFunc_ShowScope(bf_read &msg)
{
	m_bShow = msg.ReadByte();
}