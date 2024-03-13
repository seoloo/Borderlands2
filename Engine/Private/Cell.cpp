#include "..\Public\Cell.h"

#include "Navigation.h"

#ifdef _DEBUG
#include "VIBuffer_Cell.h"
#endif


CCell::CCell(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CCell::Initialize(const _float3 * pPoints, _int iIndex)
{
	m_iIndex = iIndex;

	memcpy(m_vPoints, pPoints, sizeof(_float3) * POINT_END);

#ifdef _DEBUG
	m_pVIBuffer = CVIBuffer_Cell::Create(m_pDevice, m_pContext, m_vPoints);
	if (nullptr == m_pVIBuffer)
		return E_FAIL;
#endif

	return S_OK;
}

_bool CCell::Compare_Points(const _float3& vSourPoint, const _float3& vDestPoint)
{
	if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_A]), XMLoadFloat3(&vSourPoint)))
	{
		if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_B]), XMLoadFloat3(&vDestPoint)))
			return true;

		else if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_C]), XMLoadFloat3(&vDestPoint)))
			return true;
	}

	if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_B]), XMLoadFloat3(&vSourPoint)))
	{
		if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_C]), XMLoadFloat3(&vDestPoint)))
			return true;

		else if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_A]), XMLoadFloat3(&vDestPoint)))
			return true;
	}

	if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_C]), XMLoadFloat3(&vSourPoint)))
	{
		if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_A]), XMLoadFloat3(&vDestPoint)))
			return true;

		else if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_B]), XMLoadFloat3(&vDestPoint)))
			return true;
	}
	return false;
}

_bool CCell::is_In(_fvector vPosition, _int* pNeighborIndex)
{
	for (size_t i = 0; i < LINE_END; i++)
	{
		_vector	vSourDir = XMVector3Normalize(vPosition - XMLoadFloat3(&m_vPoints[i]));

		_vector	vTmpDir = XMLoadFloat3(&m_vPoints[(i + 1) % 3]) - XMLoadFloat3(&m_vPoints[i]);

		_vector vDestDir = XMVector3Normalize(XMVectorSet(XMVectorGetZ(vTmpDir) * -1.f, 
			0.f, XMVectorGetX(vTmpDir), 0.f));

		if (0 < XMVectorGetX(XMVector3Dot(vSourDir, vDestDir)))
		{
			m_vFirstPos = m_vPoints[(i + 1) % 3];
			m_vSecondPos = m_vPoints[(i)];
			*pNeighborIndex = m_iNeighborIndices[i];

			return false;
		}
	}

	return true;
}

#ifdef _DEBUG
HRESULT CCell::Render()
{
	if (nullptr == m_pVIBuffer)
		return E_FAIL;

	m_pVIBuffer->Render();

	return S_OK;
}
#endif

CCell * CCell::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _float3 * pPoints, _int iIndex)
{
	CCell*	pInstance = new CCell(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pPoints, iIndex)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Created : CCell");
	}

	return pInstance;
}

void CCell::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pNavigation);

#ifdef _DEBUG
	Safe_Release(m_pVIBuffer);
#endif
}

