#ifndef NEO_LOADOUTMENUDEV_H
#define NEO_LOADOUTMENUDEV_H

#ifdef _WIN32
#pragma once
#endif

#include <string>
#include "neoframe.h"
#include <game/client/iviewport.h>
#include "neo_shareddefs.h"

class CNeoLoadoutMenu_Dev : public vgui::CNeoFrame, public IViewPortPanel
{
	DECLARE_CLASS_SIMPLE( CNeoLoadoutMenu_Dev, vgui::CNeoFrame );

public:
	CNeoLoadoutMenu_Dev( IViewPort *pViewPort );
	virtual ~CNeoLoadoutMenu_Dev();

	virtual const char *GetName( void ) { return PANEL_LOADOUT_DEV; }
	virtual void SetData( KeyValues *data ) {};
	virtual void Reset() {};
	virtual void Update();
	virtual bool NeedsUpdate( void ) { return m_bNeedsUpdate; }
	virtual bool HasInputElements( void ) { return true; }
	virtual void ShowPanel( bool bShow );

	vgui::VPANEL GetVPanel( void ) { return BaseClass::GetVPanel(); }
	virtual bool IsVisible() { return BaseClass::IsVisible(); }
	virtual void SetParent( vgui::VPANEL parent ) { BaseClass::SetParent( parent ); }

	virtual void ApplySchemeSettings( vgui::IScheme* pScheme );
	virtual void OnCommand( const char* command );
	virtual void OnKeyCodePressed( vgui::KeyCode code );

	virtual void SetWeaponImages();

private:
	IViewPort* m_pViewPort;
	int m_iJumpKey;
	int m_iScoreboardKey;
	float m_fCreationTime;
	bool m_bUnknown;
	bool m_bNeedsUpdate;
};

#endif // NEO_LOADOUTMENUDEV_H