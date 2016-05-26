#ifndef WEAPON_NEOBASE_H
#define WEAPON_NEOBASE_H
#ifdef _WIN32
#pragma once
#endif

#include "neo_playeranimstate.h"

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
	virtual ~CWeaponNEOBase();

	// C_BaseEntity implementations
	virtual void	Spawn();
	virtual void	Precache( void );
	virtual bool	ShouldPredict();

	// C_BaseAnimating implementations
	virtual int		InternalDrawModel( int flags );

	// C_BaseCombatWeapon implementations
	virtual bool	IsPredicted() const;

	virtual void			SetViewModel();

	virtual void	ItemPostFrame();

	virtual float			GetFireRate( void );

	virtual void			BaseCombatWeapon00();
	virtual void			BaseCombatWeapon01();

	virtual void	AddViewmodelBob( CBaseViewModel *viewmodel, Vector &origin, QAngle &angles );
	virtual	float	CalcViewmodelBob( void );

	// Now for WeaponNEOBase itself
	virtual const char*		GetWeaponTypeName();

	virtual	CNEOPlayer*		GetPlayerOwner() const;

	virtual bool			IsNotAGrenade();
	virtual bool			IsSemiAuto();
	virtual bool			IHaveNoFuckingClue00();

	virtual /*NEOWeaponID*/ int GetWeaponID( void ) const; //{ return WEAPON_NONE; }

	virtual bool			IHaveNoFuckingClue01();
	virtual bool			IHaveNoFuckingClue02();
	
	virtual bool			IsGhost(); // The robot fuck doll you know

	virtual int				GetWeaponType(); // If it's an assault rifle, etc.

	virtual bool			IsAFirearm(); // If it actually has bullets. btw I'm not sure if it's this one or the next nofuckingclue func
	virtual bool			IHaveNoFuckingClue03();

	virtual	bool			PlayEmptySound(); // *click*

	virtual bool			IHaveNoFuckingClue04();
	virtual bool			IHaveNoFuckingClue05();

	virtual void			SomethingWithTheViewModel(); // Yea that's a question. It does something with C_BasePlayer::m_bDrawViewmodel, gotta test some shit first

	virtual void*			GetSomethingFromFileWeaponInfo( void* a1 );	// I've got to reverse CNEOWeaponInfo to know what's in there

	virtual void			DoMuzzleFlash(); // This creates a Dlight then calls FX_MuzzleEffect
	
	virtual float			IHaveNoFuckingClue06();
	virtual float			IHaveNoFuckingClue07();
	
	virtual void			IHaveNoFuckingClue08();
	
	virtual float			GetRunningFov(); // When you're not aiming, I didn't want to think on a better name. Static reversal says it returns 75.f
	virtual float			GetAimingFov( int dunno ); // I think you can just push 0 since I believe they all just return the same variable. Static reversal says it returns 50.f
	
	virtual void			IHaveNoFuckingClue09( void* a1 ); // It has water trace shit, I'll figure this shit out later
	virtual void			IHaveNoFuckingClue10( void* a1, void* a2 ); // Has a call to WeaponSound(), I'll check this later aswell

private:
	int			m_iTeam2ViewModelIndex; 

	//char		WeaponNEOBasePad00[ 0x21 ];

	bool		bAimed;

	//char		WeaponNEOBasePad01[ 0x58 ];

	float		m_fAccuracy; 

	//char		WeaponNEOBasePad02[ 0x14 ];

private:
	CWeaponNEOBase( const CWeaponNEOBase & );
};


#endif // WEAPON_NEOBASE_H
