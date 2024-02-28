#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CShader final : public CComponent
{
private:
	CShader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CShader(const CShader& rhs);
	virtual ~CShader() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElememts, _uint iNumElements);
	virtual HRESULT Initialize(void* pArg) override;

public:
	/* 셰이더 전역변수에 값을 던진다. */
	HRESULT Bind_ShaderResourceView(const char* pConstantName, ID3D11ShaderResourceView* pSRV);
	HRESULT Bind_ShaderResourceViews(const char* pConstantName, ID3D11ShaderResourceView** ppSRVs, _uint iNumTexture);
	HRESULT Bind_Matrix(const char* pConstantName, const _float4x4* pMatrix);
	HRESULT Bind_Matrices(const char* pConstantName, const _float4x4* pMatrixArray, _uint iNumMatrices);
	HRESULT Bind_RawValue(const char* pConstantName, const void* pData, _uint iByteSize);
	HRESULT Begin(_uint iPassIndex);


private:
	ID3DX11Effect*				m_pEffect = { nullptr };
	_uint						m_iNumPasses = { 0 };

	vector<ID3D11InputLayout*>	m_InputLayouts;

public:
	static CShader* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElememts, _uint iNumElements);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END