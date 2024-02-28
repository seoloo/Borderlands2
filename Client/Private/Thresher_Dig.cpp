#include "stdafx.h"
#include "Thresher_Dig.h"

#include "GameInstance.h"
#include "SoundMgr.h"

HRESULT CThresher_Dig::Enter(CThresher* thresher)
{
    m_pThresher = thresher;

    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> Sound(1, 2);
    _int iRandom = Sound(gen);

    if(iRandom == 1)
        CSoundMgr::Get_Instance()->PlaySoundW(L"c2320_groundpunch.mp3", 1.f);
    else if(iRandom == 2)
        CSoundMgr::Get_Instance()->PlaySoundW(L"c2320_dust.mp3", 1.f);

    return S_OK;
}

void CThresher_Dig::Tick(_float fTimeDelta)
{
    CGameInstance* pGameInstance = CGameInstance::Get_Instance();
    Safe_AddRef(pGameInstance);

    m_pModelCom = dynamic_cast<CModel*>(pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Thresher"), TEXT("Com_Model")));
    m_pTransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Thresher"), TEXT("Com_Transform")));
    m_pPlayerTransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_Transform")));
    m_pNavigationCom = dynamic_cast<CNavigation*>(pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Thresher"), TEXT("Com_Navigation")));
    m_pTransformCom->LookAt(m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION));
    m_pObject = pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Thresher"));

    m_pModelCom->Play_Animation(fTimeDelta);
    m_pModelCom->SetUp_Animation(11);

    /*if (m_pModelCom->Get_Finished())
    {
        if (m_pModelCom->Get_AnimEnd())
        {
            m_fTimer += fTimeDelta;
            if (m_fTimer >= 1.2f)
            {
                m_fTimer = 0.f;
                m_pThresher->ChangeState(CThresher::STATE_APPEAR);
            }
        }
    }*/

    //if (m_pObject->Get_Hp() <= 5)
    //g_bCamShake = true;

    if (m_pObject->Get_Hp() <= 3000 * 0.2f)
    {
        m_bChange = true;
    }
    if (m_bChange)
    {
        m_pModelCom->Set_AnimEnd(false);

        if (m_pModelCom->Get_Animation()[11]->Get_AnimEnd())
        {
            m_pModelCom->Get_Animation()[11]->Set_AnimEnd(false);
            //g_bCamShake = false;
            m_pThresher->ChangeState(CThresher::STATE_SUMMON);
        }
    }
    else
    {
        m_pModelCom->Set_AnimEnd(false);

        if (m_pModelCom->Get_Animation()[11]->Get_AnimEnd())
        {
            m_pModelCom->Get_Animation()[11]->Set_AnimEnd(false);
           //g_bCamShake = false;

            m_pThresher->ChangeState(CThresher::STATE_APPEAR);
        }
    }

    Safe_Release(pGameInstance);
}

void CThresher_Dig::Exit(CThresher* thresher)
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
