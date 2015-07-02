//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "weapon_hl2mpbase.h"

#ifndef BASEHLCOMBATWEAPON_H
#define BASEHLCOMBATWEAPON_H
#ifdef _WIN32
#pragma once
#endif

#if defined( CLIENT_DLL )
	#define CHL2MPMachineGun C_HL2MPMachineGun
#endif

#define FM_AUTO 0
#define FM_BURST 1
#define FM_SEMI 2

//=========================================================
// Machine gun base class
//=========================================================
class CHL2MPMachineGun : public CWeaponHL2MPBase
{
public:
	DECLARE_CLASS( CHL2MPMachineGun, CWeaponHL2MPBase );
	DECLARE_DATADESC();

	CHL2MPMachineGun();
	
	DECLARE_NETWORKCLASS(); 
	DECLARE_PREDICTABLE();

	void	PrimaryAttack(void);
	void	SecondaryAttack(void);
	
	// Default calls through to m_hOwner, but plasma weapons can override and shoot projectiles here.
	virtual void	ItemPostFrame( void );
	virtual void	FireBullets( const FireBulletsInfo_t &info );
	virtual bool	Deploy( void );
	virtual bool	Holster(CBaseCombatWeapon *pSwitchingTo);
	virtual void	Drop(const Vector &vecVelocity);
	virtual bool	Reload(void);

	virtual const Vector &GetBulletSpread( void );

	int				WeaponSoundRealtime( WeaponSound_t shoot_type );

	// utility function
	static void DoMachineGunKick( CBasePlayer *pPlayer, float dampEasy, float maxVerticleKickAngle, float fireDurationTime, float slideLimitTime );

private:
	
	CHL2MPMachineGun( const CHL2MPMachineGun & );

	void ToggleScope(bool state);

protected:

	int	m_nShotsFired;	// Number of consecutive shots fired

	int  m_iFireMode;	// Switches fire mode
	bool m_bScope;		// Toggles scope effect on secondary fire

	float	m_flNextSoundTime;	// real-time clock of when to make next sound
};

#endif // BASEHLCOMBATWEAPON_H
