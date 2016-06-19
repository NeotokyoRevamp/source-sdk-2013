#ifndef NEO_CLASSMENU_H
#define NEO_CLASSMENU_H

#ifdef _WIN32
#pragma once
#endif

#include "neoframe.h"
#include <game/client/iviewport.h>

class CNeoClassMenu : public vgui::CNeoFrame, public IViewPortPanel
{
	DECLARE_CLASS_SIMPLE( CNeoClassMenu, vgui::CNeoFrame );

public:
	CNeoClassMenu();
	virtual ~CNeoClassMenu();

public:
	virtual bool IsVisible();
	virtual void SetParent( vgui::VPANEL newParent );
	virtual void ApplySchemeSettings( vgui::IScheme* pScheme );
	virtual void OnCommand( const char* command );
	virtual void OnKeyCodePressed( vgui::KeyCode code );

	void CreateImagePanels( int classId );

private:
	int m_iUnknown;
	int m_iUnknown2;
	float m_fCreationTime;
	bool m_bUnknown;
	bool m_bUnknown2;
};

#endif // NEO_CLASSMENU_H