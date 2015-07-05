#ifndef	DETPACK_H
#define	DETPACK_H

#ifdef _WIN32
#pragma once
#endif

#include "basegrenade_shared.h"

class CDetpack : public CBaseGrenade
{
public:
	DECLARE_CLASS(CDetpack, CBaseGrenade);

	void Spawn(void);
	void Precache(void);

	// Input handlers
	void InputExplode(inputdata_t &inputdata);

	CDetpack();

	DECLARE_DATADESC();
};

#endif	//DETPACK_H