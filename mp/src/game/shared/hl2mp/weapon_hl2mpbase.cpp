//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#include "cbase.h"
#include "in_buttons.h"
#include "dt_recv.h"
#include "takedamageinfo.h"
#include "ammodef.h"
#include "hl2mp_gamerules.h"


#ifdef CLIENT_DLL
extern IVModelInfoClient* modelinfo;
#else
extern IVModelInfo* modelinfo;
#endif

#if defined( CLIENT_DLL )
	#include "prediction.h"
	#include "vgui/ISurface.h"
	#include "vgui_controls/Controls.h"
	#include "c_hl2mp_player.h"
	#include "hud_crosshair.h"

#else

	#include "hl2mp_player.h"
	#include "vphysics/constraints.h"

#endif

#include "weapon_hl2mpbase.h"

// ----------------------------------------------------------------------------- //
// Global functions.
// ----------------------------------------------------------------------------- //

bool IsAmmoType( int iAmmoType, const char *pAmmoName )
{
	return GetAmmoDef()->Index( pAmmoName ) == iAmmoType;
}

static const char * s_WeaponAliasInfo[] = 
{
	"none",	//	WEAPON_NONE = 0,

	//Melee
	"shotgun",	//WEAPON_AMERKNIFE,
	
	NULL,		// end of list marker
};


// ----------------------------------------------------------------------------- //
// CWeaponHL2MPBase tables.
// ----------------------------------------------------------------------------- //

IMPLEMENT_NETWORKCLASS_ALIASED(WeaponHL2MPBase, DT_WeaponHL2MPBase)

BEGIN_NETWORK_TABLE(CWeaponHL2MPBase, DT_WeaponHL2MPBase)


#ifdef CLIENT_DLL
//	RecvPropInt( RECVINFO( m_bReflectViewModelAnimations ) ),
RecvPropInt( RECVINFO( m_bIsIronsighted )),//, 0, RecvProxy_ToggleSights ), //note: RecvPropBool is actually RecvPropInt (see its implementation), but we need a proxy
RecvPropFloat(RECVINFO(m_flIronsightedTime)),
#else
// world weapon models have no aminations
//	SendPropExclude( "DT_AnimTimeMustBeFirst", "m_flAnimTime" ),
//	SendPropExclude( "DT_BaseAnimating", "m_nSequence" ),
//	SendPropExclude( "DT_LocalActiveWeaponData", "m_flTimeWeaponIdle" ),
//	SendPropInt( SENDINFO( m_bReflectViewModelAnimations ), 1, SPROP_UNSIGNED ),
SendPropBool(SENDINFO(m_bIsIronsighted)),
SendPropFloat(SENDINFO(m_flIronsightedTime)),
#endif

END_NETWORK_TABLE()

#if !defined( CLIENT_DLL )

#include "globalstate.h"

//---------------------------------------------------------
// Save/Restore
//---------------------------------------------------------
BEGIN_DATADESC(CWeaponHL2MPBase)

DEFINE_FIELD(m_bLowered, FIELD_BOOLEAN),
DEFINE_FIELD(m_flRaiseTime, FIELD_TIME),
DEFINE_FIELD(m_nShotsFired, FIELD_INTEGER),
DEFINE_FIELD(m_iFireMode, FIELD_INTEGER),
DEFINE_FIELD(m_flNextSoundTime, FIELD_TIME),

END_DATADESC()

#endif

BEGIN_PREDICTION_DATA(CWeaponHL2MPBase)
//DEFINE_PRED_FIELD(m_bIsIronsighted, FIELD_BOOLEAN, FTYPEDESC_INSENDTABLE),
//DEFINE_PRED_FIELD(m_flIronsightedTime, FIELD_FLOAT, FTYPEDESC_INSENDTABLE),
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS( weapon_hl2mp_base, CWeaponHL2MPBase );

// ----------------------------------------------------------------------------- //
// CWeaponHL2MPBase implementation. 
// ----------------------------------------------------------------------------- //
CWeaponHL2MPBase::CWeaponHL2MPBase()
{
	SetPredictionEligible( true );
	AddSolidFlags( FSOLID_TRIGGER ); // Nothing collides with these but it gets touches.

	m_flNextResetCheckTime = 0.0f;

	m_bIsIronsighted = false;
	m_flIronsightedTime = 0.0f;
}

//-----------------------------------------------------------------------------
// Purpose: Drops the weapon into a lowered pose
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool CWeaponHL2MPBase::Lower(void)
{
	// Unzoom when starting to sprint
	if (m_bIsIronsighted)
		DisableIronsights();

	//Don't bother if we don't have the animation
	if (SelectWeightedSequence(ACT_VM_IDLE_LOWERED) == ACTIVITY_NOT_AVAILABLE)
		return false;

	m_bLowered = true;
	return true;
}

//-----------------------------------------------------------------------------
// Purpose: Brings the weapon up to the ready position
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool CWeaponHL2MPBase::Ready(void)
{
	//Don't bother if we don't have the animation
	if (SelectWeightedSequence(ACT_VM_LOWERED_TO_IDLE) == ACTIVITY_NOT_AVAILABLE)
		return false;

	m_bLowered = false;
	m_flRaiseTime = gpGlobals->curtime + 0.5f;
	return true;
}

//-----------------------------------------------------------------------------
// Purpose: 
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool CWeaponHL2MPBase::Deploy(void)
{
	// Reset our shots fired
	m_nShotsFired = 0;

	// If we should be lowered, deploy in the lowered position
	// We have to ask the player if the last time it checked, the weapon was lowered
	if (GetOwner() && GetOwner()->IsPlayer())
	{
		CHL2MP_Player *pPlayer = assert_cast<CHL2MP_Player*>(GetOwner());
		if (pPlayer->IsWeaponLowered())
		{
			if (SelectWeightedSequence(ACT_VM_IDLE_LOWERED) != ACTIVITY_NOT_AVAILABLE)
			{
				if (DefaultDeploy((char*)GetViewModel(), (char*)GetWorldModel(), ACT_VM_IDLE_LOWERED, (char*)GetAnimPrefix()))
				{
					m_bLowered = true;

					// Stomp the next attack time to fix the fact that the lower idles are long
					pPlayer->SetNextAttack(gpGlobals->curtime + 1.0);
					m_flNextPrimaryAttack = gpGlobals->curtime + 1.0;
					m_flNextSecondaryAttack = gpGlobals->curtime + 1.0;
					return true;
				}
			}
		}
	}

	m_bLowered = false;
	return BaseClass::Deploy();
}

//-----------------------------------------------------------------------------
// Purpose: 
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool CWeaponHL2MPBase::WeaponShouldBeLowered(void)
{
	// Can't be in the middle of another animation
	if (GetIdealActivity() != ACT_VM_IDLE_LOWERED && GetIdealActivity() != ACT_VM_IDLE &&
		GetIdealActivity() != ACT_VM_IDLE_TO_LOWERED && GetIdealActivity() != ACT_VM_LOWERED_TO_IDLE)
		return false;

	if (m_bLowered)
		return true;

#if !defined( CLIENT_DLL )

	if (GlobalEntity_GetState("friendly_encounter") == GLOBAL_ON)
		return true;

#endif

	return false;
}

//-----------------------------------------------------------------------------
// Purpose: Allows the weapon to choose proper weapon idle animation
//-----------------------------------------------------------------------------
void CWeaponHL2MPBase::WeaponIdle(void)
{
	//See if we should idle high or low
	if (WeaponShouldBeLowered())
	{
		// Move to lowered position if we're not there yet
		if (GetActivity() != ACT_VM_IDLE_LOWERED && GetActivity() != ACT_VM_IDLE_TO_LOWERED
			&& GetActivity() != ACT_TRANSITION)
		{
			SendWeaponAnim(ACT_VM_IDLE_LOWERED);
		}
		else if (HasWeaponIdleTimeElapsed())
		{
			// Keep idling low
			SendWeaponAnim(ACT_VM_IDLE_LOWERED);
		}
	}
	else
	{
		// See if we need to raise immediately
		if (m_flRaiseTime < gpGlobals->curtime && GetActivity() == ACT_VM_IDLE_LOWERED)
		{
			SendWeaponAnim(ACT_VM_IDLE);
		}
		else if (HasWeaponIdleTimeElapsed())
		{
			SendWeaponAnim(ACT_VM_IDLE);
		}
	}
}

//====================================================================================
// Lower weapon on sprint
//====================================================================================
void CWeaponHL2MPBase::ItemPostFrame(void)
{
	CHL2MP_Player *pOwner = GetHL2MPPlayerOwner();
	if (!pOwner)
		return;

	if (!m_bLowered && pOwner->IsSprinting())
	{
		Lower();
	}
	else if (m_bLowered && !pOwner->IsSprinting())
	{
		Ready();

		// Reset next attack time
		m_flNextPrimaryAttack = MAX(m_flNextPrimaryAttack, gpGlobals->curtime);
		m_flNextSecondaryAttack = MAX(m_flNextSecondaryAttack, gpGlobals->curtime);
	}

	// Don't allow firing etc. while lowered.
	if (m_bLowered || GetActivity() == ACT_VM_IDLE_LOWERED)
	{
		// Allow reload to finish
		if (UsesClipsForAmmo1())
		{
			CheckReload();
		}

		WeaponIdle();

		return;
	}

	// Holding down +attack with burst fire
	if (m_iFireMode == FM_BURST && (m_nShotsFired >= GetMaxBurst()))
	{
		// Reset shots fored and continue normally with regular rate of fire
		m_nShotsFired = 0;
	}
	// Weapon is semi auto and we're still holding +attack after firing
	else if (m_iFireMode == FM_SEMI && (pOwner->m_nButtons & IN_ATTACK) && m_nShotsFired >= 1)
	{
		WeaponIdle();

		return;
	}
	// -attack
	else if ((pOwner->m_nButtons & IN_ATTACK) == false)
	{
		if (m_iFireMode == FM_BURST && m_nShotsFired > 0) // Slightly bigger delay to avoid higher DPS with tapping
			m_flNextPrimaryAttack = gpGlobals->curtime + 0.15;

		// Reset shots fired
		m_nShotsFired = 0;
	}

	BaseClass::ItemPostFrame();
}

//-----------------------------------------------------------------------------
// Purpose: Override for mag toss
//-----------------------------------------------------------------------------
void CWeaponHL2MPBase::FinishReload(void)
{
	CBaseCombatCharacter *pOwner = GetOwner();

	if (pOwner)
	{
		// If I use primary clips, reload primary
		if (UsesClipsForAmmo1())
		{
			int primary = MIN(GetMaxClip1(), pOwner->GetAmmoCount(m_iPrimaryAmmoType));
			m_iClip1 = primary;
			pOwner->RemoveAmmo(primary, m_iPrimaryAmmoType);
		}

		// If I use secondary clips, reload secondary
		if (UsesClipsForAmmo2())
		{
			int secondary = MIN(GetMaxClip2(), pOwner->GetAmmoCount(m_iSecondaryAmmoType));
			m_iClip2 = secondary;
			pOwner->RemoveAmmo(secondary, m_iSecondaryAmmoType);
		}

		if (m_bReloadsSingly)
		{
			m_bInReload = false;
		}
	}
}

#if defined( CLIENT_DLL )

#define	HL2_BOB_CYCLE_MIN	1.0f
#define	HL2_BOB_CYCLE_MAX	0.45f
#define	HL2_BOB			0.002f
#define	HL2_BOB_UP		0.5f

extern float	g_lateralBob;
extern float	g_verticalBob;

static ConVar	cl_bobcycle("cl_bobcycle", "0.8");
static ConVar	cl_bob("cl_bob", "0.002");
static ConVar	cl_bobup("cl_bobup", "0.5");

// Register these cvars if needed for easy tweaking
static ConVar	v_iyaw_cycle("v_iyaw_cycle", "2", FCVAR_REPLICATED | FCVAR_CHEAT);
static ConVar	v_iroll_cycle("v_iroll_cycle", "0.5", FCVAR_REPLICATED | FCVAR_CHEAT);
static ConVar	v_ipitch_cycle("v_ipitch_cycle", "1", FCVAR_REPLICATED | FCVAR_CHEAT);
static ConVar	v_iyaw_level("v_iyaw_level", "0.3", FCVAR_REPLICATED | FCVAR_CHEAT);
static ConVar	v_iroll_level("v_iroll_level", "0.1", FCVAR_REPLICATED | FCVAR_CHEAT);
static ConVar	v_ipitch_level("v_ipitch_level", "0.3", FCVAR_REPLICATED | FCVAR_CHEAT);

//-----------------------------------------------------------------------------
// Purpose: 
// Output : float
//-----------------------------------------------------------------------------
float CWeaponHL2MPBase::CalcViewmodelBob(void)
{
	static	float bobtime;
	static	float lastbobtime;
	float	cycle;

	CBasePlayer *player = ToBasePlayer(GetOwner());
	//Assert( player );

	//NOTENOTE: For now, let this cycle continue when in the air, because it snaps badly without it

	if ((!gpGlobals->frametime) || (player == NULL))
	{
		//NOTENOTE: We don't use this return value in our case (need to restructure the calculation function setup!)
		return 0.0f;// just use old value
	}

	//Find the speed of the player
	float speed = player->GetLocalVelocity().Length2D();

	//FIXME: This maximum speed value must come from the server.
	//		 MaxSpeed() is not sufficient for dealing with sprinting - jdw

	speed = clamp(speed, -320, 320);

	float bob_offset = RemapVal(speed, 0, 320, 0.0f, 1.0f);

	bobtime += (gpGlobals->curtime - lastbobtime) * bob_offset;
	lastbobtime = gpGlobals->curtime;

	//Calculate the vertical bob
	cycle = bobtime - (int)(bobtime / HL2_BOB_CYCLE_MAX)*HL2_BOB_CYCLE_MAX;
	cycle /= HL2_BOB_CYCLE_MAX;

	if (cycle < HL2_BOB_UP)
	{
		cycle = M_PI * cycle / HL2_BOB_UP;
	}
	else
	{
		cycle = M_PI + M_PI*(cycle - HL2_BOB_UP) / (1.0 - HL2_BOB_UP);
	}

	g_verticalBob = speed*0.005f;
	g_verticalBob = g_verticalBob*0.3 + g_verticalBob*0.7*sin(cycle);

	g_verticalBob = clamp(g_verticalBob, -7.0f, 4.0f);

	//Calculate the lateral bob
	cycle = bobtime - (int)(bobtime / HL2_BOB_CYCLE_MAX * 2)*HL2_BOB_CYCLE_MAX * 2;
	cycle /= HL2_BOB_CYCLE_MAX * 2;

	if (cycle < HL2_BOB_UP)
	{
		cycle = M_PI * cycle / HL2_BOB_UP;
	}
	else
	{
		cycle = M_PI + M_PI*(cycle - HL2_BOB_UP) / (1.0 - HL2_BOB_UP);
	}

	g_lateralBob = speed*0.005f;
	g_lateralBob = g_lateralBob*0.3 + g_lateralBob*0.7*sin(cycle);
	g_lateralBob = clamp(g_lateralBob, -7.0f, 4.0f);

	//NOTENOTE: We don't use this return value in our case (need to restructure the calculation function setup!)
	return 0.0f;
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : &origin - 
//			&angles - 
//			viewmodelindex - 
//-----------------------------------------------------------------------------
void CWeaponHL2MPBase::AddViewmodelBob(CBaseViewModel *viewmodel, Vector &origin, QAngle &angles)
{
	Vector	forward, right;
	AngleVectors(angles, &forward, &right, NULL);

	CalcViewmodelBob();

	// Apply bob, but scaled down to 40%
	VectorMA(origin, g_verticalBob * 0.1f, forward, origin);

	// Z bob a bit more
	origin[2] += g_verticalBob * 0.1f;

	// bob the angles
	angles[ROLL] += g_verticalBob * 0.5f;
	angles[PITCH] -= g_verticalBob * 0.4f;

	angles[YAW] -= g_lateralBob  * 0.3f;

	VectorMA(origin, g_lateralBob * 0.8f, right, origin);
}

//-----------------------------------------------------------------------------
float CWeaponHL2MPBase::GetSpreadBias(WeaponProficiency_t proficiency)
{
	return BaseClass::GetSpreadBias(proficiency);
}
//-----------------------------------------------------------------------------

const WeaponProficiencyInfo_t *CWeaponHL2MPBase::GetProficiencyValues()
{
	return NULL;
}

#else

// Server stubs
float CWeaponHL2MPBase::CalcViewmodelBob(void)
{
	return 0.0f;
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : &origin - 
//			&angles - 
//			viewmodelindex - 
//-----------------------------------------------------------------------------
void CWeaponHL2MPBase::AddViewmodelBob(CBaseViewModel *viewmodel, Vector &origin, QAngle &angles)
{
}

//-----------------------------------------------------------------------------
float CWeaponHL2MPBase::GetSpreadBias(WeaponProficiency_t proficiency)
{
	const WeaponProficiencyInfo_t *pProficiencyValues = GetProficiencyValues();
	return (pProficiencyValues)[proficiency].bias;
}

//-----------------------------------------------------------------------------
const WeaponProficiencyInfo_t *CWeaponHL2MPBase::GetProficiencyValues()
{
	return GetDefaultProficiencyValues();
}

//-----------------------------------------------------------------------------
const WeaponProficiencyInfo_t *CWeaponHL2MPBase::GetDefaultProficiencyValues()
{
	// Weapon proficiency table. Keep this in sync with WeaponProficiency_t enum in the header!!
	static WeaponProficiencyInfo_t g_BaseWeaponProficiencyTable[] =
	{
		{ 2.50, 1.0 },
		{ 2.00, 1.0 },
		{ 1.50, 1.0 },
		{ 1.25, 1.0 },
		{ 1.00, 1.0 },
	};

	COMPILE_TIME_ASSERT(ARRAYSIZE(g_BaseWeaponProficiencyTable) == WEAPON_PROFICIENCY_PERFECT + 1);

	return g_BaseWeaponProficiencyTable;
}

#endif

Vector CWeaponHL2MPBase::GetBulletSpread(WeaponProficiency_t proficiency)
{
	return GetBulletSpread();
}

const Vector &CWeaponHL2MPBase::GetBulletSpread(void)
{
	return GetHL2MPWpnData().m_vecSpread;
}

float CWeaponHL2MPBase::GetFireRate(void)
{
	return GetHL2MPWpnData().m_flCycleTime;
}

float CWeaponHL2MPBase::GetRecoilPitch(void)
{
	return GetHL2MPWpnData().m_flRecoilPitch;
}

float CWeaponHL2MPBase::GetRecoilYaw(void)
{
	return GetHL2MPWpnData().m_flRecoilYaw;
}

bool CWeaponHL2MPBase::IsPredicted() const
{ 
	return true;
}

void CWeaponHL2MPBase::WeaponSound( WeaponSound_t sound_type, float soundtime /* = 0.0f */ )
{
#ifdef CLIENT_DLL

		// If we have some sounds from the weapon classname.txt file, play a random one of them
		const char *shootsound = GetWpnData().aShootSounds[ sound_type ]; 
		if ( !shootsound || !shootsound[0] )
			return;

		CBroadcastRecipientFilter filter; // this is client side only
		if ( !te->CanPredict() )
			return;
				
		CBaseEntity::EmitSound( filter, GetPlayerOwner()->entindex(), shootsound, &GetPlayerOwner()->GetAbsOrigin() ); 
#else
		BaseClass::WeaponSound( sound_type, soundtime );
#endif
}


CBasePlayer* CWeaponHL2MPBase::GetPlayerOwner() const
{
	return dynamic_cast< CBasePlayer* >( GetOwner() );
}

CHL2MP_Player* CWeaponHL2MPBase::GetHL2MPPlayerOwner() const
{
	return dynamic_cast< CHL2MP_Player* >( GetOwner() );
}

#ifdef CLIENT_DLL
	
void CWeaponHL2MPBase::OnDataChanged( DataUpdateType_t type )
{
	BaseClass::OnDataChanged( type );

	if ( GetPredictable() && !ShouldPredict() )
		ShutdownPredictable();
}


bool CWeaponHL2MPBase::ShouldPredict()
{
	if ( GetOwner() && GetOwner() == C_BasePlayer::GetLocalPlayer() )
		return true;

	return BaseClass::ShouldPredict();
}


#else
	
void CWeaponHL2MPBase::Spawn()
{
	BaseClass::Spawn();

	// Set this here to allow players to shoot dropped weapons
	SetCollisionGroup( COLLISION_GROUP_WEAPON );
}

void CWeaponHL2MPBase::Materialize( void )
{
	if ( IsEffectActive( EF_NODRAW ) )
	{
		// changing from invisible state to visible.
		EmitSound( "AlyxEmp.Charge" );
		
		RemoveEffects( EF_NODRAW );
		DoMuzzleFlash();
	}

	if ( HasSpawnFlags( SF_NORESPAWN ) == false )
	{
		VPhysicsInitNormal( SOLID_BBOX, GetSolidFlags() | FSOLID_TRIGGER, false );
		SetMoveType( MOVETYPE_VPHYSICS );
	}

	if ( HasSpawnFlags( SF_NORESPAWN ) == false )
	{
		if ( GetOriginalSpawnOrigin() == vec3_origin )
		{
			m_vOriginalSpawnOrigin = GetAbsOrigin();
			m_vOriginalSpawnAngles = GetAbsAngles();
		}
	}

	SetPickupTouch();

	SetThink (NULL);
}

int CWeaponHL2MPBase::ObjectCaps()
{
	return BaseClass::ObjectCaps() & ~FCAP_IMPULSE_USE;
}

#endif

void CWeaponHL2MPBase::FallInit( void )
{
#ifndef CLIENT_DLL
	SetModel( GetWorldModel() );
	VPhysicsDestroyObject();

	if ( HasSpawnFlags( SF_NORESPAWN ) == false )
	{
		SetMoveType( MOVETYPE_NONE );
		SetSolid( SOLID_BBOX );
		AddSolidFlags( FSOLID_TRIGGER );

		UTIL_DropToFloor( this, MASK_SOLID );
	}
	else
	{
		if ( !VPhysicsInitNormal( SOLID_BBOX, GetSolidFlags() | FSOLID_TRIGGER, false ) )
		{
			SetMoveType( MOVETYPE_NONE );
			SetSolid( SOLID_BBOX );
			AddSolidFlags( FSOLID_TRIGGER );
		}
		else
		{
	#if !defined( CLIENT_DLL )
			// Constrained start?
			if ( HasSpawnFlags( SF_WEAPON_START_CONSTRAINED ) )
			{
				//Constrain the weapon in place
				IPhysicsObject *pReferenceObject, *pAttachedObject;
				
				pReferenceObject = g_PhysWorldObject;
				pAttachedObject = VPhysicsGetObject();

				if ( pReferenceObject && pAttachedObject )
				{
					constraint_fixedparams_t fixed;
					fixed.Defaults();
					fixed.InitWithCurrentObjectState( pReferenceObject, pAttachedObject );
					
					fixed.constraint.forceLimit	= lbs2kg( 10000 );
					fixed.constraint.torqueLimit = lbs2kg( 10000 );

					IPhysicsConstraint *pConstraint = GetConstraint();

					pConstraint = physenv->CreateFixedConstraint( pReferenceObject, pAttachedObject, NULL, fixed );

					pConstraint->SetGameData( (void *) this );
				}
			}
	#endif //CLIENT_DLL
		}
	}

	SetPickupTouch();
	
	SetThink( &CBaseCombatWeapon::FallThink );

	SetNextThink( gpGlobals->curtime + 0.1f );

#endif
}

const CHL2MPSWeaponInfo &CWeaponHL2MPBase::GetHL2MPWpnData() const
{
	const FileWeaponInfo_t *pWeaponInfo = &GetWpnData();
	const CHL2MPSWeaponInfo *pHL2MPInfo;

	#ifdef _DEBUG
		pHL2MPInfo = dynamic_cast< const CHL2MPSWeaponInfo* >( pWeaponInfo );
		Assert( pHL2MPInfo );
	#else
		pHL2MPInfo = static_cast< const CHL2MPSWeaponInfo* >( pWeaponInfo );
	#endif
	
	return *pHL2MPInfo;
}
void CWeaponHL2MPBase::FireBullets( const FireBulletsInfo_t &info )
{
	FireBulletsInfo_t modinfo = info;

	modinfo.m_iPlayerDamage = GetHL2MPWpnData().m_iPlayerDamage;

	BaseClass::FireBullets( modinfo );
}

#if defined( CLIENT_DLL )

#include "c_te_effect_dispatch.h"

#define NUM_MUZZLE_FLASH_TYPES 4

bool CWeaponHL2MPBase::OnFireEvent( C_BaseViewModel *pViewModel, const Vector& origin, const QAngle& angles, int event, const char *options )
{
	return BaseClass::OnFireEvent( pViewModel, origin, angles, event, options );
}

void UTIL_ClipPunchAngleOffset( QAngle &in, const QAngle &punch, const QAngle &clip )
{
	QAngle	final = in + punch;

	//Clip each component
	for ( int i = 0; i < 3; i++ )
	{
		if ( final[i] > clip[i] )
		{
			final[i] = clip[i];
		}
		else if ( final[i] < -clip[i] )
		{
			final[i] = -clip[i];
		}

		//Return the result
		in[i] = final[i] - punch[i];
	}
}

#endif

bool CWeaponHL2MPBase::UsesIronsights(void) const
{
#ifdef CLIENT_DLL
	if (GetHL2MPPlayerOwner()->IronSightsEnabled()) // Using ironsights
		return true;
#endif
	return false;
}

Vector CWeaponHL2MPBase::GetViewModelPositionOffset(void) const
{
	Vector posOffset;

	float delta = (gpGlobals->curtime - m_flIronsightedTime) / 0.2f; //modify this value to adjust how fast the interpolation is
	if (delta >= 1.0f)
	{
		if (m_bIsIronsighted)
		{
			if (UsesIronsights()) // Using ironsights
				return GetHL2MPWpnData().m_vecAimPosOffset;
			else // Using classic zoom
				return GetHL2MPWpnData().m_vecZoomPosOffset;

		}
		return GetHL2MPWpnData().m_vecVMPosOffset;
	}
	
	delta = clamp(delta, 0.0f, 1.0f);

	if (m_bIsIronsighted)
		delta = 1.0f - delta;
	
	Vector start = UsesIronsights() ? GetHL2MPWpnData().m_vecAimPosOffset : GetHL2MPWpnData().m_vecZoomPosOffset;
	Vector direction = GetHL2MPWpnData().m_vecVMPosOffset - start;
	VectorMA(start, delta, direction, posOffset);
	return posOffset;
}

QAngle CWeaponHL2MPBase::GetViewModelAngleOffset(void) const
{
	QAngle angOffset;
	float delta = (gpGlobals->curtime - m_flIronsightedTime) / 0.2f; //modify this value to adjust how fast the interpolation is
	if (delta >= 1.0f)
	{
		if (m_bIsIronsighted)
		{
			if (UsesIronsights()) // Using ironsights
				return GetHL2MPWpnData().m_angAimAngOffset;
			else // Using classic zoom
				return GetHL2MPWpnData().m_angZoomAngOffset;
		}

		return GetHL2MPWpnData().m_angVMAngOffset;
	}

	delta = clamp(delta, 0.0f, 1.0f);

	if (m_bIsIronsighted)
		delta = 1.0f - delta;

	QAngle start = UsesIronsights() ? GetHL2MPWpnData().m_angAimAngOffset : GetHL2MPWpnData().m_angZoomAngOffset;
	QAngle direction = GetHL2MPWpnData().m_angVMAngOffset - start;
	VectorMA(start, delta, direction, angOffset);
	return angOffset;
}

float CWeaponHL2MPBase::GetViewModelFOV(void) const
{
	float delta = (gpGlobals->curtime - m_flIronsightedTime) / 0.2f; //modify this value to adjust how fast the interpolation is
	if (delta > 1.0f)
	{
		if (m_bIsIronsighted)
		{
			if (UsesIronsights()) // Using ironsights
				return GetHL2MPWpnData().m_flAimFov;
			else
				return GetHL2MPWpnData().m_flZoomFov;
		}
		
		return GetHL2MPWpnData().m_flVMFov;
	}

	delta = clamp(delta, 0.0f, 1.0f);

	if (m_bIsIronsighted)
		delta = 1.0f - delta;

	float start = UsesIronsights() ? GetHL2MPWpnData().m_flAimFov : GetHL2MPWpnData().m_flZoomFov;
	float direction = GetHL2MPWpnData().m_flVMFov - start;
	float fov = start + direction * delta;
	return fov;
}

float CWeaponHL2MPBase::GetIronSightFOV(void) const
{
	return GetHL2MPWpnData().m_flAimZoom;
}

bool CWeaponHL2MPBase::HasIronsights(void)
{
	if (GetHL2MPWpnData().m_iAimType > 0)
		return true;

	return false;
}

void CWeaponHL2MPBase::ToggleIronsights(void)
{
	if (m_bIsIronsighted)
		DisableIronsights();
	else
		EnableIronsights();
}

void CWeaponHL2MPBase::EnableIronsights(void)
{
#ifdef CLIENT_DLL
	if (!prediction->IsFirstTimePredicted())
		return;
#endif
	if (!HasIronsights() || m_bIsIronsighted)
		return;

	CBasePlayer *pOwner = ToBasePlayer(GetOwner());

	if (!pOwner)
		return;

	m_bIsIronsighted = true;

	if (pOwner->SetFOV(this, GetIronSightFOV(), 0.3f)) //modify the last value to adjust how fast the fov is applied
	{
		SetIronsightTime();
	}

	if (GetHL2MPWpnData().m_iAimType < 2)
		return; // Not using scope

	// TODO: View is blocked by model so hide the viewmodel

	SetWeaponVisible(false);

#ifndef CLIENT_DLL

	// Send a message to show the scope
	CSingleUserRecipientFilter filter(pOwner);
	UserMessageBegin(filter, "ShowScope");
	WRITE_BYTE(1); // Show scope
	MessageEnd();
#endif
}

void CWeaponHL2MPBase::DisableIronsights(void)
{
#ifdef CLIENT_DLL
	if (!prediction->IsFirstTimePredicted())
		return;
#endif
	if (!HasIronsights() || !m_bIsIronsighted)
		return;

	CBasePlayer *pOwner = ToBasePlayer(GetOwner());

	if (!pOwner)
		return;

	SetWeaponVisible(true); // FIXME: When scoped weapon is dropped next weapon stays invisible

	if (pOwner->SetFOV(this, 0, 0.3f)) //modify the last value to adjust how fast the fov is applied
	{
		m_bIsIronsighted = false;
		SetIronsightTime();
	}

#ifndef CLIENT_DLL
	// Send a message to show the scope
	CSingleUserRecipientFilter filter(pOwner);
	UserMessageBegin(filter, "ShowScope");
	WRITE_BYTE(0); // Hide scope
	MessageEnd();
#endif
}

void CWeaponHL2MPBase::SetIronsightTime(void)
{
	m_flIronsightedTime = gpGlobals->curtime;
}

void CWeaponHL2MPBase::PrimaryAttack(void)
{
	// Only the player fires this way so we can cast
	CBasePlayer *pPlayer = ToBasePlayer(GetOwner());
	if (!pPlayer)
		return;

	// Abort here to handle burst and auto fire modes
	if ((UsesClipsForAmmo1() && m_iClip1 == 0) || (!UsesClipsForAmmo1() && !pPlayer->GetAmmoCount(m_iPrimaryAmmoType)))
		return;

	if ((m_iFireMode == FM_BURST) && (m_nShotsFired > GetMaxBurst()))
		return;

	if ((m_iFireMode == FM_SEMI) && (m_nShotsFired >= 1))
		return;

	pPlayer->DoMuzzleFlash();

	// To make the firing framerate independent, we may have to fire more than one bullet here on low-framerate systems, 
	// especially if the weapon we're firing has a really fast rate of fire.
	int iBulletsToFire = 0;
	float fireRate = GetFireRate();

	while (m_flNextPrimaryAttack <= gpGlobals->curtime)
	{
		// MUST call sound before removing a round from the clip of a CHLMachineGun
		WeaponSound(SINGLE, m_flNextPrimaryAttack);
		m_flNextPrimaryAttack = m_flNextPrimaryAttack + fireRate;
		m_nShotsFired++;
		iBulletsToFire++;

		if (m_iFireMode == FM_SEMI)
			break; // Can't fire more than one bullet
	}

	// Make sure we don't fire more than the amount in the clip, if this weapon uses clips
	if (UsesClipsForAmmo1())
	{
		if (iBulletsToFire > m_iClip1)
			iBulletsToFire = m_iClip1;
		m_iClip1 -= iBulletsToFire;
	}

	CHL2MP_Player *pHL2MPPlayer = ToHL2MPPlayer(pPlayer);

	// Fire the bullets
	FireBulletsInfo_t info;
	info.m_iShots = iBulletsToFire;
	info.m_vecSrc = pHL2MPPlayer->Weapon_ShootPosition();
	info.m_vecDirShooting = pPlayer->GetAutoaimVector(AUTOAIM_5DEGREES);
	info.m_vecSpread = pHL2MPPlayer->GetAttackSpread(this);
	info.m_flDistance = MAX_TRACE_LENGTH;
	info.m_iAmmoType = m_iPrimaryAmmoType;
	info.m_iTracerFreq = 2;
	FireBullets(info);

	//Factor in the view kick
	AddViewKick();

	if (!m_iClip1 && pPlayer->GetAmmoCount(m_iPrimaryAmmoType) <= 0)
	{
		// HEV suit - indicate out of ammo condition
		pPlayer->SetSuitUpdate("!HEV_AMO0", FALSE, 0);
	}

	SendWeaponAnim(GetPrimaryAttackActivity());
	pPlayer->SetAnimation(PLAYER_ATTACK1);

#ifdef CLIENT_DLL
	if (prediction->IsFirstTimePredicted())
		pHL2MPPlayer->CreateRecoil(GetRecoilPitch(), GetRecoilYaw());
#endif

	if (m_iFireMode == FM_BURST && (m_nShotsFired < GetMaxBurst()))
	{
		m_flNextPrimaryAttack = gpGlobals->curtime + 0.085; // TODO: GetBurstFireRate()
	}
	else
	{
		m_flNextPrimaryAttack = gpGlobals->curtime + fireRate;
	}
}

void CWeaponHL2MPBase::SecondaryAttack(void)
{
	if (!HasIronsights())
		return;

	ToggleIronsights();

	m_flNextSecondaryAttack = gpGlobals->curtime + 0.4f;
}

bool CWeaponHL2MPBase::Holster(CBaseCombatWeapon *pSwitchingTo)
{
	if (m_bIsIronsighted)
		DisableIronsights();

	StopWeaponSound(RELOAD);

	return BaseClass::Holster(pSwitchingTo);
}

void CWeaponHL2MPBase::Drop(const Vector &vecVelocity)
{
	if (m_bIsIronsighted)
		DisableIronsights();

	StopWeaponSound(RELOAD);

	return BaseClass::Drop(vecVelocity);
}

bool CWeaponHL2MPBase::Reload(void)
{
	if (!BaseClass::Reload())
		return false;

	m_nShotsFired = 0;

	if (m_bIsIronsighted)
		DisableIronsights();

	return true;
}

void CWeaponHL2MPBase::DoMachineGunKick(CBasePlayer *pPlayer, float dampEasy, float maxVerticleKickAngle, float fireDurationTime, float slideLimitTime)
{
#define	KICK_MIN_X			0.2f	//Degrees
#define	KICK_MIN_Y			0.2f	//Degrees
#define	KICK_MIN_Z			0.1f	//Degrees

	QAngle vecScratch;
	int iSeed = CBaseEntity::GetPredictionRandomSeed() & 255;

	//Find how far into our accuracy degradation we are
	float duration = (fireDurationTime > slideLimitTime) ? slideLimitTime : fireDurationTime;
	float kickPerc = duration / slideLimitTime;

	// do this to get a hard discontinuity, clear out anything under 10 degrees punch
	pPlayer->ViewPunchReset(10);

	//Apply this to the view angles as well
	vecScratch.x = -(KICK_MIN_X + (maxVerticleKickAngle * kickPerc));
	vecScratch.y = -(KICK_MIN_Y + (maxVerticleKickAngle * kickPerc)) / 3;
	vecScratch.z = KICK_MIN_Z + (maxVerticleKickAngle * kickPerc) / 8;

	RandomSeed(iSeed);

	//Wibble left and right
	if (RandomInt(-1, 1) >= 0)
		vecScratch.y *= -1;

	iSeed++;

	//Wobble up and down
	if (RandomInt(-1, 1) >= 0)
		vecScratch.z *= -1;

	//Clip this to our desired min/max
	UTIL_ClipPunchAngleOffset(vecScratch, pPlayer->m_Local.m_vecPunchAngle, QAngle(24.0f, 3.0f, 1.0f));

	//Add it to the view punch
	// NOTE: 0.5 is just tuned to match the old effect before the punch became simulated
	pPlayer->ViewPunch(vecScratch * 0.5);
}

//-----------------------------------------------------------------------------
// Purpose: Make enough sound events to fill the estimated think interval
// returns: number of shots needed
//-----------------------------------------------------------------------------
int CWeaponHL2MPBase::WeaponSoundRealtime(WeaponSound_t shoot_type)
{
	int numBullets = 0;

	// ran out of time, clamp to current
	if (m_flNextSoundTime < gpGlobals->curtime)
	{
		m_flNextSoundTime = gpGlobals->curtime;
	}

	// make enough sound events to fill up the next estimated think interval
	float dt = clamp(m_flAnimTime - m_flPrevAnimTime, 0, 0.2);
	if (m_flNextSoundTime < gpGlobals->curtime + dt)
	{
		WeaponSound(SINGLE_NPC, m_flNextSoundTime);
		m_flNextSoundTime += GetFireRate();
		numBullets++;
	}
	if (m_flNextSoundTime < gpGlobals->curtime + dt)
	{
		WeaponSound(SINGLE_NPC, m_flNextSoundTime);
		m_flNextSoundTime += GetFireRate();
		numBullets++;
	}

	return numBullets;
}

Activity CWeaponHL2MPBase::GetPrimaryAttackActivity(void)
{
	if (m_bIsIronsighted)
		return ACT_VM_DRYFIRE; // Pretty much no recoil in animation for most weapons, should be replaced with custom animation

	// Show the normal recoil while no zoomed in
	if (m_nShotsFired < 2)
		return ACT_VM_PRIMARYATTACK;

	if (m_nShotsFired < 3)
		return ACT_VM_RECOIL1;

	if (m_nShotsFired < 4)
		return ACT_VM_RECOIL2;

	return ACT_VM_RECOIL3;
}