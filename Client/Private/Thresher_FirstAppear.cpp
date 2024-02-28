#include "stdafx.h"
#include "Thresher_FirstAppear.h"

#include "GameInstance.h"
#include "SoundMgr.h"

HRESULT CThresher_FirstAppear::Enter(CThresher* thresher)
{
    m_pThresher = thresher;

    CGameInstance* pGameInstance = CGameInstance::Get_Instance();
    Safe_AddRef(pGameInstance);

    Safe_Release(pGameInstance);

    return S_OK;
}

void CThresher_FirstAppear::Tick(_float fTimeDelta)
{
    CGameInstance* pGameInstance = CGameInstance::Get_Instance();
    Safe_AddRef(pGameInstance);

    m_pModelCom = dynamic_cast<CModel*>(pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Thresher"), TEXT("Com_Model")));
    m_pTransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Thresher"), TEXT("Com_Transform")));
    m_pPlayerTransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_Transform")));
    m_pNavigationCom = dynamic_cast<CNavigation*>(pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Thresher"), TEXT("Com_Navigation")));

    m_pTransformCom->LookAt(m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION));

    m_vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

    //32,33,34
    if (!m_bFirst)
    {
        m_pModelCom->Play_Animation(fTimeDelta * 2.f);

        m_pModelCom->SetUp_Animation(32, true);

        m_vPos.m128_f32[1] += 1.f;

        m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vPos);

        if (m_pModelCom->Get_AnimEnd())
        {
            m_pModelCom->Set_AnimEnd(false);
            if (m_pModelCom->Get_Animation()[32]->Get_AnimEnd())
            {
                m_bFirst = true;
            }
        }
    }
    if (m_bFirst)
    {
        m_pModelCom->Play_Animation(fTimeDelta);

        m_pModelCom->SetUp_Animation(33, true);
        m_vPos.m128_f32[1] -= 1.f;

        if (m_pModelCom->Get_AnimEnd())
        {
            m_pModelCom->Set_AnimEnd(false);

            if (m_pModelCom->Get_Animation()[33]->Get_AnimEnd())
            {
                m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vPos);

                m_pThresher->ChangeState(CThresher::STATE_SECONDAPPEAR);

            }
        }
    }

    Safe_Release(pGameInstance);
}

void CThresher_FirstAppear::Exit(CThresher* thresher)
{
    m_pModelCom = { nullptr };
    m_pPlayerTransformCom = { nullptr };
    m_pTransformCom = { nullptr };
    m_pNavigationCom = { nullptr };

    Safe_Release(m_pModelCom);
    Safe_Release(m_pPlayerTransformCom);
    Safe_Release(m_pTransformCom);
    Safe_Release(m_pNavigationCom);
}
