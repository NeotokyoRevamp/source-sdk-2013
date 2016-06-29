#ifndef C_NEO_PLAYER_H
#define C_NEO_PLAYER_H
#ifdef _WIN32
#pragma once
#endif

#include "neo_playeranimstate.h"
#include "c_baseplayer.h"
#include "weapon_neobase.h"
#include "iviewrender_beams.h"

class C_NEOPlayer : public C_BasePlayer, public INEOPlayerAnimStateHelpers
{
public:
	DECLARE_CLASS( C_NEOPlayer, C_BasePlayer );
	DECLARE_CLIENTCLASS();
	DECLARE_PREDICTABLE();
	DECLARE_INTERPOLATION();

	C_NEOPlayer();
	~C_NEOPlayer();

public:
	// C_BaseEnity implementantions
	virtual void			AddEntity();

	virtual Vector			EyePosition();	

	virtual void			OnDataChanged( DataUpdateType_t updateType );
	virtual void			PostDataUpdate( DataUpdateType_t updateType );

	virtual const QAngle&	GetRenderAngles();
	virtual int				DrawModel( int flags );
	virtual ShadowType_t	ShadowCastType();

	// C_BaseAnimating implementations
	virtual C_BaseAnimating* BecomeRagdollOnClient( bool bCopyEntity = true );

	virtual void			UpdateClientSideAnimation();

	virtual void			DoMuzzleFlash();
	virtual void			ProcessMuzzleFlashEvent();

	// C_BasePlayer implementations
	virtual void			CalcView( Vector &eyeOrigin, QAngle &eyeAngles, float &zNear, float &zFar, float &fov );
	virtual void			CalcPlayerView( Vector& eyeOrigin, QAngle& eyeAngles, float& fov );

	virtual bool			CreateMove( float flInputSampleTime, CUserCmd *pCmd );

	virtual IRagdoll*		GetRepresentativeRagdoll() const;

	virtual void			PreThink();

	virtual float			GetFOV();
	virtual float			GetMinFOV() const;		
	
	// INEOPlayerAnimStateHelpers implementations
	virtual CWeaponNEOBase* NEOAnim_GetActiveWeapon();
	virtual bool NEOAnim_CanMove();

	// My own stuff
	virtual void			NEO_MuzzleFlash();
	virtual void			GetEyeVectors( Vector *forward, Vector *right, Vector *up );

public:
	static C_NEOPlayer* GetLocalNEOPlayer();

	bool IsLocalNEOPlayer( void ) const;
	bool CanMove();
	bool CanSpeedBoost();

	CWeaponNEOBase* GetActiveNEOWeapon() const;

	inline int GetThermoptic() { return m_iThermoptic; }
	inline int GetVision() { return m_iVision; }
	inline int GetSprint() { return m_iSprint; }

	void UpdateSomething2();
	void UpdateThermoptic();
	void UpdateGeiger();
	void UpdateVision();
	void UpdateInCross();
	void UpdateSomething( float a1 );

public: // Eh let's make it all public for now
	CNetworkVar( bool, m_bIsVIP );

	float m_flUnknown;

	int m_iUnknown;
	float m_flUnknown3;

	CNetworkVar( int, m_iLean );

	CNetworkVar( int, m_iSprint );
	CNetworkVar( float, m_fSprintNRG );

	CNetworkVar( int, m_iThermoptic );
	int m_iOldThermoptic;
	CNetworkVar( float, m_fThermopticNRG );

	CNetworkVar( int, m_iClassType );
	int m_iOldClassType;

	CNetworkVar( int, m_iRank );
	int m_iOldRank;

	int m_iInCrossIndex;
	bool m_bAnyoneInCross;

	IPlayerAnimState* m_PlayerAnimState;

	QAngle m_angEyeAngles;
	CInterpolatedVar< QAngle > m_iv_angEyeAngles;

	CNetworkVar( int, m_iThrowGrenadeCounter );

	CNetworkVar( int, m_iShotsFired );

	EHANDLE m_hRagdoll;

	CNetworkVar( int, m_iNMFlash );
	int m_iOldNMFlash;

	float m_flUnknown2;

	CNetworkVar( int, m_iVision);

	int m_iOldVision;

	CNetworkVar( int, m_iReinforceTimer );

	CNetworkVar( int, m_iLives );
	CNetworkVar( int, m_iStar );

	bool m_bUnknown;

	CNetworkVar( float, m_fTurnSpeed );

	float m_flUnknown4;

	EHANDLE m_MobileArmor;

	Beam_t* m_pBeam;

	bool m_bIsOnDeathScreen;

	float m_fRagdollCreationTime;
	float m_fLastThinkTime;

	bool m_bUnknown2;

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