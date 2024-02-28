#include "stdafx.h"
#include "Stalker_Attack1.h"

#include "GameInstance.h"

HRESULT CStalker_Attack1::Enter(CStalker* stalker)
{
    m_pStalker = stalker;

    return S_OK;
}

void CStalker_Attack1::Tick(_float fTimeDelta)
{
    CGameInstance* pGameInstance = CGameInstance::Get_Instance();
    Safe_AddRef(pGameInstance);

    m_pModelCom->Play_Animation(fTimeDelta);

    m_pModelCom->SetUp_Animation(5, true);

    m_pPlayerTransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_Transform")));

    m_vPlayerPos.x = m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION).m128_f32[0];
    m_vPlayerPos.z = m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION).m128_f32[2];

    m_vPos.x = m_pTransformCom->Get_State(CTransform::STATE_POSITION).m128_f32[0];
    m_vPos.z = m_pTransformCom->Get_State(CTransform::STATE_POSITION).m128_f32[2];

    m_pTransformCom->LookAt(m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION));



    Safe_Release(pGameInstance);

}

void CStalker_Attack1::Exit(CStalker* stalker)
{
    Safe_Release(m_pModelCom);
    Safe_Release(m_pTransformCom);
    Safe_Release(m_pPlayerTransformCom);
    Safe_Release(m_pStalker);
}
