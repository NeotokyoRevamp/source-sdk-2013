#ifndef WEAPON_NEOBASE_H
#define WEAPON_NEOBASE_H
#ifdef _WIN32
#pragma once
#endif

#include "basecombatweapon_shared.h"

#if defined( CLIENT_DLL )
	#define CWeaponNEOBase C_WeaponNEOBase
#endif

extern int  ClassnameToWeaponID( const char *classname );
extern int	AliasToWeaponID( const char *alias );
extern const char *WeaponIDToAlias( int id );
extern const char *GetTranslatedWeaponAlias( const char *alias);
extern bool	IsPrimaryWeapon( int id );
extern bool IsSecondaryWeapon( int id );

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
	virtual void			Precache( void );
	virtual bool			ShouldPredict();

	// C_BaseAnimating implementations
	virtual int				InternalDrawModel( int flags );

	// C_BaseCombatWeapon implementations
	virtual bool			IsPredicted() const;

	virtual void			SetViewModel();

	virtual void			ItemPostFrame();

	virtual float			GetFireRate( void );

	virtual void			AddViewmodelBob( CBaseViewModel *viewmodel, Vector &origin, QAngle &angles );
	virtual	float			CalcViewmodelBob( void );

	// Now for WeaponNEOBase itself
	virtual const char*		GetWeaponTypeName();

	virtual	CNEOPlayer*		GetPlayerOwner() const;


	virtual /*NEOWeaponID*/ int GetWeaponID( void ) const; //{ return WEAPON_NONE; }

	
	virtual bool			IsGhost(); // The robot blowdoll you know

	virtual int				GetWeaponType(); // If it's an assault rifle, etc.
	
	virtual	bool			PlayEmptySound(); // *click*

	virtual void			DoMuzzleFlash(); // This creates a Dlight then calls FX_MuzzleEffect
	
	virtual float			GetRunningFov(); // When you're not aiming, I didn't want to think on a better name. Static reversal says it returns 75.f
	virtual float			GetAimingFov( int dunno ); // I think you can just push 0 since I believe they all just return the same variable. Static reversal says it returns 50.f

public:
	inline float			GetFOV() { return m_fFOV; }
private:
	int			m_iTeam2ViewModelIndex; 

	bool		bAimed;

	float		m_fFOV;

	float		m_fAccuracy; 

private:
	CWeaponNEOBase( const CWeaponNEOBase & );
};


#endif // WEAPON_NEOBASE_H
