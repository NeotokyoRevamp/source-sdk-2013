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

	void	PrimaryAttack( void );
	void	FinishBurst(void);
	
	// Default calls through to m_hOwner, but plasma weapons can override and shoot projectiles here.
	virtual void	ItemPostFrame( void );
	virtual void	FireBullets( const FireBulletsInfo_t &info );
	virtual bool	Deploy( void );
	virtual void	Drop(const Vector &vecVelocity);
	virtual bool	Reload(void);

	virtual const Vector &GetBulletSpread( void );

	int				WeaponSoundRealtime( WeaponSound_t shoot_type );

	// utility function
	static void DoMachineGunKick( CBasePlayer *pPlayer, float dampEasy, float maxVerticleKickAngle, float fireDurationTime, float slideLimitTime );

private:
	
	CHL2MPMachineGun( const CHL2MPMachineGun & );

protected:

	int	m_nShotsFired;	// Number of consecutive shots fired

	bool m_bBurst;		// Toggle burst fire
	bool m_bFinishBurst; // If true fires all remaining bullets

	int m_nBurstMaxBullets;	// Defines how many bullets to shoot each burst
	int m_nBurstBullets;	// Keeps track of how many bullets are remaining for this burst

	float	m_flNextSoundTime;	// real-time clock of when to make next sound
};

#endif // BASEHLCOMBATWEAPON_H
