#include "..\Public\Bounding_Sphere.h"
#include "DebugDraw.h"

CBounding_Sphere::CBounding_Sphere()
{
}

HRESULT CBounding_Sphere::Initialize(const _float3& vPosition, _float fRadius)
{
	m_pSphere_Original = new BoundingSphere(vPosition, fRadius);
	m_pSphere = new BoundingSphere(*m_pSphere_Original);

	return S_OK;
}

#ifdef _DEBUG

HRESULT CBounding_Sphere::Render(PrimitiveBatch<VertexPositionColor>* pBatch, _fvector vColor)
{
	DX::Draw(pBatch, *m_pSphere, vColor);

	return S_OK;
}

#endif

void CBounding_Sphere::Tick(_fmatrix TransformMatrix)
{
	m_pSphere_Original->Transform(*m_pSphere, TransformMatrix);
}

_bool CBounding_Sphere::Intersect(CCollider::TYPE eType, void* pBoundingDesc)
{
	_bool		isColl = false;

	switch (eType)
	{
	case CCollider::TYPE_AABB:
		isColl = m_pSphere->Intersects(*(BoundingBox*)pBoundingDesc);
		break;
	case CCollider::TYPE_OBB:
		isColl = m_pSphere->Intersects(*(BoundingOrientedBox*)pBoundingDesc);
		break;
	case CCollider::TYPE_SPHERE:
		isColl = m_pSphere->Intersects(*(BoundingSphere*)pBoundingDesc);
		break;
	}

	return isColl;
}

_bool CBounding_Sphere::Intersect_Mouse(_vector vOrigin, _vector vDirection, _float& fDist)
{
	_bool		bColl = false;

	return bColl = m_pSphere->Intersects(vOrigin, vDirection, fDist);
}

CBounding_Sphere * CBounding_Sphere::Create(const _float3& vPosition, _float fRadius)
{
	CBounding_Sphere*	pInstance = new CBounding_Sphere();

	if (FAILED(pInstance->Initialize(vPosition, fRadius)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Created : CBounding_Sphere");
	}

	return pInstance;
}

void CBounding_Sphere::Free()
{
	__super::Free();

	Safe_Delete(m_pSphere_Original);
	Safe_Delete(m_pSphere);
}


