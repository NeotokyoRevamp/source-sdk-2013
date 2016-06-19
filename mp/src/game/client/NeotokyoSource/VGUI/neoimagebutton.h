#ifndef NEO_IMAGEBUTTON_H
#define NEO_IMAGEBUTTON_H

#ifdef _WIN32
#pragma once
#endif

#include <vgui_controls/Button.h>

namespace vgui
{

class CNImageButton : public Button
{
	DECLARE_CLASS_SIMPLE( CNImageButton, Button );

public:
	CNImageButton( Panel *parent, const char *panelName, const char *text );
	virtual ~CNImageButton();

public:
	virtual void ApplySettings( KeyValues *inResourceData );
	virtual void OnMousePressed( MouseCode code );
	virtual void PaintBackground();
	virtual void PaintBorder();
	virtual void DrawFocusBorder( int tx0, int ty0, int tx1, int ty1 );

	void SetTexture( const char* textureName );

private:
	int m_iTextureID;
	bool m_bUnknown;
};

}

#endif // NEO_IMAGEBUTTON_H