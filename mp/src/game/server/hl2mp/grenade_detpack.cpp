#include "cbase.h"
#include "explode.h"
#include "grenade_detpack.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

BEGIN_DATADESC(CDetpack)
	DEFINE_INPUTFUNC(FIELD_VOID, "Explode", InputExplode),
END_DATADESC()

LINK_ENTITY_TO_CLASS(grenade_detpack, CDetpack);

CDetpack::CDetpack(void)
{

}

void CDetpack::Spawn(void)
{
	Precache();
	SetModel("models/weapons/w_detpack.mdl");

	VPhysicsInitNormal(SOLID_BBOX, GetSolidFlags() | FSOLID_TRIGGER, false);
	SetMoveType(MOVETYPE_VPHYSICS);

	SetCollisionGroup(COLLISION_GROUP_WEAPON);

	UTIL_SetSize(this, Vector(-6, -6, -2), Vector(6, 6, 2));

	SetGravity(UTIL_ScaleForGravity(560));	// slightly lower gravity
	SetFriction(1.0);
}

void CDetpack::Precache(void)
{
	PrecacheModel("models/weapons/w_detpack.mdl");
	PrecacheSound("Weapon_Detpack.Special1");
}

void CDetpack::InputExplode(inputdata_t &inputdata)
{
	ExplosionCreate(GetAbsOrigin() + Vector(0, 0, 16), GetAbsAngles(), GetThrower(), GetDamage(), GetDamageRadius(),
		SF_ENVEXPLOSION_NOSPARKS | SF_ENVEXPLOSION_NODLIGHTS | SF_ENVEXPLOSION_NOSMOKE, 0.0f, this);

	EmitSound("Weapon_Detpack.Special1");

	UTIL_Remove(this);
}