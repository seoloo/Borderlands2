#include "..\Public\Bounding_OBB.h"
#include "DebugDraw.h"

CBounding_OBB::CBounding_OBB()
{
}

HRESULT CBounding_OBB::Initialize(const _float3& vPosition, const _float3& vSize, const _float3& vRotation)
{
	_float4		vQuaternion;

	XMStoreFloat4(&vQuaternion, XMQuaternionRotationRollPitchYaw(vRotation.x, vRotation.y, vRotation.z));

	m_pOBB_Original = new BoundingOrientedBox(vPosition, _float3(vSize.x * 0.5f, vSize.y * 0.5f, vSize.z * 0.5f), vQuaternion);
	m_pOBB = new BoundingOrientedBox(*m_pOBB_Original);

	return S_OK;
}

void CBounding_OBB::Tick(_fmatrix TransformMatrix)
{
	m_pOBB_Original->Transform(*m_pOBB, TransformMatrix);
}

_bool CBounding_OBB::Intersect(CCollider::TYPE eType, void* pBoundingDesc)
{
	_bool		isColl = false;

	switch (eType)
	{
	case CCollider::TYPE_AABB:
		isColl = m_pOBB->Intersects(*(BoundingBox*)pBoundingDesc);
		break;
	case CCollider::TYPE_OBB:
		isColl = m_pOBB->Intersects(*(BoundingOrientedBox*)pBoundingDesc);
		break;
	case CCollider::TYPE_SPHERE:
		isColl = m_pOBB->Intersects(*(BoundingSphere*)pBoundingDesc);
		break;
	}

	return isColl;
}

_bool CBounding_OBB::Intersect_Mouse(_vector Origin, _vector Direction, _float& Dist)
{
	_bool		isColl = false;

	return isColl = m_pOBB->Intersects(Origin, Direction, Dist);
}

#ifdef _DEBUG
HRESULT CBounding_OBB::Render(PrimitiveBatch<VertexPositionColor>* pBatch, _fvector vColor)
{
	DX::Draw(pBatch, *m_pOBB, vColor);

	return S_OK;
}
#endif

CBounding_OBB * CBounding_OBB::Create(const _float3& vPosition, const _float3& vSize, const _float3& vRotation)
{
	CBounding_OBB*	pInstance = new CBounding_OBB();

	if (FAILED(pInstance->Initialize(vPosition, vSize, vRotation)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Created : CBounding_OBB");
	}

	return pInstance;
}

void CBounding_OBB::Free()
{
	__super::Free();

	Safe_Delete(m_pOBB_Original);
	Safe_Delete(m_pOBB);
}


