#ifndef C_NEO_PLAYER_H
#define C_NEO_PLAYER_H
#ifdef _WIN32
#pragma once
#endif

#include "neo_playeranimstate.h"
#include "c_baseplayer.h"

class C_NEORagdoll;
class C_NeoMobileArmorBase;

class C_NEOPlayer : public C_BasePlayer, public INEOPlayerAnimStateHelpers
{
public:
	DECLARE_CLASS( C_NEOPlayer, C_BasePlayer );
	DECLARE_CLIENTCLASS();
	DECLARE_PREDICTABLE();
	DECLARE_INTERPOLATION();

	C_NEOPlayer();
	virtual ~C_NEOPlayer();

public:
	// C_BaseEnity implementantions
	virtual void			AddEntity();

	virtual Vector			EyePosition();	 

	// C_BaseAnimating implementations
	virtual C_BaseAnimating* BecomeRagdollOnClient( bool bCopyEntity = true );

	virtual void			UpdateClientSideAnimation();

	virtual void			DoMuzzleFlash();
	virtual void			ProcessMuzzleFlashEvent();

	// C_BasePlayer implementations
	virtual void			CalcView( Vector &eyeOrigin, QAngle &eyeAngles, float &zNear, float &zFar, float &fov );
	virtual void			CalcPlayerView( Vector& eyeOrigin, QAngle& eyeAngles, float& fov );

	virtual void			CreateMove( float flInputSampleTime, CUserCmd *pCmd );

	virtual IRagdoll*		GetRepresentativeRagdoll() const;

	virtual void			PreThink();

	virtual float			GetFOV();
	virtual float			GetMinFOV() const;		

	// NOW FOR THE REAL SHIT
	virtual void			NEO_MuzzleFlash();
	virtual bool			IsWearingMobileArmor(); // I dunno if returns true if he is wearing or owns it.
													// Either way it doesn't seem to be used by the game. Was this going to be implemented or did it get trashed?

	virtual void			GetEyeVectors( Vector *forward, Vector *right, Vector *up ); // Mobile armor is seen here again, with vehicle aswell.
	virtual void			GetEyeVectors2( Vector* something, Vector *forward, Vector *right, Vector *up ); // Pretty much the same as GetEyeVectors, but it uses some vector or angles,
																											 // I'm not sure, and I couldn't bother to reverse it yet

public:
	bool m_bIsVIP;

	//uint8_t NEOPlayerPad00[ 0x10 ];

	int m_iLean;
	int m_iSprint;	  
	float m_fSprintNRG;		
	int m_iThermoptic;

	//uint8_t NEOPlayerPad01[ 0x4 ];

	float m_fThermopticNRG;

	//uint8_t NEOPlayerPad02[ 0x10 ];

	int m_iClassType;
	int m_iOldClassType;

	int m_iRank;
	int m_iOldRank;

	//uint8_t NEOPlayerPad03[ 0x8 ];

	IPlayerAnimState* m_pPlayerAnimState;

	QAngle m_angEyeAngles;
	CInterpolatedVar< QAngle > m_iv_angEyeAngles;

	int m_iShotsFired;

	CHandle< C_NEORagdoll > m_hRagdoll;

	int m_iNMFlash;

	//uint8_t NEOPlayerPad04[ 0x8 ];

	int m_iVision;

	//uint8_t NEOPlayerPad05[ 0x8 ];

	int m_iLives;
	int m_iStar;

	//uint8_t NEOPlayerPad06[ 0x4 ];

	float m_fTurnSpeed;

	//uint8_t NEOPlayerPad07[ 0x4 ];

	CHandle< C_NeoMobileArmorBase > m_MobileArmor;

	//uint8_t NEOPlayerPad08[ 0x4 ];

	bool m_bIsOnDeathScreen;

	float m_fLastDeathTime;
	float m_fCurrentTime;

	//uint8_t NEOPlayerPad09[ 0x4 ];

private:
	C_NEOPlayer( const C_NEOPlayer& );
};

inline C_NEOPlayer *ToNEOPlayer( CBaseEntity *pEntity )
{
	if ( !pEntity || !pEntity->IsPlayer() )
		return NULL;

#ifdef _DEBUG
	return dynamic_cast< C_NEOPlayer* >( pEntity );
#else
	return static_cast< C_NEOPlayer* >( pEntity );
#endif
}			  

#endif // C_NEO_PLAYER_H