#ifndef NEO_TEAMMENU_H
#define NEO_TEAMMENU_H

#ifdef _WIN32
#pragma once
#endif

#include "neoframe.h"
#include <game/client/iviewport.h>

class CNeoTeamMenu : public vgui::CNeoFrame, public IViewPortPanel
{
	DECLARE_CLASS_SIMPLE( CNeoTeamMenu, vgui::CNeoFrame );

public:
	CNeoTeamMenu( IViewPort *pViewPort );
	virtual ~CNeoTeamMenu();

	virtual const char *GetName( void ) { return PANEL_TEAM; }
	virtual void SetData( KeyValues *data ) {};
	virtual void Reset() {};
	virtual void Update();
	virtual bool NeedsUpdate( void ) { return true; }
	virtual bool HasInputElements( void ) { return true; }
	virtual void ShowPanel( bool bShow );

	vgui::VPANEL GetVPanel( void ) { return BaseClass::GetVPanel(); }
	virtual bool IsVisible() { return BaseClass::IsVisible(); }
	virtual void SetParent( vgui::VPANEL parent ) { BaseClass::SetParent( parent ); }

	virtual void ApplySchemeSettings( vgui::IScheme* pScheme );
	virtual void OnCommand( const char* command );
	virtual void OnKeyCodePressed( vgui::KeyCode code );

private:
	IViewPort* m_pViewPort;
	int m_iJumpKey;
	int m_iScoreboardKey;
	float m_fCreationTime;
};

#endif // NEO_TEAMMENU_H