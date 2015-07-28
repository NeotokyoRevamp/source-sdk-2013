#ifndef	BASENEOGRENADE_H
#define	BASENEOGRENADE_H

#ifdef _WIN32
#pragma once
#endif

#include "basegrenade_shared.h"

class CBaseNeoGrenade : public CBaseGrenade
{
public:
	DECLARE_CLASS(CBaseNeoGrenade, CBaseGrenade);
#if !defined( CLIENT_DLL )
	DECLARE_DATADESC();
#endif

	CBaseNeoGrenade();
};

#endif	//BASENEOGRENADE_H