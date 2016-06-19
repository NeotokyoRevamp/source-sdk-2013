#if !defined( CLIENTMODE_NEONORMAL_H )
#define CLIENTMODE_NEONORMAL_H

#ifdef _WIN32
#pragma once
#endif

#include "clientmode_shared.h"

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class ClientModeNEONormal : public ClientModeShared
{
public:
	DECLARE_CLASS( ClientModeNEONormal, ClientModeShared );

	ClientModeNEONormal();
	~ClientModeNEONormal();

public:
	virtual void	InitViewport();
	virtual void	Init();
	virtual void	Shutdown();
	virtual void	LevelInit( const char *newmap );
	virtual void	LevelShutdown();
	virtual float	GetViewModelFOV();

	bool IsNEODev();

private:
	bool m_bUnknown;
	bool m_bIsNEODev;
};

inline bool ClientModeNEONormal::IsNEODev()
{
	return m_bIsNEODev;
}

extern IClientMode* GetClientModeNormal();
extern ClientModeNEONormal* GetClientModeNEONormal();

#endif // CLIENTMODE_NEONORMAL_H