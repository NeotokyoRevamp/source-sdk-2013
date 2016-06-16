#include "weapon_neobase.h"
#include "c_neoplayer.h"
#include "materialsystem\imaterialvar.h"
#include "keyvalues.h"
#include "c_recipientfilter.h"
#include "iefx.h"
#include "dlight.h"

// We use these on the c&p CalcViewmodelBob
#define	HL2_BOB_CYCLE_MAX	0.45f
#define	HL2_BOB_UP		0.5f

float g_lateralBob = 0.f;
float g_verticalBob = 0.f;

ConVar v_vmtweak( "v_vmtweak", "0", FCVAR_CLIENTDLL );

ConVar v_vm_aimfov( "v_vm_aimfov", "54.0", FCVAR_CLIENTDLL );
ConVar v_vmaimoffsetup( "v_vmaimoffsetup", "0.0", FCVAR_CLIENTDLL );
ConVar v_vmaimoffsetforward( "v_vmaimoffsetforward", "0.0", FCVAR_CLIENTDLL );
ConVar v_vmaimoffsetright( "v_vmaimoffsetright", "0.0", FCVAR_CLIENTDLL );
ConVar v_vmaimangleyaw( "v_vmaimangleyaw", "0.0", FCVAR_CLIENTDLL );
ConVar v_vmaimanglepitch( "v_vmaimanglepitch", "0.0", FCVAR_CLIENTDLL );
ConVar v_vmaimangleroll( "v_vmaimangleroll", "0.0", FCVAR_CLIENTDLL );

ConVar v_vm_fov( "v_vm_fov", "54.0", FCVAR_CLIENTDLL );
ConVar v_vmoffsetup( "v_vmoffsetup", "0.0", FCVAR_CLIENTDLL );
ConVar v_vmoffsetforward( "v_vmoffsetforward", "0.0", FCVAR_CLIENTDLL );
ConVar v_vmoffsetright( "v_vmoffsetright", "0.0", FCVAR_CLIENTDLL );
ConVar v_vmangleyaw( "v_vmangleyaw", "0.0", FCVAR_CLIENTDLL );
ConVar v_vmanglepitch( "v_vmanglepitch", "0.0", FCVAR_CLIENTDLL );
ConVar v_vmangleroll( "v_vmangleroll", "0.0", FCVAR_CLIENTDLL );

static const char* s_WeaponAliasInfo[] =
{
	"none",	
	"mp5",		
	"supa7",
	"grenade",
	"hl2pistol",
	"tachi",
	"zr68s",
	"zr68c",
	"zr68l",
	"mx",
	"milso",
	"srm",
	"srm_s",
	"milso_s",
	"mpn",
	"pz",
	"jitte",
	"jittescoped",
	"ghost",
	"smac",
	"m41",
	"m41l",
	"smokegrenade",
	"remotedet",
	"kyla",
	"aa13",
	"knife",
	"mx_silenced",
	"srs",
	"m41s",
	NULL,
};

const char* WeaponIDToAlias( int id )
{
	if ( ( id >= WEAPON_MAX) || ( id < 0 ) )
		return NULL;

	return s_WeaponAliasInfo[ id ];
}

bool CreateThermopticMaterial( IMaterial* material )
{
	if ( !g_pMaterialSystem )
		return false;

	KeyValues* keyValues = new KeyValues( "Refract" );

	material = g_pMaterialSystem->CreateMaterial( "thermoptic", keyValues );

	if ( !material )
		return false;

	bool found = false;

	material->SetShader( "Refract" );
	material->SetMaterialVarFlag( MATERIAL_VAR_MODEL, true );
	material->SetMaterialVarFlag( MATERIAL_VAR_SUPPRESS_DECALS, true );

	IMaterialVar* refractamount = material->FindVar( "$refractamount", &found );

	if ( found )
		refractamount->SetFloatValue( 0.8f );

	IMaterialVar* normalmap = material->FindVar( "$normalmap", &found );

	if ( found )
	{
		ITexture* waterTexture = g_pMaterialSystem->FindTexture( "dev/water_normal", "ClientEffect textures" );

		if ( waterTexture )
			normalmap->SetTextureValue( waterTexture );
	}

	IMaterialVar* bumpframe = material->FindVar( "$bumpframe", &found );

	if ( found )
		bumpframe->SetIntValue( 0 );

	return true;
}

IMaterial* GetThermopticMaterial()
{
	static IMaterial* pThermopticMaterial;

	if ( pThermopticMaterial || CreateThermopticMaterial( pThermopticMaterial ) )
		return pThermopticMaterial;

	return nullptr;
}

// This function has really some nasty stuff going on :/
void UpdateThermopticMaterial( IMaterial* material, float a2 )
{
	*(int*) ((uintp) material + 4) = a2 * 0.2f; // material + 4 is CMaterial::m_iEnumerationID, which we can't set through IMaterial. I'll have to look for a lookaround later. -Ochii

	if ( !material )
		return;

	bool found = false;

	IMaterialVar* refractamount = material->FindVar( "$refractamount", &found );

	if ( found )
		refractamount->SetFloatValue( material->GetEnumerationID() * 0.035f );

	IMaterialVar* bluramount = material->FindVar( "$bluramount", &found );

	if ( found )
		bluramount->SetFloatValue( material->GetEnumerationID() * 20.f + 10.524f );

	IMaterialVar* refracttint = material->FindVar( "$refracttint", &found );

	if ( found )
		refracttint->SetVecValue( 0.85f - material->GetEnumerationID() * 0.8f, 0.85f - material->GetEnumerationID() * 0.6f, 0.85f - material->GetEnumerationID() * 0.6f );

	IMaterialVar* fresnelreflection = material->FindVar( "$fresnelreflection", &found );

	if ( found )
		fresnelreflection->SetFloatValue( material->GetEnumerationID() * 2 + 0.25f );

	IMaterialVar* bumptransform = material->FindVar( "$bumptransform", &found );

	if ( found )
	{
		VMatrix matrix( 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f );

		if ( bumptransform->GetType() == MATERIAL_VAR_TYPE_MATRIX )
			bumptransform->SetMatrixValue( matrix );
		else
			bumptransform->SetVecValue( 0.f, 0.f, 0.f );
	}
}

IMPLEMENT_NETWORKCLASS_ALIASED( WeaponNEOBase, DT_WeaponNEOBase )

BEGIN_NETWORK_TABLE( CWeaponNEOBase, DT_WeaponNEOBase )
	RecvPropBool( RECVINFO( bAimed ) ),
	RecvPropFloat( RECVINFO( m_fAccuracy ) ),
	RecvPropInt( RECVINFO( m_iTeam2ViewModelIndex ) )
END_NETWORK_TABLE()

LINK_ENTITY_TO_CLASS( weapon_neo_base, CWeaponNEOBase );

CWeaponNEOBase::CWeaponNEOBase()
{
	SetPredictionEligible( true );
	AddSolidFlags(FSOLID_TRIGGER);

	bAimed = false;
	m_fAccuracy = 0.f;
}

void CWeaponNEOBase::Spawn()
{
	m_fMinRange1 = 0.f;
	m_fMinRange2 = 0.f;
	m_fMaxRange1 = 0.f;
	m_fMaxRange2 = 0.f;

	BaseClass::Spawn();
}

void CWeaponNEOBase::Precache()
{
	BaseClass::Precache();

	CNEOWeaponInfo* weaponInfo = GetNEOWeaponInfoFromHandle( GetWeaponFileInfoHandle() );

	m_bDrawCrosshair = weaponInfo->m_bDrawCrosshair;

	m_iAimType = weaponInfo->m_iAimType;
	m_iScopeStyle = weaponInfo->m_iScopeStyle;

	m_fVMFov = weaponInfo->m_fVMFov;
	m_fVMAimFov = weaponInfo->m_fVMAimFov;
	m_fAimFov = weaponInfo->m_fAimFov;

	m_fVMOffsetUp = weaponInfo->m_fVMOffsetUp;
	m_fVMOffsetForward = weaponInfo->m_fVMOffsetForward;
	m_fVMOffsetRight = weaponInfo->m_fVMOffsetRight;

	m_fVMAimOffsetUp = weaponInfo->m_fVMAimOffsetUp;
	m_fVMAimOffsetForward = weaponInfo->m_fVMAimOffsetForward;
	m_fVMAimOffsetRight = weaponInfo->m_fVMAimOffsetRight;

	m_fVMAngleYaw = weaponInfo->m_fVMAngleYaw;
	m_fVMAnglePitch = weaponInfo->m_fVMAnglePitch;
	m_fVMAngleRoll = weaponInfo->m_fVMAngleRoll;

	m_fVMAimAngleYaw = weaponInfo->m_fVMAimAngleYaw;
	m_fVMAimAnglePitch = weaponInfo->m_fVMAimAnglePitch;
	m_fVMAimAngleRoll = weaponInfo->m_fVMAimAngleRoll;

	m_fTPMuzzleFlashScale = weaponInfo->m_fTPMuzzleFlashScale;

	int viewmodelIndex = modelinfo->GetModelIndex( GetTeam2ViewModelName() );

	if ( m_iTeam2ViewModelIndex != viewmodelIndex )
		m_iTeam2ViewModelIndex = viewmodelIndex;
}

bool CWeaponNEOBase::ShouldPredict()
{
	if ( ToNEOPlayer( GetOwner() ) == C_NEOPlayer::GetLocalNEOPlayer() )
		return true;

	return false;
}

int CWeaponNEOBase::InternalDrawModel( int flags )
{
	C_NEOPlayer* owner = dynamic_cast< C_NEOPlayer* >( GetOwner() );
	C_NEOPlayer* localPlayer = C_NEOPlayer::GetLocalNEOPlayer();

	if ( !owner || !localPlayer )
		return BaseClass::InternalDrawModel( flags );

	if ( owner->GetThermoptic() == 1 )
	{
		if ( !g_pMaterialSystemHardwareConfig->SupportsPixelShaders_2_0() )
			return 0;

		IMaterial* thermopticMaterial = GetThermopticMaterial();

		UpdateThermopticMaterial( thermopticMaterial, owner->m_flUnknown );

		modelrender->ForcedMaterialOverride( thermopticMaterial );
		int result = BaseClass::InternalDrawModel( flags );
		modelrender->ForcedMaterialOverride( nullptr );

		return result;
	}

	else if ( owner->GetVision() == 3 ) // Thermal vision
	{
		if ( !g_pMaterialSystemHardwareConfig->SupportsPixelShaders_2_0() )
			return BaseClass::InternalDrawModel( flags );

		IMaterial* matThermal = g_pMaterialSystem->FindMaterial( "dev/vm_thermal", TEXTURE_GROUP_OTHER );

		if ( IsErrorMaterial( matThermal ) )
		{
			DevMsg( SPEW_MESSAGE, "Fuck me...\n" ); // Their message, not mine kek
			BaseClass::DrawModel( flags );
		}

		bool found = false;
		IMaterialVar* matVar = matThermal->FindVar( "$eyevec", &found );

		if ( found )
		{
			Vector forward;
			GetVectors( &forward, nullptr, nullptr );
			matVar->SetVecValue( forward.x, forward.y, forward.z, 3.f );
		}

		modelrender->ForcedMaterialOverride( matThermal );
		int result = BaseClass::InternalDrawModel( flags );
		modelrender->ForcedMaterialOverride( nullptr );

		return result;
	}

	if ( owner->GetVision() == 4 ) // Motion vision
	{
		if ( !g_pMaterialSystemHardwareConfig->SupportsPixelShaders_2_0() )
			return BaseClass::InternalDrawModel( flags );

		IMaterial* matMotion = g_pMaterialSystem->FindMaterial( "dev/motion", TEXTURE_GROUP_OTHER );

		if ( IsErrorMaterial( matMotion ) )
		{
			DevMsg( SPEW_MESSAGE, "Fuck me...\n" ); 
			BaseClass::DrawModel( flags );
		}

		float velocity = localPlayer->GetLocalVelocity().Length() / 75.f;

		if ( velocity > 4.f )
			velocity = 4.f;

		bool found = false;
		IMaterialVar* matVar = matMotion->FindVar( "$eyevec", &found );

		if ( found )
		{
			Vector forward;
			GetVectors( &forward, nullptr, nullptr );
			matVar->SetVecValue( forward.x, forward.y, forward.z, velocity );
		}

		modelrender->ForcedMaterialOverride( matMotion );
		int result = BaseClass::InternalDrawModel( flags );
		modelrender->ForcedMaterialOverride( nullptr );

		return result;
	}

	return BaseClass::InternalDrawModel( flags );
}

void CWeaponNEOBase::SetViewModel()
{
	C_NEOPlayer* owner = ToNEOPlayer( GetOwner() );

	if ( !owner )
		return;

	C_BaseViewModel* viewModel = owner->GetViewModel( m_nViewModelIndex );

	if ( !viewModel )
		return;

	viewModel->SetWeaponModel( GetWorldModel(), this );
}

void CWeaponNEOBase::ItemPostFrame()
{
	C_NEOPlayer* owner = ToNEOPlayer( GetOwner() );

	if ( owner->GetObserverMode() != OBS_MODE_NONE )
		return;

	/* if ( g_pGameRules->0x64 )
	{
		input->ClearInputButton( 1 );
		return;
	}*/ // I need to check CNEOGameRules

	if ( !owner || owner->GetSprint() == 1 || owner->GetMoveType() == MOVETYPE_LADDER )
		return;

	if ( m_bUnknown
		&& owner->m_nButtons & 0x40000000 // Custom IN_ flag? ( 1 << 30 )
		&& gpGlobals->curtime > m_flNextPrimaryAttack && gpGlobals->curtime > m_flNextSecondaryAttack )
	{
		m_bUnknown2 = true;
		m_fUnknown = gpGlobals->curtime + 0.25f;

		if ( bAimed )
			bAimed = false;

		SendWeaponAnim( ACT_SLAM_STICKWALL_ND_IDLE );

		float seqDuration = SequenceDuration() + gpGlobals->curtime;

		if ( m_flNextSecondaryAttack != seqDuration )
			m_flNextSecondaryAttack = seqDuration;

		if ( m_flNextPrimaryAttack != m_flNextSecondaryAttack )
			m_flNextPrimaryAttack = m_flNextSecondaryAttack;

		SetWeaponIdleTime( seqDuration );
	}

	else if ( m_bUnknown2 && gpGlobals->curtime >= m_fUnknown )
	{
		m_bUnknown2 = false;
		// call to vfunc at 322, which is only implemented by shotguns (?). I have to add it here later.
	}
}

void CWeaponNEOBase::OverrideViewmodelBob( CBaseViewModel *viewmodel, Vector &origin, QAngle &angles )
{
	Vector forward, right;

	AngleVectors( angles, &forward, &right, nullptr );

	float fOffsetUp, fOffsetForward, fOffsetRight, fAngleYaw, fAnglePitch, fAngleRoll;

	if ( !v_vmtweak.GetBool() )
	{
		switch ( m_iUnknown )
		{
			case 1:
				m_fFov = m_fVMFov - m_fVMAimFov * m_fUnknown2 + m_fVMAimFov;
				fOffsetUp = m_fVMOffsetUp - m_fVMAimOffsetUp * m_fUnknown2 + m_fVMAimOffsetUp;
				fOffsetForward = m_fVMOffsetForward - m_fVMAimOffsetForward * m_fUnknown2 + m_fVMAimOffsetForward;
				fOffsetRight = m_fVMOffsetRight - m_fVMAimOffsetRight * m_fUnknown2 + m_fVMAimOffsetRight;
				fAngleYaw = m_fVMAngleYaw - m_fVMAimAngleYaw * m_fUnknown2 + m_fVMAimAngleYaw;
				fAnglePitch = m_fVMAnglePitch - m_fVMAimAnglePitch * m_fUnknown2 + m_fVMAimAnglePitch;
				fAngleRoll = m_fVMAngleRoll - m_fVMAimAngleRoll * m_fUnknown2 + m_fVMAimAngleRoll;
				break;

			case 2:
				m_fFov = m_fVMAimFov;
				fOffsetUp = m_fVMAimOffsetUp;
				fOffsetForward = m_fVMAimOffsetForward;
				fOffsetRight = m_fVMAimOffsetRight;
				fAngleYaw = m_fVMAimAngleYaw;
				fAnglePitch = m_fVMAimAnglePitch;
				fAngleRoll = m_fVMAimAngleRoll;
				break;

			case 3:
				m_fFov = m_fVMAimFov - m_fVMFov * m_fUnknown2 + m_fVMFov;
				fOffsetUp = m_fVMAimOffsetUp - m_fVMOffsetUp * m_fUnknown2 + m_fVMOffsetUp;
				fOffsetForward = m_fVMAimOffsetForward - m_fVMOffsetForward * m_fUnknown2 + m_fVMOffsetForward;
				fOffsetRight = m_fVMAimOffsetRight - m_fVMOffsetRight * m_fUnknown2 + m_fVMOffsetRight;
				fAngleYaw = m_fVMAimAngleYaw - m_fVMAngleYaw * m_fUnknown2 + m_fVMAngleYaw;
				fAnglePitch = m_fVMAimAnglePitch - m_fVMAnglePitch * m_fUnknown2 + m_fVMAnglePitch;
				fAngleRoll = m_fVMAimAngleRoll - m_fVMAngleRoll * m_fUnknown2 + m_fVMAngleRoll;
				break;		

			default:
				m_fFov = m_fVMFov;
				fOffsetUp = m_fVMOffsetUp;
				fOffsetForward = m_fVMOffsetForward;
				fOffsetRight = m_fVMOffsetRight;
				fAngleYaw = m_fVMAngleYaw;
				fAnglePitch = m_fVMAnglePitch;
				fAngleRoll = m_fVMAngleRoll;
		}
	}

	else
	{
		if ( bAimed )
		{
			m_fFov = v_vm_aimfov.GetFloat();
			fOffsetUp = v_vmaimoffsetup.GetFloat();
			fOffsetForward = v_vmaimoffsetforward.GetFloat();
			fOffsetRight = v_vmaimoffsetright.GetFloat();
			fAngleYaw = v_vmaimangleyaw.GetFloat();
			fAnglePitch = v_vmaimanglepitch.GetFloat();
			fAngleRoll = v_vmaimangleroll.GetFloat();
		}

		else
		{
			m_fFov = v_vm_fov.GetFloat();
			fOffsetUp = v_vmoffsetup.GetFloat();
			fOffsetForward = v_vmoffsetforward.GetFloat();
			fOffsetRight = v_vmoffsetright.GetFloat();
			fAngleYaw = v_vmangleyaw.GetFloat();
			fAnglePitch = v_vmanglepitch.GetFloat();
			fAngleRoll = v_vmangleroll.GetFloat();
		}
	}

	VectorMA( origin, fOffsetForward, forward, origin );
	VectorMA( origin, fOffsetRight, right, origin );

	origin.z += fOffsetUp;
	
	angles.x += fAnglePitch;
	angles.y += fAngleYaw;
	angles.z += fAngleRoll;
}

void CWeaponNEOBase::AddViewmodelBob( CBaseViewModel *viewmodel, Vector &origin, QAngle &angles )
{
	float bobMult = 1.f; // Eh I'm pretty sure we can name this better

	switch ( m_iUnknown )
	{
		case 1:
			bobMult = m_fUnknown2 * 0.5f + 0.5f;
			break;

		case 2:
			bobMult = 0.5f;
			break;

		case 3:
			bobMult = 1.f - 0.5f * m_fUnknown2;
	}

	Vector forward, right;

	AngleVectors( angles, &forward, &right, nullptr );

	CalcViewmodelBob();

	VectorMA( origin, g_verticalBob * 0.1 * bobMult, forward, origin );

	origin.z += g_verticalBob * 0.2f * bobMult;

	angles.z += g_verticalBob * 0.5f * bobMult;
	angles.x -= g_verticalBob * 0.4f * bobMult;

	angles.y -= g_lateralBob * 0.3f * bobMult;

	VectorMA( origin, bobMult * (g_lateralBob * 0.8f), right, origin );
}

// I c&p this from CBaseHLCombatWeapon since it seems to be exactly the same
float CWeaponNEOBase::CalcViewmodelBob()
{
	static	float bobtime;
	static	float lastbobtime;
	float	cycle;

	CBasePlayer *player = ToBasePlayer( GetOwner() );
	//Assert( player );

	//NOTENOTE: For now, let this cycle continue when in the air, because it snaps badly without it

	if ( (!gpGlobals->frametime) || (player == NULL) )
	{
		//NOTENOTE: We don't use this return value in our case (need to restructure the calculation function setup!)
		return 0.0f;// just use old value
	}

	//Find the speed of the player
	float speed = player->GetLocalVelocity().Length2D();

	//FIXME: This maximum speed value must come from the server.
	//		 MaxSpeed() is not sufficient for dealing with sprinting - jdw

	speed = clamp( speed, -320, 320 );

	float bob_offset = RemapVal( speed, 0, 320, 0.0f, 1.0f );

	bobtime += (gpGlobals->curtime - lastbobtime) * bob_offset;
	lastbobtime = gpGlobals->curtime;

	//Calculate the vertical bob
	cycle = bobtime - (int) (bobtime / HL2_BOB_CYCLE_MAX)*HL2_BOB_CYCLE_MAX;
	cycle /= HL2_BOB_CYCLE_MAX;

	if ( cycle < HL2_BOB_UP )
	{
		cycle = M_PI * cycle / HL2_BOB_UP;
	}
	else
	{
		cycle = M_PI + M_PI*(cycle - HL2_BOB_UP) / (1.0 - HL2_BOB_UP);
	}

	g_verticalBob = speed*0.005f;
	g_verticalBob = g_verticalBob*0.3 + g_verticalBob*0.7*sin( cycle );

	g_verticalBob = clamp( g_verticalBob, -7.0f, 4.0f );

	//Calculate the lateral bob
	cycle = bobtime - (int) (bobtime / HL2_BOB_CYCLE_MAX * 2)*HL2_BOB_CYCLE_MAX * 2;
	cycle /= HL2_BOB_CYCLE_MAX * 2;

	if ( cycle < HL2_BOB_UP )
	{
		cycle = M_PI * cycle / HL2_BOB_UP;
	}
	else
	{
		cycle = M_PI + M_PI*(cycle - HL2_BOB_UP) / (1.0 - HL2_BOB_UP);
	}

	g_lateralBob = speed*0.005f;
	g_lateralBob = g_lateralBob*0.3 + g_lateralBob*0.7*sin( cycle );
	g_lateralBob = clamp( g_lateralBob, -7.0f, 4.0f );

	//NOTENOTE: We don't use this return value in our case (need to restructure the calculation function setup!)
	return 0.0f;
}

bool CWeaponNEOBase::PlayEmptySound()
{
	CPASAttenuationFilter filter( this );
	EmitSound( filter, entindex(), "Default.ClipEmpty_Rifle" );
}

void CWeaponNEOBase::DoNEOMuzzleFlash()
{
	CNEOPlayer* owner = ToNEOPlayer( GetOwner() );

	if ( !owner )
		return;

	CBaseViewModel* viewModel = owner->GetViewModel( m_nViewModelIndex );

	if ( !viewModel )
		return;

	Vector origin;
	QAngle angles;

	bool result = viewModel->GetAttachment( 1, origin, angles );

	if ( !result )
		return;

	dlight_t* light = effects->CL_AllocDlight( LIGHT_INDEX_TE_DYNAMIC + index );

	if ( !light )
		return;

	light->origin = origin;
	light->radius = RandomFloat( 64.f, 96.f );
	light->decay = RandomFloat( 64.f, 96.f ) / 0.05f;
	light->color.r = 255;
	light->color.g = 192;
	light->color.b = 64;
	light->color.exponent = 5;
	light->die = gpGlobals->curtime + 0.05f;

	FX_MuzzleEffect( origin, angles, 1.f, 1 ); // 1 is the world index
}

const char* CWeaponNEOBase::GetTeam2ViewModelName()
{
	return ((CNEOWeaponInfo*) &GetWpnData())->m_szTeam2viewmodel; // Yeah this is kinda ugly
}