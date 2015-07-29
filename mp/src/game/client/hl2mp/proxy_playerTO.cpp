#include "cbase.h"
#include "c_hl2mp_player.h"
#include "materialsystem/imaterialvar.h"
#include "materialsystem/imaterialproxy.h"
#include "baseviewmodel_shared.h"
 
class CProxy_PlayerTO : public IMaterialProxy
{
public:
    CProxy_PlayerTO();
    virtual ~CProxy_PlayerTO();
    virtual bool Init( IMaterial *pMaterial, KeyValues *pKeyValues );
    C_BaseEntity *BindArgToEntity( void *pArg );
    virtual void OnBind( void* C_BaseEntity );
    virtual void Release( void ) { delete this; }
    IMaterial *GetMaterial( void );
 
private:
    IMaterialVar* thermopticVar;
};
 
CProxy_PlayerTO::CProxy_PlayerTO()
{
    thermopticVar = NULL;
}
 
CProxy_PlayerTO::~CProxy_PlayerTO()
{
}
 
bool CProxy_PlayerTO::Init( IMaterial *pMaterial, KeyValues *pKeyValues )
{
    bool found;
 
    thermopticVar = pMaterial->FindVar( "$thermoptic", &found, false );
    if ( !found )
        return false;
 
    return true;
}
 
C_BaseEntity *CProxy_PlayerTO::BindArgToEntity( void *pArg )
{
    IClientRenderable *pRend = (IClientRenderable *)pArg;
    return pRend ? pRend->GetIClientUnknown()->GetBaseEntity() : NULL;
}
 
void CProxy_PlayerTO::OnBind( void* pC_BaseEntity )
{
    if ( !pC_BaseEntity )
        return;
 
    C_BaseEntity *pEntity = BindArgToEntity( pC_BaseEntity );
 
    //If this is a player's viewmodel...
    if ( C_BaseViewModel *pViewModel = dynamic_cast< C_BaseViewModel *>(pEntity) )
    {
        C_HL2MP_Player *pPlayer = static_cast<C_HL2MP_Player *>(ToBasePlayer( pViewModel->GetOwner() ));
        thermopticVar->SetFloatValue( 1 - pPlayer->GetCloakFactor() );
    }
 
    //If this is a non-player character...
    else if ( C_HL2MP_Player *pNPC = dynamic_cast< C_HL2MP_Player *>(pEntity) )
    {
        thermopticVar->SetFloatValue( 1 - pNPC->GetCloakFactor() );
    }
 
    //If this is a weapon's worldmodel (under the assumption it's in something's possesion)...
    else if ( C_BaseCombatWeapon *pWeapon = dynamic_cast< C_BaseCombatWeapon *>(pEntity) )
    {
        C_HL2MP_Player *pOwner = static_cast<C_HL2MP_Player *>(ToBaseCombatCharacter( pWeapon->GetOwner() ));
        if ( !pOwner )
            return;
 
        thermopticVar->SetFloatValue( 1 - pOwner->GetCloakFactor() );
    }
    else
        return;
}
IMaterial *CProxy_PlayerTO::GetMaterial()
{
    return thermopticVar->GetOwningMaterial();
}
EXPOSE_INTERFACE( CProxy_PlayerTO, IMaterialProxy, "PlayerTO" IMATERIAL_PROXY_INTERFACE_VERSION );
