#include "stdafx.h"
#include "..\Public\Camera_Free.h"

#include "GameInstance.h"

CCamera_Free::CCamera_Free(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCamera(pDevice, pContext)
{
}

CCamera_Free::CCamera_Free(const CCamera_Free& rhs)
	: CCamera(rhs)
{
}

HRESULT CCamera_Free::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCamera_Free::Initialize(void* pArg)
{
	CAMERAFREEDESC* pCameraFreeDesc = (CAMERAFREEDESC*)pArg;

	/* 나에게 필요한 정보를 셋한다. */

	if (FAILED(__super::Initialize(&pCameraFreeDesc->CameraDesc)))
		return E_FAIL;

	return S_OK;
}

void CCamera_Free::Tick(_float fTimeDelta)
{

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (KEYPRESSING('W'))
		m_pTransform->ToolGo_Straight(fTimeDelta);

	if (KEYPRESSING('S'))
		m_pTransform->ToolGo_Backward(fTimeDelta);

	if (KEYPRESSING('D'))
		m_pTransform->ToolGo_Right(fTimeDelta);

	if (KEYPRESSING('A'))
		m_pTransform->ToolGo_Left(fTimeDelta);

	_long	MouseMove = 0;

	if (MouseMove = pGameInstance->Get_DIMouseMove(CInput_Device::DIMM_X))
		m_pTransform->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * MouseMove * 0.3f);

	if (MouseMove = pGameInstance->Get_DIMouseMove(CInput_Device::DIMM_Y))
		m_pTransform->Turn(m_pTransform->Get_State(CTransform::STATE_RIGHT), fTimeDelta * MouseMove * 0.3f);

	Safe_Release(pGameInstance);

	__super::Set_Transform();
}

void CCamera_Free::Late_Tick(_float fTimeDelta)
{
}

CCamera_Free* CCamera_Free::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCamera_Free* pInstance = new CCamera_Free(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Created : CCamera_Free");
	}

	return pInstance;
}

CGameObject* CCamera_Free::Clone(void* pArg)
{
	CCamera_Free* pInstance = new CCamera_Free(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Cloned : CCamera_Free");
	}

	return pInstance;
}

void CCamera_Free::Free()
{
	__super::Free();

}
