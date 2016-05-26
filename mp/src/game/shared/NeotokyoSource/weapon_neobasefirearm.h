#ifndef WEAPON_NEOBASE_FIREARM_H
#define WEAPON_NEOBASE_FIREARM_H
#ifdef _WIN32
#pragma once
#endif

#include "weapon_neobase.h"

#if defined( CLIENT_DLL )
	#define CWeaponNeoBaseGun C_WeaponNeoBaseFirearm
#else
#endif

class CWeaponNeoBaseGun : public CWeaponNEOBase
{
public:
	
	DECLARE_CLASS( CWeaponNeoBaseGun, CWeaponNEOBase );
	DECLARE_NETWORKCLASS(); 
	DECLARE_PREDICTABLE();
	
	CWeaponNeoBaseGun();
	virtual ~CWeaponNeoBaseGun();

	// C_BaseCombatWeapon implementations	
	virtual bool Deploy();
	virtual bool ReloadOrSwitchWeapons();
	virtual void ItemPostFrame();
	virtual void WeaponIdle();	
	virtual void FinishReload();
	virtual bool Reload();

	virtual void PrimaryAttack();
	virtual void SecondaryAttack();

	virtual void Spawn();

	// C_WeaponNEOBase implementations		 
	virtual bool IsNotAGrenade();

	virtual	bool PlayEmptySound();

	virtual bool IHaveNoFuckingClue04();

	virtual void* GetSomethingFromFileWeaponInfo( void* a1 );

	// MY OWN STUFF
	virtual void PrimaryAttackMobileArmor();
	virtual void EjectBrass();

public:
	//uint8_t WeaponNeoBaseFirearmPad00[ 0x14 ]; 
	int m_iFireMode; 
	//uint8_t WeaponNeoBaseFirearmPad01[ 0x8 ];

private:		
	CWeaponNeoBaseGun( const CWeaponNeoBaseGun & );	   
};


#endif // WEAPON_NEOBASE_FIREARM_H
