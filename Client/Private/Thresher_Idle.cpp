#include "stdafx.h"
#include "Thresher_Idle.h"

#include "GameInstance.h"
#include "SoundMgr.h"

HRESULT CThresher_Idle::Enter(CThresher* thresher)
{
    m_pThresher = thresher;

    CGameInstance* pGameInstance = CGameInstance::Get_Instance();
    Safe_AddRef(pGameInstance);

    CSoundMgr::Get_Instance()->PlaySoundW(L"kerberos_attack_icebreath_04_voice.ogg", 2.f);


    Safe_Release(pGameInstance);

    return S_OK;
}

void CThresher_Idle::Tick(_float fTimeDelta)
{
    CGameInstance* pGameInstance = CGameInstance::Get_Instance();
    Safe_AddRef(pGameInstance);
    m_pPlayerTransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_Transform")));
    m_pTransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Thresher"), TEXT("Com_Transform")));
    m_pModelCom = dynamic_cast<CModel*>(pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Thresher"), TEXT("Com_Model")));

    m_pTransformCom->LookAt(m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION));

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

    g_bThresherScene = true;

    m_pModelCom->Play_Animation(fTimeDelta);

    m_pModelCom->SetUp_Animation(12);


    if (m_pModelCom->Get_Animation()[12]->Get_AnimEnd())
    {
        m_pModelCom->Get_Animation()[12]->Set_AnimEnd(false);
        g_bThresherScene = false;
        m_pThresher->ChangeState(CThresher::STATE_DIG);
    }

    Safe_Release(pGameInstance);
}

void CThresher_Idle::Exit(CThresher* thresher)
{
    m_pPlayerTransformCom = { nullptr };
    Safe_Release(m_pModelCom);
    Safe_Release(m_pTransformCom);
}
