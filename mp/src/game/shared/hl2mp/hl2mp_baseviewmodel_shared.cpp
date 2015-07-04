#include "cbase.h"

#include "hl2mp_baseviewmodel_shared.h"
#include "weapon_hl2mpbase.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

CBaseHL2MPViewModel::CBaseHL2MPViewModel()
{

}

CBaseHL2MPViewModel::~CBaseHL2MPViewModel()
{

}

void CBaseHL2MPViewModel::CalcViewModelView(CBasePlayer *owner, const Vector& eyePosition, const QAngle& eyeAngles)
{
	CWeaponHL2MPBase *pWeapon = static_cast<CWeaponHL2MPBase*> (GetOwningWeapon()); // Is there are nicer way to do this?

	if (!pWeapon)
		return;

	// TODO: Smooth the origin changes
	// TODO: Handle the weapon fov

	Vector vForward, vRight, vUp, newPos, vOffset;
	QAngle newAng, angOffset;

	newAng = eyeAngles;
	newPos = eyePosition;

	AngleVectors(newAng, &vForward, &vRight, &vUp);

	vOffset = pWeapon->GetViewModelPositionOffset();
	angOffset = pWeapon->GetViewModelAngleOffset();

	newPos += vForward * vOffset.x;
	newPos += vRight * vOffset.y;
	newPos += vUp * vOffset.z;
	newAng += angOffset;

	BaseClass::CalcViewModelView(owner, newPos, newAng);
}

LINK_ENTITY_TO_CLASS(hl2mp_viewmodel, CBaseHL2MPViewModel);

IMPLEMENT_NETWORKCLASS_ALIASED(BaseHL2MPViewModel, DT_BaseHL2MPViewModel)

BEGIN_NETWORK_TABLE_NOBASE(CBaseHL2MPViewModel, DT_BaseHL2MPViewModel)
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA(CBaseHL2MPViewModel)
END_PREDICTION_DATA()