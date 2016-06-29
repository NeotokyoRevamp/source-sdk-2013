#ifndef NEO_FRAME_H
#define NEO_FRAME_H

#ifdef _WIN32
#pragma once
#endif

#include "vgui_controls\Frame.h"

namespace vgui
{

class CNeoFrame : public Frame
{
	DECLARE_CLASS_SIMPLE( CNeoFrame, Frame );

public:
	CNeoFrame( const char *panelName );
	virtual ~CNeoFrame();

	virtual bool IsVisible();
	virtual void SetParent( VPANEL newParent );
	virtual void ApplySchemeSettings( IScheme *pScheme );
	virtual void PaintBackground();
	virtual void Paint();

private:
	int m_iTextureID1;
	int m_iTextureID2;
	int m_iTextureID3;
};

}

#endif // NEO_FRAME_H