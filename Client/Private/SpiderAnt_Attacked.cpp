#include "stdafx.h"
#include "SpiderAnt_State.h"
#include "SpiderAnt.h"
#include "GameInstance.h"

#include "SpiderAnt_Attacked.h"
#include "SoundMgr.h"

HRESULT CSpiderAnt_Attacked::Enter(CSpiderAnt* spiderant)
{
    CGameInstance* pGameInstance = CGameInstance::Get_Instance();
    Safe_AddRef(pGameInstance);

    m_pObject = pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_SpiderAnt"));

    Safe_Release(pGameInstance);

    m_pSpiderAnt = spiderant;

    return S_OK;
}

void CSpiderAnt_Attacked::Tick(_float fTimeDelta)
{
    CGameInstance* pGameInstance = CGameInstance::Get_Instance();
    Safe_AddRef(pGameInstance);

    m_pModelCom = dynamic_cast<CModel*>(pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_SpiderAnt"), TEXT("Com_Model")));
    m_pObject = pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_SpiderAnt"));

    m_pModelCom->Play_Animation(fTimeDelta * 1.3f);

    m_pModelCom->SetUp_Animation(35);

    if (m_pModelCom->Get_Animation()[35]->Get_AnimEnd())
    {
        m_pModelCom->Get_Animation()[35]->Set_AnimEnd(false);
        m_pSpiderAnt->Change_State(CSpiderAnt::STATE_WALK);
    }

    Safe_Release(pGameInstance);
}

void CSpiderAnt_Attacked::Exit(CSpiderAnt* spiderant)
{
    m_pModelCom = { nullptr };
    m_pPlayerTransformCom = { nullptr };
    m_pTransformCom = { nullptr };
    m_pNavigationCom = { nullptr };
    m_pObject = { nullptr };

    Safe_Release(m_pModelCom);
    Safe_Release(m_pPlayerTransformCom);
    Safe_Release(m_pTransformCom);
    Safe_Release(m_pNavigationCom);
}
