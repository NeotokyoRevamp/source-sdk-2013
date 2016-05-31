#ifndef WEAPON_NEOBASE_FIREARM_H
#define WEAPON_NEOBASE_FIREARM_H
#ifdef _WIN32
#pragma once
#endif

#include "weapon_neobase.h"

#ifdef CLIENT_DLL
#define CWeaponNeoBaseFirearm C_WeaponNeoBaseFirearm
#else
#endif

class CWeaponNeoBaseFirearm : public CWeaponNEOBase
{
public:	
	DECLARE_CLASS( CWeaponNeoBaseFirearm, CWeaponNEOBase);
	DECLARE_NETWORKCLASS(); 
	DECLARE_PREDICTABLE();
	
	CWeaponNeoBaseFirearm();

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
	virtual	bool PlayEmptySound();

	// MY OWN STUFF
	virtual void PrimaryAttackMobileArmor();
	virtual void EjectBrass();

private:
	int m_iFireMode; 

private:		
	CWeaponNeoBaseFirearm(const CWeaponNeoBaseFirearm& );
};


#endif // WEAPON_NEOBASE_FIREARM_H
