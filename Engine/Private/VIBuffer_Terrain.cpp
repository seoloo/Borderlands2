#include "..\Public\VIBuffer_Terrain.h"
#include "Frustum.h"
#include "QuadTree.h"
#include "PipeLine.h"

CVIBuffer_Terrain::CVIBuffer_Terrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer(pDevice, pContext)
	, m_pFrustum(CFrustum::Get_Instance())
{
	Safe_AddRef(m_pFrustum);
}

CVIBuffer_Terrain::CVIBuffer_Terrain(const CVIBuffer_Terrain& rhs)
	: CVIBuffer(rhs)
	, m_iNumVerticesX(rhs.m_iNumVerticesX)
	, m_iNumVerticesZ(rhs.m_iNumVerticesZ)
	, m_pFrustum(rhs.m_pFrustum)
	, m_pIndice(rhs.m_pIndice)
	, m_pQuadTree(rhs.m_pQuadTree)
{
	Safe_AddRef(m_pQuadTree);
	Safe_AddRef(m_pFrustum);
}

HRESULT CVIBuffer_Terrain::Initialize_Prototype(const _tchar* pHeightMapFilePath)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	HANDLE		hFile = CreateFile(pHeightMapFilePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	_ulong					dwByte = 0;

	BITMAPFILEHEADER		fh;
	BITMAPINFOHEADER		ih;

	ReadFile(hFile, &fh, sizeof fh, &dwByte, nullptr);
	ReadFile(hFile, &ih, sizeof ih, &dwByte, nullptr);

	m_iNumVerticesX = ih.biWidth;
	m_iNumVerticesZ = ih.biHeight;

	m_iNumVertices = m_iNumVerticesX * m_iNumVerticesZ;

	_ulong* pPixel = new _ulong[m_iNumVertices];
	ZeroMemory(pPixel, sizeof(_ulong) * m_iNumVertices);

	ReadFile(hFile, pPixel, sizeof(_ulong) * m_iNumVertices, &dwByte, nullptr);

	CloseHandle(hFile);

	m_iNumVertexBuffers = 1;
	m_iStride = sizeof(VTXNORTEX);
	m_iNumIndices = (m_iNumVerticesX - 1) * (m_iNumVerticesZ - 1) * 2 * 3;
	m_iIndexStride = sizeof(_ulong);
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	m_iNumPrimitive = (m_iNumVerticesX - 1) * (m_iNumVerticesZ - 1) * 2;

#pragma region VERTEX_BUFFER
	VTXNORTEX* pVertices = new VTXNORTEX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXNORTEX) * m_iNumVertices);

	m_pVerticesPos = new _float3[m_iNumVertices];
	ZeroMemory(m_pVerticesPos, sizeof(m_iNumVertices));

	for (size_t i = 0; i < m_iNumVerticesZ; i++)
	{
		for (size_t j = 0; j < m_iNumVerticesX; j++)
		{
			_uint		iIndex = i * m_iNumVerticesX + j;

			m_pVerticesPos[iIndex] = pVertices[iIndex].vPosition = _float3(j, (pPixel[iIndex] & 0x000000ff) / 10.f, i);
			pVertices[iIndex].vNormal = _float3(0.f, 0.f, 0.f);
			pVertices[iIndex].vTexcoord = _float2(j / (m_iNumVerticesX - 1.f), i / (m_iNumVerticesZ - 1.f));
		}
	}

	Safe_Delete_Array(pPixel);

#pragma endregion

#pragma region INDEX_BUFFER

	_ulong* pIndices = new _ulong[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_ulong) * m_iNumIndices); 

	m_pIndice = new _ulong[m_iNumIndices];
	ZeroMemory(m_pIndice, sizeof(_ulong) * m_iNumIndices);
	_uint		iNumIndices = 0;

	for (size_t i = 0; i < m_iNumVerticesZ - 1; i++)
	{
		for (size_t j = 0; j < m_iNumVerticesX - 1; j++)
		{
			_uint		iIndex = i * m_iNumVerticesX + j;

			_uint		iIndices[4] = {
				iIndex + m_iNumVerticesX,
				iIndex + m_iNumVerticesX + 1,
				iIndex + 1,
				iIndex
			};

			_vector		vSour, vDest, vNormal;

			pIndices[iNumIndices++] = iIndices[0];
			pIndices[iNumIndices++] = iIndices[1];
			pIndices[iNumIndices++] = iIndices[2];

			vSour = XMLoadFloat3(&pVertices[iIndices[1]].vPosition) -
				XMLoadFloat3(&pVertices[iIndices[0]].vPosition);

			vDest = XMLoadFloat3(&pVertices[iIndices[2]].vPosition) -
				XMLoadFloat3(&pVertices[iIndices[1]].vPosition);

			vNormal = XMVector3Cross(vSour, vDest);

			XMStoreFloat3(&pVertices[iIndices[0]].vNormal,
				XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[0]].vNormal) + vNormal));
			XMStoreFloat3(&pVertices[iIndices[1]].vNormal,
				XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[1]].vNormal) + vNormal));
			XMStoreFloat3(&pVertices[iIndices[2]].vNormal,
				XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[2]].vNormal) + vNormal));

			pIndices[iNumIndices++] = iIndices[0];
			pIndices[iNumIndices++] = iIndices[2];
			pIndices[iNumIndices++] = iIndices[3];

			vSour = XMLoadFloat3(&pVertices[iIndices[2]].vPosition) -
				XMLoadFloat3(&pVertices[iIndices[0]].vPosition);

			vDest = XMLoadFloat3(&pVertices[iIndices[3]].vPosition) -
				XMLoadFloat3(&pVertices[iIndices[2]].vPosition);

			vNormal = XMVector3Cross(vSour, vDest);

			XMStoreFloat3(&pVertices[iIndices[0]].vNormal,
				XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[0]].vNormal) + vNormal));
			XMStoreFloat3(&pVertices[iIndices[2]].vNormal,
				XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[2]].vNormal) + vNormal));
			XMStoreFloat3(&pVertices[iIndices[3]].vNormal,
				XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[3]].vNormal) + vNormal));
		}
	}

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iNumVertices * m_iStride;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iStride;

	ZeroMemory(&m_InitializeData, sizeof m_InitializeData);
	m_InitializeData.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iNumIndices * m_iIndexStride;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = 0;

	ZeroMemory(&m_InitializeData, sizeof m_InitializeData);
	m_InitializeData.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	m_pIndices = pIndices;

	Safe_Delete_Array(pVertices);

#pragma endregion

	/*m_pQuadTree = CQuadTree::Create(
		m_iNumVerticesX * m_iNumVerticesZ - m_iNumVerticesX,
		m_iNumVerticesX * m_iNumVerticesZ - 1,
		m_iNumVerticesX - 1,
		0);

	m_pQuadTree->SetUp_Neighbors();*/

	return S_OK;
}

HRESULT CVIBuffer_Terrain::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

_float CVIBuffer_Terrain::Compute_Height(_fvector vLocalPos)
{
	/* 실제 객체가 존재하는 사각형 영역에 왼쪽 하단 정점 인덱스를 구한다. */
	_uint		iIndex = (_uint)XMVectorGetZ(vLocalPos) * m_iNumVerticesX + (_uint)XMVectorGetX(vLocalPos);

	_uint		iIndices[4] = {
		iIndex + m_iNumVerticesX,
		iIndex + m_iNumVerticesX + 1,
		iIndex + 1,
		iIndex
	};

	_float		fWidth = XMVectorGetX(vLocalPos) - m_pVerticesPos[iIndices[0]].x;
	_float		fDepth = m_pVerticesPos[iIndices[0]].z - XMVectorGetZ(vLocalPos);

	_vector		vPlane;

	/*D3DXPlaneFromPointNormal();
	D3DXPlaneFromPoints();*/

	/* 오른쪽 위 */
	if (fWidth > fDepth)
	{
		vPlane = XMPlaneFromPoints(XMLoadFloat3(&m_pVerticesPos[iIndices[0]]),
			XMLoadFloat3(&m_pVerticesPos[iIndices[1]]),
			XMLoadFloat3(&m_pVerticesPos[iIndices[2]]));
	}
	else
	{
		vPlane = XMPlaneFromPoints(XMLoadFloat3(&m_pVerticesPos[iIndices[0]]),
			XMLoadFloat3(&m_pVerticesPos[iIndices[2]]),
			XMLoadFloat3(&m_pVerticesPos[iIndices[3]]));
	}

	/*ax + by + cz + d = 0
	y = (-ax - cz - d) / b*/

	float fHeight = (-vPlane.m128_f32[0] * vLocalPos.m128_f32[0] -
		vPlane.m128_f32[2] * vLocalPos.m128_f32[2] - vPlane.m128_f32[3]) / vPlane.m128_f32[1];

	return fHeight;
}

void CVIBuffer_Terrain::Culling(_fmatrix WorldMatrix)
{
	/* 로컬스페이스 상의 평면 여섯개를 구성한다. */
	m_pFrustum->Transform_ToLocalSpace(WorldMatrix);

	_uint		iNumIndices = 0;

	CPipeLine* pPipeLine = CPipeLine::Get_Instance();
	Safe_AddRef(pPipeLine);

	m_pQuadTree->Culling(m_pFrustum, m_pVerticesPos, m_pIndice, &iNumIndices, XMLoadFloat4(&pPipeLine->Get_CamPosition()));

	Safe_Release(pPipeLine);

	D3D11_MAPPED_SUBRESOURCE		MappedSubResource;
	ZeroMemory(&MappedSubResource, sizeof MappedSubResource);

	m_pContext->Map(m_pIB, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedSubResource);

	memcpy(MappedSubResource.pData, m_pIndice, sizeof(_ulong) * iNumIndices);

	m_pContext->Unmap(m_pIB, 0);

	m_iNumIndices = iNumIndices;
}

CVIBuffer_Terrain* CVIBuffer_Terrain::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pHeightMapFilePath)
{
	CVIBuffer_Terrain* pInstance = new CVIBuffer_Terrain(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pHeightMapFilePath)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Created : CVIBuffer_Terrain");
	}

	return pInstance;
}

CComponent* CVIBuffer_Terrain::Clone(void* pArg)
{
	CVIBuffer_Terrain* pInstance = new CVIBuffer_Terrain(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Cloned : CVIBuffer_Terrain");
	}

	return pInstance;
}

void CVIBuffer_Terrain::Free()
{
	__super::Free();

	Safe_Release(m_pQuadTree);
	Safe_Release(m_pFrustum);

	if (false == m_isCloned)
	{
		Safe_Delete_Array(m_pIndice);
	}
}
