#include "VIBuffer_Cell.h"

CVIBuffer_Cell::CVIBuffer_Cell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CVIBuffer_Cell::CVIBuffer_Cell(const CVIBuffer_Cell& rhs)
	: CVIBuffer(rhs)
{
}

HRESULT CVIBuffer_Cell::Initialize_Prototype(const _float3* pPoints)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_iNumVertexBuffers = 1;
	m_iNumVertices = 3;
	m_iStride = sizeof(VTXPOS);
	m_iNumIndices = 4;
	m_iIndexStride = sizeof(_ushort);
	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;

#pragma region VERTEX_BUFFER

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iNumVertices * m_iStride;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iStride;

	VTXPOS* pVertices = new VTXPOS[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXPOS) * m_iNumVertices);

	memcpy(pVertices, pPoints, sizeof(_float3) * 3);

	ZeroMemory(&m_InitializeData, sizeof m_InitializeData);
	m_InitializeData.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);
#pragma endregion

#pragma region INDEX_BUFFER
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iNumIndices * m_iIndexStride;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = 0;

	_ushort* pIndices = new _ushort[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_ushort) * m_iNumIndices);

	pIndices[0] = 0;
	pIndices[1] = 1;
	pIndices[2] = 2;
	pIndices[3] = 0;

	ZeroMemory(&m_InitializeData, sizeof m_InitializeData);
	m_InitializeData.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

#pragma endregion
	return S_OK;
}

HRESULT CVIBuffer_Cell::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

CVIBuffer_Cell* CVIBuffer_Cell::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* pPoints)
{
	CVIBuffer_Cell* pInstance = new CVIBuffer_Cell(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pPoints)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Created : CVIBuffer_Rect");
	}

	return pInstance;
}

CComponent* CVIBuffer_Cell::Clone(void* pArg)
{
	CVIBuffer_Cell* pInstance = new CVIBuffer_Cell(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Cloned : CVIBuffer_Rect");
	}

	return pInstance;
}

void CVIBuffer_Cell::Free()
{
	__super::Free();

}
