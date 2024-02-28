#include "..\Public\Frustum.h"
#include "PipeLine.h"

IMPLEMENT_SINGLETON(CFrustum)

CFrustum::CFrustum()
{

}

HRESULT CFrustum::Initialize()
{
	/* 투영스페이스 상의 절두체 꼭지점. */
	m_vPoints[0] = _float3(-1.f, 1.f, 0.f);
	m_vPoints[1] = _float3(1.f, 1.f, 0.f);
	m_vPoints[2] = _float3(1.f, -1.f, 0.f);
	m_vPoints[3] = _float3(-1.f, -1.f, 0.f);

	m_vPoints[4] = _float3(-1.f, 1.f, 1.f);
	m_vPoints[5] = _float3(1.f, 1.f, 1.f);
	m_vPoints[6] = _float3(1.f, -1.f, 1.f);
	m_vPoints[7] = _float3(-1.f, -1.f, 1.f);

	return S_OK;
}

void CFrustum::Tick()
{
	CPipeLine*		pPipeLine = CPipeLine::Get_Instance();
	Safe_AddRef(pPipeLine);

	_matrix	ProjMatrixInverse = pPipeLine->Get_Transform_Matrix_Inverse(CPipeLine::D3DTS_PROJ);
	_matrix	ViewMatrixInverse = pPipeLine->Get_Transform_Matrix_Inverse(CPipeLine::D3DTS_VIEW);

	_vector			vPoints[8];

	for (size_t i = 0; i < 8; i++)
	{
		vPoints[i] = XMVector3TransformCoord(XMLoadFloat3(&m_vPoints[i]), ProjMatrixInverse);
		XMStoreFloat3(&m_vPoints_InWorld[i], XMVector3TransformCoord(vPoints[i], ViewMatrixInverse));
	}

	Make_Plane(m_vPoints_InWorld, m_vPlanes_InWorld);

	Safe_Release(pPipeLine);

}

void CFrustum::Transform_ToLocalSpace(_fmatrix WorldMatrix)
{
	_float3		vPoints[8];
	_matrix		WorldMatrixInverse;

	WorldMatrixInverse = XMMatrixInverse(nullptr, WorldMatrix);

	for (size_t i = 0; i < 8; i++)
	{
		XMStoreFloat3(&vPoints[i], XMVector3TransformCoord(XMLoadFloat3(&m_vPoints_InWorld[i]),
			WorldMatrixInverse));		
	}

	Make_Plane(vPoints, m_vPlanes_InLocal);
}

_bool CFrustum::isInFrustum_InWorld(_fvector vWorldPos, _float fRange)
{
	for (size_t i = 0; i < 6; i++)
	{
		/* i번째 평면의 바깥에 있다. */
		if (fRange < XMVectorGetX(XMPlaneDotCoord(XMLoadFloat4(&m_vPlanes_InWorld[i]), vWorldPos)))
			return false;
	}

	return true;
}

_bool CFrustum::isInFrustum_InLocal(_fvector vLocalPos, _float fRange)
{
	for (size_t i = 0; i < 6; i++)
	{
		/* i번째 평면의 바깥에 있다. */
		if (fRange < XMVectorGetX(XMPlaneDotCoord(XMLoadFloat4(&m_vPlanes_InLocal[i]), vLocalPos)))
			return false;
	}

	return true;
}


HRESULT CFrustum::Make_Plane(const _float3 * pPoints, _float4 * pPlanes)
{
	XMStoreFloat4(&pPlanes[0], XMPlaneFromPoints(XMLoadFloat3(&pPoints[1]), 
		XMLoadFloat3(&pPoints[5]), 
		XMLoadFloat3(&pPoints[6])));
	XMStoreFloat4(&pPlanes[1], XMPlaneFromPoints(XMLoadFloat3(&pPoints[4]),
		XMLoadFloat3(&pPoints[0]),
		XMLoadFloat3(&pPoints[3])));
	XMStoreFloat4(&pPlanes[2], XMPlaneFromPoints(XMLoadFloat3(&pPoints[4]),
		XMLoadFloat3(&pPoints[5]),
		XMLoadFloat3(&pPoints[1])));
	XMStoreFloat4(&pPlanes[3], XMPlaneFromPoints(XMLoadFloat3(&pPoints[3]),
		XMLoadFloat3(&pPoints[2]),
		XMLoadFloat3(&pPoints[6])));
	XMStoreFloat4(&pPlanes[4], XMPlaneFromPoints(XMLoadFloat3(&pPoints[5]),
		XMLoadFloat3(&pPoints[4]),
		XMLoadFloat3(&pPoints[7])));
	XMStoreFloat4(&pPlanes[5], XMPlaneFromPoints(XMLoadFloat3(&pPoints[0]),
		XMLoadFloat3(&pPoints[1]),
		XMLoadFloat3(&pPoints[2])));

	return S_OK;
}

void CFrustum::Free()
{
	
}
