// TODO: Handle grenade specific things like explosion here

#include "cbase.h"
#include "grenade_explosive.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#define GRENADE_MODEL "models/weapons/w_frag.mdl"

BEGIN_DATADESC(CHEGrenade)
END_DATADESC()

LINK_ENTITY_TO_CLASS(grenade_explosive, CHEGrenade);

CHEGrenade::CHEGrenade(void)
{

}