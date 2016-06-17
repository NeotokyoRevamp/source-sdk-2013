#ifndef NEOVIEWPORT_H
#define NEOVIEWPORT_H

#ifdef _WIN32
#pragma once
#endif

#include "baseviewport.h"

using namespace vgui;

namespace vgui 
{
	class Panel;
	class Label;
	class CBitmapImagePanel;
}


class NEOViewport : public CBaseViewport
{
private:
	DECLARE_CLASS_SIMPLE( NEOViewport, CBaseViewport );

public:
	virtual void ApplySchemeSettings( vgui::IScheme *pScheme );
	virtual IViewPortPanel* CreatePanelByName( const char *szPanelName );
	virtual void CreateDefaultPanels();
	virtual void Start( IGameUIFuncs *pGameUIFuncs, IGameEventManager2 *pGameEventManager );
	virtual int GetDeathMessageStartHeight();
};


#endif // NEOVIEWPORT_H
