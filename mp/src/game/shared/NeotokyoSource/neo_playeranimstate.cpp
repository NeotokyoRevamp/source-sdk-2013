#include "cbase.h"
#include "neo_playeranimstate.h"
#include "base_playeranimstate.h"
#include "c_neoplayer.h"
#include "weapon_neobase.h"
#include "animation.h"

// Mostly from CSS' source

#define ANIM_TOPSPEED_WALK			100
#define ANIM_TOPSPEED_RUN			250
#define ANIM_TOPSPEED_RUN_CROUCH	85

#define DEFAULT_IDLE_NAME "idle_upper_"
#define DEFAULT_CROUCH_IDLE_NAME "crouch_idle_upper_"
#define DEFAULT_CROUCH_WALK_NAME "crouch_walk_upper_"
#define DEFAULT_WALK_NAME "walk_upper_"
#define DEFAULT_RUN_NAME "run_upper_"

#define DEFAULT_FIRE_IDLE_NAME "idle_shoot_"
#define DEFAULT_FIRE_CROUCH_NAME "crouch_idle_shoot_"
#define DEFAULT_FIRE_CROUCH_WALK_NAME "crouch_walk_shoot_"
#define DEFAULT_FIRE_WALK_NAME "walk_shoot_"
#define DEFAULT_FIRE_RUN_NAME "run_shoot_"

// I'm not sure about these values
#define FIRESEQUENCE_LAYER		(AIMSEQUENCE_LAYER+NUM_AIMSEQUENCE_LAYERS)
#define RELOADSEQUENCE_LAYER	(FIRESEQUENCE_LAYER+1)
#define GRENADESEQUENCE_LAYER	(RELOADSEQUENCE_LAYER+1)
#define NUM_LAYERS_WANTED		(GRENADESEQUENCE_LAYER+1)

float g_flThrowGrenadeFraction = 0.25;


class CNEOPlayerAnimState : public CBasePlayerAnimState, public INEOPlayerAnimState
{
public:
	DECLARE_CLASS( CNEOPlayerAnimState, CBasePlayerAnimState );
	friend INEOPlayerAnimState* CreatePlayerAnimState( CBaseAnimatingOverlay *pEntity, INEOPlayerAnimStateHelpers *pHelpers, LegAnimType_t legAnimType, bool bUseAimSequences );

	CNEOPlayerAnimState();

	virtual void DoAnimationEvent( PlayerAnimEvent_t event, int nData );
	virtual bool IsThrowingGrenade();

	virtual void ClearAnimationLayers();
	virtual void ComputeSequences( CStudioHdr *pStudioHdr );
	virtual Activity CalcMainActivity();
	virtual int CalcAimLayerSequence( float *flCycle, float *flAimSequenceWeight, bool bForceIdle );
	virtual bool CanThePlayerMove();
	virtual float GetCurrentMaxGroundSpeed();
	virtual void DebugShowAnimState( int iStartLine );
	virtual void ComputePoseParam_BodyPitch( CStudioHdr *pStudioHdr );

	void InitNEO( CBaseAnimatingOverlay *pPlayer, INEOPlayerAnimStateHelpers *pHelpers, LegAnimType_t legAnimType, bool bUseAimSequences );

protected:
	int CalcReloadLayerSequence();
	void ComputeFireSequence( CStudioHdr *pStudioHdr );

	void ComputeReloadSequence( CStudioHdr *pStudioHdr );
	int CalcFireLayerSequence( PlayerAnimEvent_t event );

	int GetOuterGrenadeThrowCounter();
	bool IsOuterGrenadePrimed();
	int CalcGrenadePrimeSequence();
	int CalcGrenadeThrowSequence();
	void ComputeGrenadeSequence( CStudioHdr *pStudioHdr );

	const char* GetWeaponSuffix();
	bool HandleJumping();

	void UpdateLayerSequenceGeneric( CStudioHdr *pStudioHdr, int iLayer, bool &bEnabled, float &flCurCycle, int &iSequence, bool bWaitAtEnd );

private:
	bool m_bJumping;			// Set on a jump event.
	bool m_bUnknown; // It's very likely that it's related to jumping to be placed in here
	float m_flJumpStartTime;
	bool m_bFirstJumpFrame;

	// Aim sequence plays reload while this is on.
	bool m_bReloading;
	float m_flReloadCycle;
	int m_iReloadSequence;

	// This is set to true if ANY animation is being played in the fire layer.
	bool m_bFiring;						// If this is on, then it'll continue the fire animation in the fire layer
										// until it completes.
	int m_iFireSequence;				// (For any sequences in the fire layer, including grenade throw).
	float m_flFireCycle;

	// These control grenade animations.
	bool m_bThrowingGrenade;
	bool m_bPrimingGrenade;
	float m_flGrenadeCycle;
	int m_iGrenadeSequence;
	int m_iLastThrowGrenadeCounter;	// used to detect when the guy threw the grenade.

	INEOPlayerAnimStateHelpers* m_pHelpers;
};


INEOPlayerAnimState* CreatePlayerAnimState( CBaseAnimatingOverlay *pEntity, INEOPlayerAnimStateHelpers *pHelpers, LegAnimType_t legAnimType, bool bUseAimSequences )
{
	CNEOPlayerAnimState *pRet = new CNEOPlayerAnimState;
	pRet->InitNEO( pEntity, pHelpers, legAnimType, bUseAimSequences );
	return pRet;
}


CNEOPlayerAnimState::CNEOPlayerAnimState()
{
	m_pOuter = nullptr;

	m_bUnknown = false;
}

void CNEOPlayerAnimState::InitNEO( CBaseAnimatingOverlay *pEntity, INEOPlayerAnimStateHelpers *pHelpers, LegAnimType_t legAnimType, bool bUseAimSequences )
{
	CModAnimConfig config;
	config.m_flMaxBodyYawDegrees = 90;
	config.m_LegAnimType = legAnimType;
	config.m_bUseAimSequences = bUseAimSequences;

	m_pHelpers = pHelpers;

	BaseClass::Init( pEntity, config );
}

const char* CNEOPlayerAnimState::GetWeaponSuffix()
{
	// Figure out the weapon suffix.
	CWeaponNEOBase *pWeapon = m_pHelpers->NEOAnim_GetActiveWeapon();
	if ( !pWeapon )
		return nullptr;

	return pWeapon->GetNEOWpnData().szAnimationPrefix;
}

void CNEOPlayerAnimState::DoAnimationEvent( PlayerAnimEvent_t event, int nData )
{
	switch ( event )
	{
		case PLAYERANIMEVENT_FIRE_GUN_PRIMARY:
		case PLAYERANIMEVENT_FIRE_GUN_SECONDARY:
			// Regardless of what we're doing in the fire layer, restart it.
			m_flFireCycle = 0;
			m_iFireSequence = CalcFireLayerSequence( event );
			m_bFiring = m_iFireSequence != -1;
			break;

		case PLAYERANIMEVENT_JUMP:
			// Play the jump animation.
			m_bJumping = true;
			m_bFirstJumpFrame = true;
			m_flJumpStartTime = gpGlobals->curtime;
			break;

		case PLAYERANIMEVENT_RELOAD:
			m_iReloadSequence = CalcReloadLayerSequence();

			if ( m_iReloadSequence != -1 )
			{
				m_bReloading = true;
				m_flReloadCycle = 0;
			}
	}
}

bool CNEOPlayerAnimState::IsThrowingGrenade()
{
	if ( m_bThrowingGrenade )
	{
		// An animation event would be more appropriate here.
		return m_flGrenadeCycle < g_flThrowGrenadeFraction;
	}
	else
	{
		bool bThrowPending = (m_iLastThrowGrenadeCounter != GetOuterGrenadeThrowCounter());
		return bThrowPending || IsOuterGrenadePrimed();
	}
}

void CNEOPlayerAnimState::ClearAnimationLayers()
{
	if ( !m_pOuter )
		return;

	m_pOuter->SetNumAnimOverlays( NUM_LAYERS_WANTED );
	for ( int i = 0; i < m_pOuter->GetNumAnimOverlays(); i++ )
	{
		m_pOuter->GetAnimOverlay( i )->SetOrder( CBaseAnimatingOverlay::MAX_OVERLAYS );
#ifndef CLIENT_DLL
		m_pOuter->GetAnimOverlay( i )->m_fFlags = 0;
#endif
	}
}

void CNEOPlayerAnimState::ComputeSequences( CStudioHdr *pStudioHdr )
{
	BaseClass::ComputeSequences( pStudioHdr );

	ComputeFireSequence( pStudioHdr );
	ComputeReloadSequence( pStudioHdr );
	ComputeGrenadeSequence( pStudioHdr );

	C_NEOPlayer* localPlayer = C_NEOPlayer::GetLocalNEOPlayer();

	localPlayer->UpdateSomething2();
	localPlayer->UpdateThermoptic();
	localPlayer->NEO_MuzzleFlash();
}

Activity CNEOPlayerAnimState::CalcMainActivity()
{
	float flOuterSpeed = GetOuterXYSpeed();

	if ( HandleJumping() )
	{
		return ACT_HOP;
	}
	else
	{
		Activity idealActivity = ACT_IDLE;

		if ( m_pOuter->GetFlags() & FL_DUCKING )
		{
			if ( flOuterSpeed > MOVING_MINIMUM_SPEED )
				idealActivity = ACT_RUN_CROUCH;
			else
				idealActivity = ACT_CROUCHIDLE;
		}
		else
		{
			if ( flOuterSpeed > MOVING_MINIMUM_SPEED )
			{
				if ( flOuterSpeed > ARBITRARY_RUN_SPEED )
					idealActivity = ACT_RUN;
				else
					idealActivity = ACT_WALK;
			}
			else
			{
				idealActivity = ACT_IDLE;
			}
		}

		return idealActivity;
	}
}

int CNEOPlayerAnimState::CalcAimLayerSequence( float *flCycle, float *flAimSequenceWeight, bool bForceIdle )
{
	const char *pSuffix = GetWeaponSuffix();

	if ( !pSuffix )
		pSuffix = "Pistol";

	if ( bForceIdle )
	{
		switch ( GetCurrentMainSequenceActivity() )
		{
			case ACT_CROUCHIDLE:
			case ACT_RUN_CROUCH:
				return CalcSequenceIndex( "%s%s", DEFAULT_CROUCH_IDLE_NAME, pSuffix );

			default:
				return CalcSequenceIndex( "%s%s", DEFAULT_IDLE_NAME, pSuffix );
		}
	}
	else
	{
		switch ( GetCurrentMainSequenceActivity() )
		{
			case ACT_RUN:
				return CalcSequenceIndex( "%s%s", DEFAULT_RUN_NAME, pSuffix );

			case ACT_WALK:
			case ACT_RUNTOIDLE:
			case ACT_IDLETORUN:
				return CalcSequenceIndex( "%s%s", DEFAULT_WALK_NAME, pSuffix );

			case ACT_CROUCHIDLE:
				return CalcSequenceIndex( "%s%s", DEFAULT_CROUCH_IDLE_NAME, pSuffix );

			case ACT_RUN_CROUCH:
				return CalcSequenceIndex( "%s%s", DEFAULT_CROUCH_WALK_NAME, pSuffix );

			case ACT_IDLE:
			default:
				return CalcSequenceIndex( "%s%s", DEFAULT_IDLE_NAME, pSuffix );
		}
	}
}

bool CNEOPlayerAnimState::CanThePlayerMove()
{
	return m_pHelpers->NEOAnim_CanMove();
}

float CNEOPlayerAnimState::GetCurrentMaxGroundSpeed()
{
	Activity currentActivity = m_pOuter->GetSequenceActivity( m_pOuter->GetSequence() );

	if ( currentActivity == ACT_WALK || currentActivity == ACT_IDLE )
		return ANIM_TOPSPEED_WALK;

	else if ( currentActivity == ACT_RUN )
		return ANIM_TOPSPEED_RUN;

	else if ( currentActivity == ACT_RUN_CROUCH )
		return ANIM_TOPSPEED_RUN_CROUCH;

	else
		return 0;
}

void CNEOPlayerAnimState::DebugShowAnimState( int iStartLine )
{
	engine->Con_NPrintf( iStartLine++, "fire  : %s, cycle: %.2f\n", m_bFiring ? GetSequenceName( m_pOuter->GetModelPtr(), m_iFireSequence ) : "[not firing]", m_flFireCycle );
	engine->Con_NPrintf( iStartLine++, "reload: %s, cycle: %.2f\n", m_bReloading ? GetSequenceName( m_pOuter->GetModelPtr(), m_iReloadSequence ) : "[not reloading]", m_flReloadCycle );
	BaseClass::DebugShowAnimState( iStartLine );
}

void CNEOPlayerAnimState::ComputePoseParam_BodyPitch( CStudioHdr *pStudioHdr )
{
	BaseClass::ComputePoseParam_BodyPitch( pStudioHdr );
}

bool CNEOPlayerAnimState::HandleJumping()
{
	// Might have to look at this if statement later
	if ( !m_bJumping && !(m_pOuter->GetFlags() & FL_ONGROUND) )
	{
		m_bJumping = true;
		m_bFirstJumpFrame = true;
		m_flJumpStartTime = gpGlobals->curtime;
	}

	if ( m_bJumping )
	{
		if ( m_bFirstJumpFrame )
		{
			m_bFirstJumpFrame = false;
			RestartMainSequence();	// Reset the animation.
		}

		// Don't check if he's on the ground for a sec.. sometimes the client still has the
		// on-ground flag set right when the message comes in.
		if ( gpGlobals->curtime - m_flJumpStartTime > 0.2f )
		{
			if ( m_pOuter->GetFlags() & FL_ONGROUND )
			{
				m_bJumping = false;
				RestartMainSequence();	// Reset the animation.
			}
		}
	}

	// Are we still jumping? If so, keep playing the jump animation.
	return m_bJumping;
}

void CNEOPlayerAnimState::ComputeFireSequence( CStudioHdr *pStudioHdr )
{
	UpdateLayerSequenceGeneric( pStudioHdr, FIRESEQUENCE_LAYER, m_bFiring, m_flFireCycle, m_iFireSequence, false );
}

void CNEOPlayerAnimState::ComputeReloadSequence( CStudioHdr *pStudioHdr )
{
	UpdateLayerSequenceGeneric( pStudioHdr, RELOADSEQUENCE_LAYER, m_bReloading, m_flReloadCycle, m_iReloadSequence, false );
}

// Should we be returning -1 when these functions fail?
int CNEOPlayerAnimState::CalcFireLayerSequence( PlayerAnimEvent_t event )
{
	// Figure out the weapon suffix.
	CWeaponNEOBase *pWeapon = m_pHelpers->NEOAnim_GetActiveWeapon();
	if ( !pWeapon )
		return 0;

	const char *pSuffix = GetWeaponSuffix();

	if ( !pSuffix )
		return 0;

	if ( event == PLAYERANIMEVENT_THROW_GRENADE )
		pSuffix = "Gren";

	switch ( GetCurrentMainSequenceActivity() )
	{
		case ACT_PLAYER_RUN_FIRE:
		case ACT_RUN:
			return CalcSequenceIndex( "%s%s", DEFAULT_FIRE_RUN_NAME, pSuffix );

		case ACT_PLAYER_WALK_FIRE:
		case ACT_WALK:
			return CalcSequenceIndex( "%s%s", DEFAULT_FIRE_WALK_NAME, pSuffix );

		case ACT_PLAYER_CROUCH_FIRE:
		case ACT_CROUCHIDLE:
			return CalcSequenceIndex( "%s%s", DEFAULT_FIRE_CROUCH_NAME, pSuffix );

		case ACT_PLAYER_CROUCH_WALK_FIRE:
		case ACT_RUN_CROUCH:
			return CalcSequenceIndex( "%s%s", DEFAULT_FIRE_CROUCH_WALK_NAME, pSuffix );

		default:
		case ACT_PLAYER_IDLE_FIRE:
			return CalcSequenceIndex( "%s%s", DEFAULT_FIRE_IDLE_NAME, pSuffix );
	}
}

int CNEOPlayerAnimState::CalcReloadLayerSequence()
{
	const char* weaponSuffix = GetWeaponSuffix();

	if ( !weaponSuffix )
		return -1;

	char szName[ 512 ];

	V_snprintf( szName, sizeof( szName ), "reload_%s", weaponSuffix );
	int iReloadSequence = m_pOuter->LookupSequence( szName );
	if ( iReloadSequence != -1 )
		return iReloadSequence;

	return -1;
}


// I'm not sure if these functions are named correctly
int CNEOPlayerAnimState::CalcGrenadePrimeSequence()
{
	return CalcSequenceIndex( "Run_Upper_Grenade" );
}

int CNEOPlayerAnimState::CalcGrenadeThrowSequence()
{
	return CalcSequenceIndex( "idle_shoot_gren1" );
}

int CNEOPlayerAnimState::GetOuterGrenadeThrowCounter()
{
	C_NEOPlayer* player = dynamic_cast<C_NEOPlayer*>(m_pOuter);

	if ( player )
		return player->m_iThrowGrenadeCounter;

	return 0;
}

void CNEOPlayerAnimState::UpdateLayerSequenceGeneric( CStudioHdr *pStudioHdr, int iLayer, bool &bEnabled, float &flCurCycle, int &iSequence, bool bWaitAtEnd )
{
	if ( !bEnabled || iSequence < 0 )
		return;

	// Increment the fire sequence's cycle.
	flCurCycle += m_pOuter->GetSequenceCycleRate( pStudioHdr, iSequence ) * gpGlobals->frametime;
	if ( flCurCycle > 1 )
	{
		if ( bWaitAtEnd )
		{
			flCurCycle = 1;
		}
		else
		{
			// Not firing anymore.
			bEnabled = false;
			iSequence = 0;
			return;
		}
	}

	// Now dump the state into its animation layer.
	CAnimationLayer *pLayer = m_pOuter->GetAnimOverlay( iLayer );

	pLayer->m_flCycle = flCurCycle;
	pLayer->m_nSequence = iSequence;

	pLayer->m_flPlaybackRate = 1.0f;
	pLayer->m_flWeight = 1.0f;
	pLayer->m_nOrder = iLayer;
#ifndef CLIENT_DLL
	pLayer->m_fFlags |= ANIM_LAYER_ACTIVE;
#endif
}

bool CNEOPlayerAnimState::IsOuterGrenadePrimed()
{
	CBaseCombatCharacter *pChar = m_pOuter->MyCombatCharacterPointer();
	if ( pChar )
	{
		// !!! GOTTA REVERSE GRENADES FIRST !!!
		//CBaseNEOGrenade *pGren = dynamic_cast<CBaseNEOGrenade*>(pChar->GetActiveWeapon());
		//return pGren && pGren->IsPinPulled();
	}
	else
	{
		return NULL;
	}
}

void CNEOPlayerAnimState::ComputeGrenadeSequence( CStudioHdr *pStudioHdr )
{
	if ( m_bThrowingGrenade )
	{
		UpdateLayerSequenceGeneric( pStudioHdr, GRENADESEQUENCE_LAYER, m_bThrowingGrenade, m_flGrenadeCycle, m_iGrenadeSequence, false );
	}
	else
	{
		// Priming the grenade isn't an event.. we just watch the player for it.
		// Also play the prime animation first if he wants to throw the grenade.
		bool bThrowPending = (m_iLastThrowGrenadeCounter != GetOuterGrenadeThrowCounter());
		if ( IsOuterGrenadePrimed() || bThrowPending )
		{
			if ( !m_bPrimingGrenade )
			{
				// If this guy just popped into our PVS, and he's got his grenade primed, then
				// let's assume that it's all the way primed rather than playing the prime
				// animation from the start.
				if ( TimeSinceLastAnimationStateClear() < 0.4f )
				{
					m_flGrenadeCycle = 1;
				}
				else
				{
					m_flGrenadeCycle = 0;
				}

				m_iGrenadeSequence = CalcGrenadePrimeSequence();
			}

			m_bPrimingGrenade = true;
			UpdateLayerSequenceGeneric( pStudioHdr, GRENADESEQUENCE_LAYER, m_bPrimingGrenade, m_flGrenadeCycle, m_iGrenadeSequence, true );

			// If we're waiting to throw and we're done playing the prime animation...
			if ( bThrowPending && m_flGrenadeCycle == 1 )
			{
				m_iLastThrowGrenadeCounter = GetOuterGrenadeThrowCounter();

				// Now play the throw animation.
				m_iGrenadeSequence = CalcGrenadeThrowSequence();
				if ( m_iGrenadeSequence != -1 )
				{
					// Configure to start playing 
					m_bThrowingGrenade = true;
					m_bPrimingGrenade = false;
					m_flGrenadeCycle = 0;
				}
			}
		}
		else
		{
			m_bPrimingGrenade = false;
		}
	}
}