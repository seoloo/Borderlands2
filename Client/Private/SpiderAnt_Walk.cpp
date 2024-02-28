#include "stdafx.h"
#include "SpiderAnt_State.h"
#include "SpiderAnt.h"
#include "GameInstance.h"

#include "SpiderAnt_Walk.h"
#include "SoundMgr.h"

HRESULT CSpiderAnt_Walk::Enter(CSpiderAnt* spiderant)
{
    CGameInstance* pGameInstance = CGameInstance::Get_Instance();
    Safe_AddRef(pGameInstance);


    Safe_Release(pGameInstance);

    m_pSpiderAnt = spiderant;

    return S_OK;
}

void CSpiderAnt_Walk::Tick(_float fTimeDelta)
{
    CGameInstance* pGameInstance = CGameInstance::Get_Instance();
    Safe_AddRef(pGameInstance);

    m_pModelCom = dynamic_cast<CModel*>(pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_SpiderAnt"), TEXT("Com_Model")));
    m_pTransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_SpiderAnt"), TEXT("Com_Transform")));
    m_pPlayerTransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_Transform")));
    m_pNavigationCom = dynamic_cast<CNavigation*>(pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_SpiderAnt"), TEXT("Com_Navigation")));
    m_pObject = pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_SpiderAnt"));
    m_pObject2 = pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_SpiderAnt"));

    m_pModelCom->Play_Animation(fTimeDelta);
    m_pModelCom->SetUp_Animation(64);

    m_vPlayerPos.x = m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION).m128_f32[0];
    m_vPlayerPos.z = m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION).m128_f32[2];

    m_vPos.x = m_pTransformCom->Get_State(CTransform::STATE_POSITION).m128_f32[0];
    m_vPos.z = m_pTransformCom->Get_State(CTransform::STATE_POSITION).m128_f32[2];

    m_Look = m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE_POSITION);

    m_pTransformCom->LookAt(m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION));
    m_pTransformCom->Go_Look(m_Look, fTimeDelta, m_pNavigationCom);

    m_fSoundTimer += fTimeDelta;
    if (m_fSoundTimer > 0.5f)
    {
        CSoundMgr::Get_Instance()->PlaySoundW(L"Baggi_Land.wav", 1.f);
        m_fSoundTimer = 0.f;
    }
    _int ihp = m_pObject->Get_Hp();
    _bool bAttacked = m_pObject->Get_Attacked();

    _bool bDig = { false };

    if (ihp <= 3000 * 0.6f)
        bDig = true;

    if (bDig)
    {
        m_pSpiderAnt->Change_State(CSpiderAnt::STATE_DIG);
        m_bFinished = true;
    }

    if (!bDig)
    {
        if (dynamic_cast<CSpiderAnt*>(m_pObject2)->Get_Collided())
            m_pSpiderAnt->Change_State(CSpiderAnt::STATE_ATTACK);
    }

    if (bAttacked && !m_bFinished)
    {
        _int iAttack = 0;

        if (m_iCount == 0)
        {
            random_device rd;
            mt19937 gen(rd());
            uniform_int_distribution<int> Attack(1, 100);
            iAttack = Attack(gen);
            m_iCount++;
        }

        if (iAttack > 90)
        {
            m_pObject->Set_Attacked(false);
            m_pSpiderAnt->Change_State(CSpiderAnt::STATE_ATTACKED);
            m_iCount = 0;
        }
        else
        {
            m_pObject->Set_Attacked(false);
            m_iCount = 0;
        }
    }

    Safe_Release(pGameInstance);
}

void CSpiderAnt_Walk::Exit(CSpiderAnt* spiderant)
{
    m_pModelCom = { nullptr };
    m_pPlayerTransformCom = { nullptr };
    m_pTransformCom = { nullptr };
    m_pNavigationCom = { nullptr };
    m_pObject = { nullptr };
    m_pObject2 = { nullptr };

    Safe_Release(m_pModelCom);
    Safe_Release(m_pPlayerTransformCom);
    Safe_Release(m_pTransformCom);
    Safe_Release(m_pNavigationCom);
}
