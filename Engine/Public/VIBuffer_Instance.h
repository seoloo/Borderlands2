#pragma once
#include "VIBuffer.h"
BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Instance abstract : public CVIBuffer
{
protected:
	CVIBuffer_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Instance(const CVIBuffer_Instance& rhs);
	virtual ~CVIBuffer_Instance() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual HRESULT Render();

protected:
	ID3D11Buffer* m_pInstanceVB = { nullptr };
	_uint					m_iNumInstance = { 0 };
	_uint					m_iNumIndicesPerInstance = { 0 };
	_uint					m_iInstanceStride = { 0 };

public:
	virtual CComponent* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END

