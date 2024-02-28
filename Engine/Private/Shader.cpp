#include "..\Public\Shader.h"

CShader::CShader(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext)	
{
}

CShader::CShader(const CShader & rhs)
	: CComponent(rhs)
	, m_pEffect(rhs.m_pEffect)
	, m_iNumPasses(rhs.m_iNumPasses)
	, m_InputLayouts(rhs.m_InputLayouts)
{
	for (auto& pInputLayout : m_InputLayouts)
		Safe_AddRef(pInputLayout);

	Safe_AddRef(m_pEffect);
}

HRESULT CShader::Initialize_Prototype(const _tchar * pShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElememts, _uint iNumElements)
{
	_uint			iHlslFlag = { 0 };

#ifdef _DEBUG
	iHlslFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	iHlslFlag = D3DCOMPILE_OPTIMIZATION_LEVEL1;
#endif

	if (FAILED(D3DX11CompileEffectFromFile(pShaderFilePath, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, iHlslFlag, 0, m_pDevice, &m_pEffect, nullptr)))
		return E_FAIL;

	/* 테크니커의 정보를 얻어온다. : 테크니커를 통해 패스에 접근을 할꺼야. */
	ID3DX11EffectTechnique*		pTechnique = m_pEffect->GetTechniqueByIndex(0);
	if (nullptr == pTechnique)
		return E_FAIL;

	D3DX11_TECHNIQUE_DESC		TechniqueDesc;
	ZeroMemory(&TechniqueDesc, sizeof TechniqueDesc);

	if (FAILED(pTechnique->GetDesc(&TechniqueDesc)))
		return E_FAIL;

	m_InputLayouts.reserve(TechniqueDesc.Passes);

	m_iNumPasses = TechniqueDesc.Passes;

	/* 이 테크니커 안에 정의되어ㅣㅆ는 패스의 갯수 */
	for (size_t i = 0; i < TechniqueDesc.Passes; i++)
	{
		/* i번째 패스객체를 받아온다. */
		ID3DX11EffectPass*		pPass = pTechnique->GetPassByIndex(i);
			

		D3DX11_PASS_DESC		PassDesc;
		ZeroMemory(&PassDesc, sizeof PassDesc);

		if (FAILED(pPass->GetDesc(&PassDesc)))
			return E_FAIL;		

		ID3D11InputLayout*		pInputLayout = { nullptr };

		if (FAILED(m_pDevice->CreateInputLayout(pElememts, iNumElements, PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &pInputLayout)))
			return E_FAIL;

		m_InputLayouts.push_back(pInputLayout);
	}
	
	return S_OK;
}

HRESULT CShader::Initialize(void * pArg)
{
	return S_OK;
}

HRESULT CShader::Bind_ShaderResourceView(const char * pConstantName, ID3D11ShaderResourceView * pSRV)
{
	if (nullptr == m_pEffect)
		return E_FAIL;

	/* 이 셰이더 안에 정의되ㅓ있는 특정 이름을 가진 전역변수의 핸들을 얻어온다. */
	ID3DX11EffectVariable*		pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable)
		return E_FAIL;

	ID3DX11EffectShaderResourceVariable*	pSRVVariable = pVariable->AsShaderResource();
	if (nullptr == pSRVVariable)
		return E_FAIL;

	return pSRVVariable->SetResource(pSRV);	
}

HRESULT CShader::Bind_ShaderResourceViews(const char * pConstantName, ID3D11ShaderResourceView** ppSRVs, _uint iNumTexture)
{

	if (nullptr == m_pEffect)
		return E_FAIL;

	/* 이 셰이더 안에 정의되ㅓ있는 특정 이름을 가진 전역변수의 핸들을 얻어온다. */
	ID3DX11EffectVariable*		pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable)
		return E_FAIL;

	ID3DX11EffectShaderResourceVariable*	pSRVVariable = pVariable->AsShaderResource();
	if (nullptr == pSRVVariable)
		return E_FAIL;

	return pSRVVariable->SetResourceArray(ppSRVs, 0, iNumTexture);	
}

HRESULT CShader::Bind_Matrix(const char* pConstantName, const _float4x4* pMatrix)
{
	if (nullptr == m_pEffect)
		return E_FAIL;

	/* 이 셰이더 안에 정의되ㅓ있는 특정 이름을 가진 전역변수의 핸들을 얻어온다. */

	ID3DX11EffectVariable*		pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable)
		return E_FAIL;

	ID3DX11EffectMatrixVariable*	pMatrixVariable = pVariable->AsMatrix();
	if (nullptr == pMatrixVariable)
		return E_FAIL;

	return pMatrixVariable->SetMatrix((_float*)pMatrix);	
}

HRESULT CShader::Bind_Matrices(const char* pConstantName, const _float4x4* pMatrixArray, _uint iNumMatrices)
{
	if (nullptr == m_pEffect)
		return E_FAIL;

	/* 이 셰이더 안에 정의되ㅓ있는 특정 이름을 가진 전역변수의 핸들을 얻어온다. */

	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable)
		return E_FAIL;

	ID3DX11EffectMatrixVariable* pMatrixVariable = pVariable->AsMatrix();
	if (nullptr == pMatrixVariable)
		return E_FAIL;

	return pMatrixVariable->SetMatrixArray((_float*)pMatrixArray, 0, iNumMatrices);
}

HRESULT CShader::Bind_RawValue(const char* pConstantName, const void* pData, _uint iByteSize)
{
	if (nullptr == m_pEffect)
		return E_FAIL;

	/* 이 셰이더 안에 정의되어있는 특정 이름을 가진 전역변수의 핸들을 얻어온다 */

	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);

	if (nullptr == pVariable)
		return E_FAIL;

	return pVariable->SetRawValue(pData, 0, iByteSize);
}

HRESULT CShader::Begin(_uint iPassIndex)
{
	if (iPassIndex >= m_iNumPasses)
		return E_FAIL;

	ID3DX11EffectTechnique*		pTechnique = m_pEffect->GetTechniqueByIndex(0);
	if (nullptr == pTechnique)
		return E_FAIL;

	if (FAILED(pTechnique->GetPassByIndex(iPassIndex)->Apply(0, m_pContext)))
		return E_FAIL;

	m_pContext->IASetInputLayout(m_InputLayouts[iPassIndex]);

	return S_OK;
}

CShader * CShader::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar * pShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElememts, _uint iNumElements)
{
	CShader*	pInstance = new CShader(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pShaderFilePath, pElememts, iNumElements)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Created : CShader");
	}

	return pInstance;
}

CComponent * CShader::Clone(void * pArg)
{
	CShader*	pInstance = new CShader(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Cloned : CShader");
	}

	return pInstance;
}

void CShader::Free()
{
	__super::Free();

	for (auto& pInputLayout : m_InputLayouts)
		Safe_Release(pInputLayout);

	m_InputLayouts.clear();

	Safe_Release(m_pEffect);
}
