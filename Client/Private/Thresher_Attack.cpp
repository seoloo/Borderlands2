#include "stdafx.h"
#include "Thresher_Attack.h"

#include "GameInstance.h"
#include "SoundMgr.h"

HRESULT CThresher_Attack::Enter(CThresher* thresher)
{
    m_pThresher = thresher;

    CGameInstance* pGameInstance = CGameInstance::Get_Instance();
    Safe_AddRef(pGameInstance);

    random_device rd;  
    mt19937 gen(rd()); 
    uniform_int_distribution<int> dist(3, 9); 


    m_iRandom = dist(gen);

    m_pObject = pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Thresher"));

    m_iPreviousHp = m_pObject->Get_Hp();

    g_bCamShake = false;

    Safe_Release(pGameInstance);

    return S_OK;
}

void CThresher_Attack::Tick(_float fTimeDelta)
{
    CGameInstance* pGameInstance = CGameInstance::Get_Instance();
    Safe_AddRef(pGameInstance);

    m_pModelCom = dynamic_cast<CModel*>(pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Thresher"), TEXT("Com_Model")));
    m_pTransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Thresher"), TEXT("Com_Transform")));
    m_pPlayerTransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_Transform")));
    m_pNavigationCom = dynamic_cast<CNavigation*>(pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Thresher"), TEXT("Com_Navigation")));

    m_pTransformCom->LookAt(m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION));

    m_pModelCom->Play_Animation(fTimeDelta);

    m_pModelCom->SetUp_Animation(m_iRandom, true);

    m_fTimer += fTimeDelta;
    if (m_fTimer > 0.5f)
    {
        CSoundMgr::Get_Instance()->PlaySoundW(L"BOSS_Fire_FireBall.ogg", 1.f);
        m_fTimer = 0.f;
    }

    m_iCurrentHp = m_pObject->Get_Hp();

    if (m_pModelCom->Get_AnimEnd())
    {
        m_pModelCom->Set_AnimEnd(false);

        if (m_pModelCom->Get_Animation()[m_iRandom]->Get_AnimEnd())
        {
            if (m_iCurrentHp < m_iPreviousHp - 180)
            {
                m_pModelCom->Stop_Animation(fTimeDelta, true);
                m_pThresher->ChangeState(CThresher::STATE_DIG);
            }
        }
    }

    Safe_Release(pGameInstance);
}

void CThresher_Attack::Exit(CThresher* thresher)
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
