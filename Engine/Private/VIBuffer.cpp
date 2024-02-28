#include "..\Public\VIBuffer.h"
#include "Picking.h"

CVIBuffer::CVIBuffer(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext)
{
}

CVIBuffer::CVIBuffer(const CVIBuffer& rhs)
	: CComponent(rhs)
	, m_pVB(rhs.m_pVB)
	, m_pIB(rhs.m_pIB)
	, m_iNumVertices(rhs.m_iNumVertices)
	, m_iStride(rhs.m_iStride)
	, m_iIndexStride(rhs.m_iIndexStride)
	, m_iNumIndices(rhs.m_iNumIndices)
	, m_iNumVertexBuffers(rhs.m_iNumVertexBuffers)
	, m_eIndexFormat(rhs.m_eIndexFormat)
	, m_eTopology(rhs.m_eTopology)
	, m_pVerticesPos(rhs.m_pVerticesPos)
	, m_pIndices(rhs.m_pIndices)
	, m_iNumPrimitive(rhs.m_iNumPrimitive)
	
{
	Safe_AddRef(m_pVB);
	Safe_AddRef(m_pIB);

}

HRESULT CVIBuffer::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CVIBuffer::Initialize(void * pArg)
{
	return S_OK;
}

HRESULT CVIBuffer::Render()
{
	ID3D11Buffer*			pVertexBuffers[] = {
		m_pVB, 		
	};

	_uint					iStrides[] = {
		m_iStride, 
	};

	_uint					iOffsets[] = {
		0,
	};

	m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, iStrides, iOffsets);
	m_pContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);
	m_pContext->IASetPrimitiveTopology(m_eTopology);
	m_pContext->DrawIndexed(m_iNumIndices, 0, 0);
	
	return S_OK;
}

_bool CVIBuffer::Intersect_Polygons(const _float4x4& WorldMatrix, _float3* pOut)
{
	CPicking* pPicking = CPicking::Get_Instance();

	/* Ray, RayPos 로컬로 이동시킨다. */
	pPicking->Transform_ToLocalSpace(WorldMatrix);

	/* 삼각형 갯수만큼 루프돌면서 각 삼각형과 피킹이 들고 있는 마우스레이의 충돌을 구한다. */
	for (size_t i = 0; i < m_iNumPrimitive; i++)
	{
		/* 삼각형을 구성하는 정점의 인덱스 세개. */
		void* iIndices = { nullptr };

		if (2 == m_iIndexStride)
		{
			iIndices = new _ushort[3];
			memcpy(iIndices, (_ubyte*)m_pIndices + (i * m_iIndexStride * 3), m_iIndexStride * 3);

			_vector vPosition0 = XMLoadFloat3(&m_pVerticesPos[((_ushort*)(iIndices))[0]]);
			_vector vPosition1 = XMLoadFloat3(&m_pVerticesPos[((_ushort*)(iIndices))[1]]);
			_vector vPosition2 = XMLoadFloat3(&m_pVerticesPos[((_ushort*)(iIndices))[2]]);

			if (true == pPicking->Intersect_Triangle_InLocal(vPosition0, vPosition1, vPosition2, pOut))
				return true;
		}
		else
		{
			iIndices = new _ulong[3];
			memcpy(iIndices, (_ubyte*)m_pIndices + (i * m_iIndexStride * 3), m_iIndexStride * 3);

			_vector vPosition0 = XMVectorSet(m_pVerticesPos[((_ulong*)(iIndices))[0]].x, m_pVerticesPos[((_ulong*)(iIndices))[0]].y, m_pVerticesPos[((_ulong*)(iIndices))[0]].z ,1.f);
			_vector vPosition1 = XMVectorSet(m_pVerticesPos[((_ulong*)(iIndices))[1]].x, m_pVerticesPos[((_ulong*)(iIndices))[1]].y, m_pVerticesPos[((_ulong*)(iIndices))[1]].z, 1.f);
			_vector vPosition2 = XMVectorSet(m_pVerticesPos[((_ulong*)(iIndices))[2]].x, m_pVerticesPos[((_ulong*)(iIndices))[2]].y, m_pVerticesPos[((_ulong*)(iIndices))[2]].z, 1.f);

			/*_vector vPosition0 = XMLoadFloat3(&m_pVerticesPos[((_ulong*)(iIndices))[0]]);
			_vector vPosition1 = XMLoadFloat3(&m_pVerticesPos[((_ulong*)(iIndices))[1]]);
			_vector vPosition2 = XMLoadFloat3(&m_pVerticesPos[((_ulong*)(iIndices))[2]]);*/

			if (true == pPicking->Intersect_Triangle_InLocal(vPosition0, vPosition1, vPosition2, pOut))
				return true;
		}
	}

	return false;
}

HRESULT CVIBuffer::Create_Buffer(ID3D11Buffer ** ppBuffer)
{
	if (nullptr == m_pDevice)
		return E_FAIL;	

	return m_pDevice->CreateBuffer(&m_BufferDesc, &m_InitializeData, ppBuffer);	
}

void CVIBuffer::Free()
{
	__super::Free();

	if (!m_isCloned)
	{
		Safe_Delete_Array(m_pIndices);
		Safe_Delete_Array(m_pVerticesPos);
	}

	Safe_Release(m_pIB);
	Safe_Release(m_pVB);

}
