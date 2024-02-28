#include "stdafx.h"
#include "Thresher_SecondAppear.h"

#include "GameInstance.h"
#include "SoundMgr.h"

HRESULT CThresher_SecondAppear::Enter(CThresher* thresher)
{
    m_pThresher = thresher;

    CGameInstance* pGameInstance = CGameInstance::Get_Instance();
    Safe_AddRef(pGameInstance);

    m_pTransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Thresher"), TEXT("Com_Transform")));
    m_pPlayerTransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_Transform")));

   /* random_device		RandomDevice;
    mt19937_64			RandomNumber;
    uniform_real_distribution<float>		RandomX;
    uniform_real_distribution<float>		RandomZ;

    RandomNumber = mt19937_64(RandomDevice());

    RandomX = uniform_real_distribution<float>(70.f, 115.f);
    RandomZ = uniform_real_distribution<float>(10.f, 105.f);*/

    //m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(RandomX(RandomNumber), 0.f, RandomZ(RandomNumber), 1.f));
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(90.f, 0.f, 100.f, 1.f));

    Safe_Release(pGameInstance);

    return S_OK;
}

void CThresher_SecondAppear::Tick(_float fTimeDelta)
{
    CGameInstance* pGameInstance = CGameInstance::Get_Instance();
    Safe_AddRef(pGameInstance);

    m_pModelCom = dynamic_cast<CModel*>(pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Thresher"), TEXT("Com_Model")));
    m_pTransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Thresher"), TEXT("Com_Transform")));
    m_pPlayerTransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_Transform")));
    m_pNavigationCom = dynamic_cast<CNavigation*>(pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Thresher"), TEXT("Com_Navigation")));

    m_pModelCom->Play_Animation(fTimeDelta);

    m_pModelCom->SetUp_Animation(13, true);

    if (m_pModelCom->Get_AnimEnd())
    {
        m_pModelCom->Set_AnimEnd(false);
        m_pModelCom->Stop_Animation(fTimeDelta, true);

        if (m_pModelCom->Get_Animation()[13]->Get_AnimEnd())
        {
            m_pModelCom->Get_Animation()[13]->Set_AnimEnd(false);
            m_pThresher->ChangeState(CThresher::STATE_APPEAR);
        }
    }

    Safe_Release(pGameInstance);
}

void CThresher_SecondAppear::Exit(CThresher* thresher)
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
