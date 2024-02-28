#include "..\Public\BlendObject.h"
#include "PipeLine.h"
#include "Transform.h"

CBlendObject::CBlendObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{

}

CBlendObject::CBlendObject(const CBlendObject & rhs)
	: CGameObject(rhs)
{
}

HRESULT CBlendObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBlendObject::Initialize(void * pArg)
{
	return S_OK;
}

void CBlendObject::Tick(_float fTimeDelta)
{
}

void CBlendObject::Late_Tick(_float fTimeDelta)
{
}

HRESULT CBlendObject::Render()
{
	return S_OK;
}

void CBlendObject::Compute_CamDistance(class CTransform * pTransform)
{
	_float4x4		ViewMatrix;

	CPipeLine* pPipeLine = CPipeLine::Get_Instance();
	Safe_AddRef(pPipeLine);

	_vector		vCamPosition = XMLoadFloat4(&pPipeLine->Get_CamPosition());

	m_fCamDistance = XMVector3Length(vCamPosition - pTransform->Get_State(CTransform::STATE_POSITION)).m128_f32[0];

	Safe_Release(pPipeLine);
}



void CBlendObject::Free()
{
	__super::Free();
}
