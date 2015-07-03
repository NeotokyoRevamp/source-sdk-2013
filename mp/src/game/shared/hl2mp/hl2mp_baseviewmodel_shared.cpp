#include "cbase.h"

#include "hl2mp_baseviewmodel_shared.h"

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
	// TODO: Fix origin and use ironsights if needed

	BaseClass::CalcViewModelView(owner, eyePosition, eyeAngles);
}

LINK_ENTITY_TO_CLASS(hl2mp_viewmodel, CBaseHL2MPViewModel);

IMPLEMENT_NETWORKCLASS_ALIASED(BaseHL2MPViewModel, DT_BaseHL2MPViewModel)

BEGIN_NETWORK_TABLE_NOBASE(CBaseHL2MPViewModel, DT_BaseHL2MPViewModel)
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA(CBaseHL2MPViewModel)
END_PREDICTION_DATA()