#include "..\Public\Bounding_AABB.h"
#include "DebugDraw.h"


CBounding_AABB::CBounding_AABB()
{
}

HRESULT CBounding_AABB::Initialize(const _float3& vPosition, const _float3& vSize)
{
	m_pAABB_Original = new BoundingBox(vPosition, _float3(vSize.x * 0.5f, vSize.y * 0.5f, vSize.z * 0.5f));
	m_pAABB = new BoundingBox(*m_pAABB_Original);

	return S_OK;
}
void CBounding_AABB::Tick(_fmatrix TransformMatrix)
{
	_matrix		Matrix = TransformMatrix;

	Matrix.r[0] = XMVectorSet(1.f, 0.f, 0.f, 0.f) * XMVector3Length(Matrix.r[0]).m128_f32[0];	
	Matrix.r[1] = XMVectorSet(0.f, 1.f, 0.f, 0.f) * XMVector3Length(Matrix.r[1]).m128_f32[0];
	Matrix.r[2] = XMVectorSet(0.f, 0.f, 1.f, 0.f) * XMVector3Length(Matrix.r[2]).m128_f32[0];
		
	m_pAABB_Original->Transform(*m_pAABB, Matrix);
}

_bool CBounding_AABB::Intersect(CCollider::TYPE eType, void* pBoundingDesc)
{
	_bool		isColl = false;

	switch (eType)
	{
	case CCollider::TYPE_AABB:
		isColl = m_pAABB->Intersects(*(BoundingBox*)pBoundingDesc);
		break;
	case CCollider::TYPE_OBB:
		isColl = m_pAABB->Intersects(*(BoundingOrientedBox*)pBoundingDesc);
		break;
	case CCollider::TYPE_SPHERE:
		isColl = m_pAABB->Intersects(*(BoundingSphere*)pBoundingDesc);
		break;
	}

	return isColl;
}

_bool CBounding_AABB::Intersect_Mouse(_vector Origin, _vector Direction, _float& Dist)
{
	_bool		isColl = false;

	return isColl = m_pAABB->Intersects(Origin, Direction, Dist);
}

#ifdef _DEBUG
HRESULT CBounding_AABB::Render(PrimitiveBatch<VertexPositionColor>* pBatch, _fvector vColor)
{
	DX::Draw(pBatch, *m_pAABB, vColor);

	return S_OK;
}
#endif

CBounding_AABB * CBounding_AABB::Create(const _float3& vPosition, const _float3& vSize)
{
	CBounding_AABB*	pInstance = new CBounding_AABB();

	if (FAILED(pInstance->Initialize(vPosition, vSize)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Created : CBounding_AABB");
	}

	return pInstance;
}

void CBounding_AABB::Free()
{
	__super::Free();

	Safe_Delete(m_pAABB_Original);
	Safe_Delete(m_pAABB);
}


