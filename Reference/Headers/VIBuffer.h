#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer abstract : public CComponent
{
protected:
	CVIBuffer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer(const CVIBuffer& rhs);
	virtual ~CVIBuffer() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual HRESULT Render();

public:
	_bool Intersect_Polygons(const _float4x4& WorldMatrix, _float3* pOut);

protected:
	ID3D11Buffer*				m_pVB = { nullptr };
	ID3D11Buffer*				m_pIB = { nullptr };

protected:
	D3D11_BUFFER_DESC			m_BufferDesc;
	D3D11_SUBRESOURCE_DATA		m_InitializeData;
	_uint						m_iNumVertices = { 0 };
	_uint						m_iStride = { 0 };
	_uint						m_iIndexStride = { 0 };
	_uint						m_iNumIndices = { 0 };
	_uint						m_iNumVertexBuffers = { 0 };
	DXGI_FORMAT					m_eIndexFormat;
	D3D11_PRIMITIVE_TOPOLOGY	m_eTopology;

protected:
	_float3*					m_pVerticesPos = { nullptr };	//피킹용 위치정보와 인덱스 정보
	void*						m_pIndices = { nullptr };		//피킹용 위치정보와 인덱스 정보
	_uint						m_iNumPrimitive = { 0 };

protected:
	HRESULT Create_Buffer(_Inout_ ID3D11Buffer** ppBuffer);
	

public:
	virtual CComponent* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END