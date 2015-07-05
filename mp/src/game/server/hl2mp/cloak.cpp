#include "cbase.h"
#include "hl2mp_player.h"

void CC_Cloak( void )
{
    CHL2MP_Player *pPlayer = static_cast<CHL2MP_Player *>(UTIL_GetListenServerHost());
 
    if ( pPlayer->GetCloakStatus() == 0 )
    {
        pPlayer->SetCloakStatus( 3 );
        Msg( "Cloaking...\n" );
    }
    else if ( pPlayer->GetCloakStatus() == 2 )
    {
        pPlayer->SetCloakStatus( 1 );
        Msg( "Uncloaking...\n" );
    }
    else
        return;
}

static ConCommand cloak( "cloak", CC_Cloak );
