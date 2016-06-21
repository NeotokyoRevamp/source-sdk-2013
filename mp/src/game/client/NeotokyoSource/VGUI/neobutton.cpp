#include "cbase.h"
#include "neobutton.h"
#include <vgui/ISurface.h>

namespace vgui
{

CNButton::CNButton( Panel *parent, const char *panelName, const char *text ) : Button( parent, panelName, text )
{
	SetPaintBackgroundEnabled( false );

	m_iButtonNormalId = surface()->CreateNewTextureID();
	surface()->DrawSetTextureFile( m_iButtonNormalId, "vgui/buttons/button_normal", true, false );

	m_iButtonPressedId = surface()->CreateNewTextureID();
	surface()->DrawSetTextureFile( m_iButtonPressedId, "vgui/buttons/button_pressed", true, false );

	m_iButtonMouseoverId = surface()->CreateNewTextureID();
	surface()->DrawSetTextureFile( m_iButtonMouseoverId, "vgui/buttons/button_mouseover", true, false );

	m_iButtonDisabledId = surface()->CreateNewTextureID();
	surface()->DrawSetTextureFile( m_iButtonDisabledId, "vgui/buttons/button_disabled", true, false );

	m_iButtonArmedId = surface()->CreateNewTextureID();
	surface()->DrawSetTextureFile( m_iButtonArmedId, "vgui/buttons/button_armed", true, false );

	m_fUnknown = 0.f;
}

CNButton::~CNButton()
{
}

void CNButton::OnMousePressed( MouseCode code )
{
	BaseClass::OnMousePressed( code );
}

void CNButton::PaintBackground()
{
}

void CNButton::PaintBorder()
{
}

void CNButton::DrawFocusBorder( int tx0, int ty0, int tx1, int ty1 )
{
	BaseClass::DrawFocusBorder( tx0, ty0, tx1, ty1 - 2 );
}

}