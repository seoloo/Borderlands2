#include "..\Public\Collider.h"

#include "Bounding_AABB.h"
#include "Bounding_OBB.h"
#include "Bounding_Sphere.h"

#include "PipeLine.h"
#include "Picking.h"

CCollider::CCollider(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext)
{
}

CCollider::CCollider(const CCollider & rhs)
	: CComponent(rhs)
	, m_eType(rhs.m_eType)
#ifdef _DEBUG
	, m_pBatch(rhs.m_pBatch)
	, m_pEffect(rhs.m_pEffect)
	, m_pInputLayout(rhs.m_pInputLayout)
#endif
{
#ifdef _DEBUG
	Safe_AddRef(m_pInputLayout);
#endif
}

HRESULT CCollider::Initialize_Prototype(TYPE eType)
{
	m_eType = eType;

#ifdef _DEBUG
	m_pBatch = new PrimitiveBatch<VertexPositionColor>(m_pContext);

	m_pEffect = new BasicEffect(m_pDevice);

	m_pEffect->SetVertexColorEnabled(true);

	const void* pShaderByteCode = nullptr;
	size_t	ShaderByteCodeLength = 0;

	m_pEffect->GetVertexShaderBytecode(&pShaderByteCode, &ShaderByteCodeLength);

	if (FAILED(m_pDevice->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount,
		pShaderByteCode, ShaderByteCodeLength, &m_pInputLayout)))
		return E_FAIL;
#endif

	return S_OK;
}

HRESULT CCollider::Initialize(void * pArg)
{
	COLLIDERDESC* pColliderDesc = (COLLIDERDESC*)pArg;

	/* 모든 충돌체가 원점에 존재하고 사이즈가 1인상태로 정의된다. 회전 x */
	/* 셋팅된 데이터를 월드스페이스 상의 정보를 가진다. */
	switch (m_eType)
	{
	case TYPE_AABB:
		m_pBounding = CBounding_AABB::Create(pColliderDesc->vPosition, pColliderDesc->vSize);
		break;
	case TYPE_OBB:
		m_pBounding = CBounding_OBB::Create(pColliderDesc->vPosition, pColliderDesc->vSize, pColliderDesc->vRotation);
		break;
	case TYPE_SPHERE:
		m_pBounding = CBounding_Sphere::Create(pColliderDesc->vPosition, pColliderDesc->fRadius);
		break;
	}

	if (nullptr == m_pBounding)
		return E_FAIL;
	return S_OK;
}

void CCollider::Tick(_fmatrix WorldMatrix)
{
	m_pBounding->Tick(WorldMatrix);
}

_bool CCollider::Collision(CCollider* pTargetCollider)
{
	m_isCollision = false;

	if (nullptr == pTargetCollider)
		return m_isCollision;

	return m_isCollision = m_pBounding->Intersect(pTargetCollider->m_eType, pTargetCollider->m_pBounding->Get_BoundingDesc());
}

_bool CCollider::Collision_Mouse()
{
	_bool	bCollision = false;

	CPicking* pPicking = CPicking::Get_Instance();

	//pPicking->Transform_ToLocalSpace(WorldMatrix);

	if(pPicking->Intersect_Collider(m_pBounding))
		return bCollision = true;

	return bCollision;
}

#ifdef _DEBUG
HRESULT CCollider::Render()
{
	/*m_pContext->IASetInputLayout(m_pInputLayout);

	m_pEffect->SetWorld(XMMatrixIdentity());

	CPipeLine* pPipeLine = CPipeLine::Get_Instance();
	Safe_AddRef(pPipeLine);

	m_pEffect->SetView(pPipeLine->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
	m_pEffect->SetProjection(pPipeLine->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));

	Safe_Release(pPipeLine);

	m_pEffect->Apply(m_pContext);

	m_pBatch->Begin();

	_vector		vColor = m_isCollision == false ? XMVectorSet(0.f, 1.f, 0.f, 1.f) : XMVectorSet(1.f, 0.f, 0.f, 1.f);

	m_pBounding->Render(m_pBatch, vColor);

	m_pBatch->End();*/
	return S_OK;
}
#endif

CCollider * CCollider::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, TYPE eType)
{
	CCollider*	pInstance = new CCollider(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eType)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Created : CCollider");
	}

	return pInstance;
}

CComponent * CCollider::Clone(void * pArg)
{
	CCollider*	pInstance = new CCollider(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Cloned : CCollider");
	}

	return pInstance;
}

void CCollider::Free()
{
	__super::Free();

	Safe_Release(m_pBounding);

#ifdef _DEBUG
	if (false == m_isCloned)
	{
		Safe_Delete(m_pBatch);
		Safe_Delete(m_pEffect);
	}

	Safe_Release(m_pInputLayout);
#endif
}
