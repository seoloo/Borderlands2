#include "Tool_Camera.h"

#include "GameInstance.h"

CTool_Camera::CTool_Camera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CCamera(pDevice, pContext)
{
}

CTool_Camera::CTool_Camera(const CTool_Camera& rhs)
    : CCamera(rhs)
{
}

HRESULT CTool_Camera::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CTool_Camera::Initialize(void* pArg)
{
    CAMERAFREEDESC* pCameraFreeDesc = (CAMERAFREEDESC*)pArg;

    /* 나에게 필요한 정보를 셋한다. */

    if (FAILED(__super::Initialize(&pCameraFreeDesc->CameraDesc)))
        return E_FAIL;

    return S_OK;
}

void CTool_Camera::Tick(_float fTimeDelta)
{
    CGameInstance* pGameInstance = CGameInstance::Get_Instance();
    Safe_AddRef(pGameInstance);

    if (pGameInstance->Get_DIKeyState(DIK_W) & 0x8000)
    {
        m_pTransform->ToolGo_Straight(fTimeDelta);
    }

    if (pGameInstance->Get_DIKeyState(DIK_S) & 0x8000)
    {
        m_pTransform->ToolGo_Backward(fTimeDelta);
    }

    if (pGameInstance->Get_DIKeyState(DIK_A) & 0x8000)
    {
        m_pTransform->ToolGo_Left(fTimeDelta);
    }

    if (pGameInstance->Get_DIKeyState(DIK_D) & 0x8000)
    {
        m_pTransform->ToolGo_Right(fTimeDelta);
    }

    if (pGameInstance->Get_DIKeyState(DIK_Q) & 0x8000)
    {
        m_pTransform->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * -0.4f);
    }

    if (pGameInstance->Get_DIKeyState(DIK_E) & 0x8000)
    {
        m_pTransform->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * 0.4f);
    }

    if (pGameInstance->Get_DIKeyState(DIK_R) & 0x8000)
    {
        m_pTransform->Turn(m_pTransform->Get_State(CTransform::STATE_RIGHT), fTimeDelta * -0.4f);
    }

    if (pGameInstance->Get_DIKeyState(DIK_F) & 0x8000)
    {
        m_pTransform->Turn(m_pTransform->Get_State(CTransform::STATE_RIGHT), fTimeDelta * 0.4f);
    }

    _long	MouseMove = 0;

    /*if (MouseMove = pGameInstance->Get_DIMouseMove(CInput_Device::DIMM_X))
        m_pTransform->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * MouseMove * 0.1f);*/

    /*if (MouseMove = pGameInstance->Get_DIMouseMove(CInput_Device::DIMM_Y))
        m_pTransform->Turn(m_pTransform->Get_State(CTransform::STATE_RIGHT), fTimeDelta * MouseMove * 0.1f);*/

    Safe_Release(pGameInstance);

    __super::Set_Transform();
}

void CTool_Camera::Late_Tick(_float fTimeDelta)
{
}

CTool_Camera* CTool_Camera::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CTool_Camera* pInstance = new CTool_Camera(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        Safe_Release(pInstance);
        MSG_BOX("Failed to Created : CTool_Camera");
    }

    return pInstance;
}

CGameObject* CTool_Camera::Clone(void* pArg)
{
    CTool_Camera* pInstance = new CTool_Camera(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        Safe_Release(pInstance);
        MSG_BOX("Failed to Cloned : CTool_Camera");
    }

    return pInstance;
}

void CTool_Camera::Free()
{
    __super::Free();
}
