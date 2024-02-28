#include "EffectTool_Camera.h"
#include "GameInstance.h"

CEffectTool_Camera::CEffectTool_Camera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CCamera(pDevice, pContext)
{
}

CEffectTool_Camera::CEffectTool_Camera(const CEffectTool_Camera& rhs)
    : CCamera(rhs)
{
}

HRESULT CEffectTool_Camera::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CEffectTool_Camera::Initialize(void* pArg)
{
    CAMERAFREEDESC* pCameraFreeDesc = (CAMERAFREEDESC*)pArg;

    /* 나에게 필요한 정보를 셋한다. */

    if (FAILED(__super::Initialize(&pCameraFreeDesc->CameraDesc)))
        return E_FAIL;

    return S_OK;
}

void CEffectTool_Camera::Tick(_float fTimeDelta)
{
    CGameInstance* pGameInstance = CGameInstance::Get_Instance();
    Safe_AddRef(pGameInstance);

    if (KEYPRESSING('W'))
        m_pTransform->ToolGo_Straight(fTimeDelta);

    if (KEYPRESSING('S'))
        m_pTransform->ToolGo_Backward(fTimeDelta);

    if (KEYPRESSING('A'))
        m_pTransform->ToolGo_Left(fTimeDelta);

    if (KEYPRESSING('D'))
        m_pTransform->ToolGo_Right(fTimeDelta);

    if (KEYPRESSING('Q'))
        m_pTransform->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * -0.4f);

    if (KEYPRESSING('E'))
        m_pTransform->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * 0.4f);
    
    if (KEYPRESSING('R'))
        m_pTransform->Turn(m_pTransform->Get_State(CTransform::STATE_RIGHT), fTimeDelta * -0.4f);

    if (KEYPRESSING('F'))
        m_pTransform->Turn(m_pTransform->Get_State(CTransform::STATE_RIGHT), fTimeDelta * 0.4f);

    Safe_Release(pGameInstance);

    __super::Set_Transform();
}

void CEffectTool_Camera::Late_Tick(_float fTimeDelta)
{
}

CEffectTool_Camera* CEffectTool_Camera::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CEffectTool_Camera* pInstance = new CEffectTool_Camera(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        Safe_Release(pInstance);
        MSG_BOX("Failed to Created : CEffectTool_Camera");
    }

    return pInstance;
}

CGameObject* CEffectTool_Camera::Clone(void* pArg)
{
    CEffectTool_Camera* pInstance = new CEffectTool_Camera(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        Safe_Release(pInstance);
        MSG_BOX("Failed to Cloned : CEffectTool_Camera");
    }

    return pInstance;
}

void CEffectTool_Camera::Free()
{
    __super::Free();
}
