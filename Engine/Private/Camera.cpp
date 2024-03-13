#include "Camera.h"
#include "Component_Manager.h"

#include "PipeLine.h"

CCamera::CCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject(pDevice, pContext)
    , m_pPipeLine(CPipeLine::Get_Instance())
{
    Safe_AddRef(m_pPipeLine);
}

CCamera::CCamera(const CCamera& rhs)
    : CGameObject(rhs)
    , m_pPipeLine(rhs.m_pPipeLine)
{
    Safe_AddRef(m_pPipeLine);
}

HRESULT CCamera::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CCamera::Initialize(void* pArg)
{
	CAMERADESC* pCameraDesc = (CAMERADESC*)pArg;

	CComponent_Manager* pComponent_Manager = CComponent_Manager::Get_Instance();
	Safe_AddRef(pComponent_Manager);

	Safe_AddRef(m_pTransform);

	m_pTransform = (CTransform*)pComponent_Manager->Clone_Component(
		pCameraDesc->iLevelIndex, pCameraDesc->strTranformTag, &pCameraDesc->TransformDesc);

	if (nullptr == m_pTransform)
		return E_FAIL;

	Safe_Release(pComponent_Manager);

	m_pTransform->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&pCameraDesc->vEye));
	m_pTransform->LookAt(XMLoadFloat4(&pCameraDesc->vAt));

	m_fFovy = m_CameraDesc.fFovy = pCameraDesc->fFovy;
	m_fAspect = m_CameraDesc.fAspect = pCameraDesc->fAspect;
	m_fNear = m_CameraDesc.fNear = pCameraDesc->fNear;
	m_fFar = m_CameraDesc.fFar = pCameraDesc->fFar;

	return	S_OK;
}

void CCamera::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	m_fFovy = m_CameraDesc.fFovy;
	m_fAspect = m_CameraDesc.fAspect;
	m_fNear = m_CameraDesc.fNear;
	m_fFar = m_CameraDesc.fFar;
}

void CCamera::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CCamera::Set_Transform()
{
	if (nullptr == m_pPipeLine)
		return E_FAIL;

	m_pPipeLine->Set_Transform(CPipeLine::D3DTS_VIEW,
		m_pTransform->Get_WorldMatrix_Inverse());

	m_pPipeLine->Set_Transform(CPipeLine::D3DTS_PROJ,
		XMMatrixPerspectiveFovLH(m_fFovy, m_fAspect, m_fNear, m_fFar));

	return S_OK;
}

void CCamera::Free()
{
	__super::Free();

	Safe_Release(m_pPipeLine);
	Safe_Release(m_pTransform);
}
