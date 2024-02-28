#include "VIBuffer_Instance.h"

CVIBuffer_Instance::CVIBuffer_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CVIBuffer(pDevice, pContext)
{
}

CVIBuffer_Instance::CVIBuffer_Instance(const CVIBuffer_Instance& rhs)
    : CVIBuffer(rhs)
    , m_pInstanceVB(rhs.m_pInstanceVB)
    , m_iNumInstance(rhs.m_iNumInstance)
    , m_iNumIndicesPerInstance(rhs.m_iNumIndicesPerInstance)
    , m_iInstanceStride(rhs.m_iInstanceStride)
{
}

HRESULT CVIBuffer_Instance::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CVIBuffer_Instance::Initialize(void* pArg)
{
    return S_OK;
}

HRESULT CVIBuffer_Instance::Render()
{
	ID3D11Buffer* pVertexBuffers[] = {
		m_pVB,
		m_pInstanceVB
	};

	_uint					iStrides[] = {
		m_iStride,
		m_iInstanceStride
	};

	_uint					iOffsets[] = {
		0,
		0
	};

	m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, iStrides, iOffsets);
	m_pContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);
	m_pContext->IASetPrimitiveTopology(m_eTopology);
	m_pContext->DrawIndexedInstanced(m_iNumIndicesPerInstance, m_iNumInstance, 0, 0, 0);

    return S_OK;
}

void CVIBuffer_Instance::Free()
{
    __super::Free();

    Safe_Release(m_pInstanceVB);
}
