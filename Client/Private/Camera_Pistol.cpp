#include "stdafx.h"
#include "Camera_Pistol.h"

#include "GameInstance.h"

CCamera_Pistol::CCamera_Pistol(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CCamera(pDevice, pContext)
{
}

CCamera_Pistol::CCamera_Pistol(const CCamera_Pistol& rhs)
    : CCamera(rhs)
{
}

HRESULT CCamera_Pistol::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CCamera_Pistol::Initialize(void* pArg)
{
    CAMERAPISTOLDESC* pCameraDesc = (CAMERAPISTOLDESC*)pArg;

    /* 나에게 필요한 정보를 셋한다. */
    if (FAILED(__super::Initialize(&pCameraDesc->CameraDesc)))
        return E_FAIL;

    m_pBoneMatrix = pCameraDesc->pBoneMatrix;
    m_pParentMatrix = pCameraDesc->pParentMatrix;
    m_PivotMatrix = pCameraDesc->PivotMatrix;

    if (FAILED(Add_Components()))
        return E_FAIL;

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 3.f, 0.f, 1.f));

    CGameInstance* pGameInstance = CGameInstance::Get_Instance();
    Safe_AddRef(pGameInstance);

    Safe_Release(pGameInstance);

    return S_OK;
}

void CCamera_Pistol::Tick(_float fTimeDelta)
{
    __super::Tick(fTimeDelta);

    CGameInstance* pGameInstance = CGameInstance::Get_Instance();
    Safe_AddRef(pGameInstance);

    m_pPlayerTransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_Transform")));
    Safe_AddRef(m_pPlayerTransformCom);

    m_PlayerPos = m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION);
    m_PlayerUp = m_pPlayerTransformCom->Get_State(CTransform::STATE_UP);
    m_PlayerLook = m_pPlayerTransformCom->Get_State(CTransform::STATE_LOOK);
    m_PlayerRight = m_pPlayerTransformCom->Get_State(CTransform::STATE_RIGHT);
    
    m_pTransform->Set_State(CTransform::STATE_LOOK, m_PlayerLook);
    m_pTransform->Set_State(CTransform::STATE_UP, m_PlayerUp);
    m_pTransform->Set_State(CTransform::STATE_RIGHT, m_PlayerRight);
    m_pTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_PlayerUp.m128_f32[0] + m_PlayerPos.m128_f32[0], m_PlayerUp.m128_f32[1], m_PlayerUp.m128_f32[2] + m_PlayerPos.m128_f32[2], m_PlayerPos.m128_f32[3]));

    _matrix		BoneMatrix = XMLoadFloat4x4(m_pBoneMatrix);

    BoneMatrix.r[0] = XMVector3Normalize(BoneMatrix.r[0]);
    BoneMatrix.r[1] = XMVector3Normalize(BoneMatrix.r[1]);
    BoneMatrix.r[2] = XMVector3Normalize(BoneMatrix.r[2]);

    XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix_Float4x4() *
        BoneMatrix *
        XMLoadFloat4x4(&m_PivotMatrix) *
        XMLoadFloat4x4(m_pParentMatrix));
    

    Safe_Release(pGameInstance);

    __super::Set_Transform();
}

void CCamera_Pistol::Late_Tick(_float fTimeDelta)
{
    __super::Late_Tick(fTimeDelta);
}

HRESULT CCamera_Pistol::Add_Components()
{
    /* Com_Transform */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
        TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
        return E_FAIL;

    return S_OK;
}

CCamera_Pistol* CCamera_Pistol::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CCamera_Pistol* pInstance = new CCamera_Pistol(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        Safe_Release(pInstance);
        MSG_BOX("Failed to Created : CCamera_Pistol");
    }

    return pInstance;
}

CGameObject* CCamera_Pistol::Clone(void* pArg)
{
    CCamera_Pistol* pInstance = new CCamera_Pistol(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        Safe_Release(pInstance);
        MSG_BOX("Failed to Cloned : CCamera_Pistol");
    }

    return pInstance;
}

void CCamera_Pistol::Free()
{
    __super::Free();

    Safe_Release(m_pPlayerTransformCom);
    Safe_Release(m_pTransformCom);
}
