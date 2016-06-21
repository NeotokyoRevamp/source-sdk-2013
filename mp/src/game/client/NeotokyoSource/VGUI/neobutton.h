#ifndef NEO_BUTTON_H
#define NEO_BUTTON_H

#ifdef _WIN32
#pragma once
#endif

#include <vgui_controls/Button.h>

namespace vgui
{

class CNButton : public Button
{
	DECLARE_CLASS_SIMPLE( CNButton, Button );

public:
	CNButton( Panel *parent, const char *panelName, const char *text );
	virtual ~CNButton();

public:
	virtual void OnMousePressed( MouseCode code );
	virtual void PaintBackground();
	virtual void PaintBorder();
	virtual void DrawFocusBorder( int tx0, int ty0, int tx1, int ty1 );

private:
	int m_iButtonNormalId;
	int m_iButtonPressedId;
	int m_iButtonMouseoverId;
	int m_iButtonDisabledId;
	int m_iButtonArmedId;

	float m_fUnknown;
};

}

#endif // NEO_BUTTON_H