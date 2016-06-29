#ifndef WEAPON_NEOBASE_H
#define WEAPON_NEOBASE_H
#ifdef _WIN32
#pragma once
#endif

#include "basecombatweapon_shared.h"
#include "neo_playeranimstate.h"
#include "neo_weapon_parse.h"

#if defined( CLIENT_DLL )
	#define CWeaponNEOBase C_WeaponNEOBase
#endif

extern const char* WeaponIDToAlias( int id );

class CNEOPlayer;					 

class CWeaponNEOBase : public CBaseCombatWeapon
{
public:
	DECLARE_CLASS( CWeaponNEOBase, CBaseCombatWeapon );
	DECLARE_NETWORKCLASS(); 
	DECLARE_PREDICTABLE();

	CWeaponNEOBase();

	// C_BaseEntity implementations
	virtual void			Spawn();
	virtual void			Precache();
	virtual bool			ShouldPredict();

	// C_BaseAnimating implementations
	virtual int				InternalDrawModel( int flags );

	// C_BaseCombatWeapon implementations
	virtual bool			IsPredicted() const { return true; }

	virtual void			SetViewModel();

	virtual void			ItemPostFrame();

	virtual float			GetFireRate() { return 1.f; }

	virtual void			OverrideViewmodelBob( CBaseViewModel *viewmodel, Vector &origin, QAngle &angles );
	virtual void			AddViewmodelBob( CBaseViewModel *viewmodel, Vector &origin, QAngle &angles );
	virtual	float			CalcViewmodelBob();

	// Now for WeaponNEOBase itself
	virtual const char*		GetWeaponTypeName() { return "primary"; }

	virtual NEOWeaponID		GetWeaponID() const { return WEAPON_NONE; }
		
	virtual bool			IsGhost() { return false; } // The robot blowdoll you know

	virtual int				GetWeaponType() { return 0x68; } // If it's an assault rifle, etc. We'll get an enum for this later
	
	virtual	bool			PlayEmptySound(); // *click*

	virtual void			DoNEOMuzzleFlash(); // This creates a Dlight then calls FX_MuzzleEffect

	virtual const char*		GetTeam2ViewModelName();
	
	virtual float			GetRunningFov() { return 75.f; }
	virtual float			GetAimingFov() { return 50.f; }

public:
	const CNEOWeaponInfo& GetNEOWpnData() const;

public: // Let's just keep these public for a while
	int			m_iTeam2ViewModelIndex; 

	float		m_fTPMuzzleFlashScale;

	bool		m_bUnknown2;
	float		m_fUnknown;
	int			m_iUnknown;
	float		m_fUnknown2;

	bool		m_bDrawCrosshair;
	bool		bAimed;

	int			m_iAimType;
	int			m_iScopeStyle;

	float		m_fVMFov;
	float		m_fVMAimFov;
	float		m_fAimFov;

	float		m_fVMOffsetUp;
	float		m_fVMOffsetForward;
	float		m_fVMOffsetRight;

	float		m_fVMAimOffsetUp;
	float		m_fVMAimOffsetForward;
	float		m_fVMAimOffsetRight;

	float		m_fVMAngleYaw;
	float		m_fVMAnglePitch;
	float		m_fVMAngleRoll;

	float		m_fVMAimAngleYaw;
	float		m_fVMAimAnglePitch;
	float		m_fVMAimAngleRoll;

	bool		m_bUnknown;
	
	float		m_fAccuracy; 

	int			m_iUnknown2;

private:
	CWeaponNEOBase( const CWeaponNEOBase & );
};


#endif // WEAPON_NEOBASE_H
