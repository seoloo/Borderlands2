#include "stdafx.h"
#include "Thresher_Dead.h"

#include "GameInstance.h"
#include "SoundMgr.h"

HRESULT CThresher_Dead::Enter(CThresher* thresher)
{
    m_pThresher = thresher;

    CGameInstance* pGameInstance = CGameInstance::Get_Instance();
    Safe_AddRef(pGameInstance);

    CSoundMgr::Get_Instance()->PlaySoundW(L"c4100_bomb.mp3", 1.f);

    Safe_Release(pGameInstance);

    return S_OK;
}

void CThresher_Dead::Tick(_float fTimeDelta)
{
    CGameInstance* pGameInstance = CGameInstance::Get_Instance();
    Safe_AddRef(pGameInstance);

    m_pModelCom = dynamic_cast<CModel*>(pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Thresher"), TEXT("Com_Model")));
    m_pTransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Thresher"), TEXT("Com_Transform")));
    m_pPlayerTransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_Transform")));
    m_pNavigationCom = dynamic_cast<CNavigation*>(pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Thresher"), TEXT("Com_Navigation")));

    m_pModelCom->Play_Animation(fTimeDelta);

    /*m_pModelCom->Dead_Animation(15, fTimeDelta * 0.005f);

    if (m_pModelCom->Get_Animation()[15]->Get_AnimEnd())
        m_pThresher->Set_DeathAnim(true);*/

    m_pObject = pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Thresher"));

    m_pObject->Set_Hp(0);
    
    m_pModelCom->SetUp_Animation(15, true);

    if (m_pModelCom->Get_AnimEnd())
    {
        m_pModelCom->Set_AnimEnd(false);

        if (m_pModelCom->Get_Animation()[15]->Get_AnimEnd())
        {
            m_pThresher->Set_DeathAnim(true);

        }
    }
    Safe_Release(pGameInstance);
}

void CThresher_Dead::Exit(CThresher* thresher)
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
