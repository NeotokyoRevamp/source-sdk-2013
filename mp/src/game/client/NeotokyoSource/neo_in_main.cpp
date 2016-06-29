#include "cbase.h"
#include "kbutton.h"
#include "input.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//-----------------------------------------------------------------------------
// Purpose: HL Input interface
//-----------------------------------------------------------------------------
class CNEOInput : public CInput
{
public:
};

static CNEOInput g_Input;

// Expose this interface
IInput *input = ( IInput * )&g_Input;
