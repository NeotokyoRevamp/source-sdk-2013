#ifndef	HEGRENADE_H
#define	HEGRENADE_H

#ifdef _WIN32
#pragma once
#endif

#include "grenade_baseneogrenade.h"

class CHEGrenade : public CBaseNeoGrenade
{
public:
	DECLARE_CLASS(CHEGrenade, CBaseNeoGrenade);
#if !defined( CLIENT_DLL )
	DECLARE_DATADESC();
#endif

	CHEGrenade();
};

#endif	//HEGRENADE_H