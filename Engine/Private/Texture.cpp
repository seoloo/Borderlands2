#include "..\Public\Texture.h"
#include "Shader.h"

CTexture::CTexture(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext)
{
}

CTexture::CTexture(const CTexture & rhs)
	: CComponent(rhs)
	, m_Textures(rhs.m_Textures)
{
	for (auto& pSRV : m_Textures)
		Safe_AddRef(pSRV);
}

HRESULT CTexture::Initialize_Prototype(const _tchar * pTextureFilePath, _uint iNumTextures)
{
	m_Textures.reserve(iNumTextures);

	for (size_t i = 0; i < iNumTextures; i++)
	{
		_tchar		szFullPath[MAX_PATH] = TEXT("");

		wsprintf(szFullPath, pTextureFilePath, i);

		ID3D11ShaderResourceView*		pSRV = nullptr;

		_tchar		szExt[MAX_PATH] = TEXT("");

		_wsplitpath_s(szFullPath, nullptr, 0, nullptr, 0, nullptr, 0, szExt, MAX_PATH);

		if (false == lstrcmp(szExt, TEXT(".dds")))
			DirectX::CreateDDSTextureFromFile(m_pDevice, szFullPath, nullptr, &pSRV);
		else if (false == lstrcmp(szExt, TEXT(".tga")))
			pSRV = nullptr;
		else
			DirectX::CreateWICTextureFromFile(m_pDevice, szFullPath, nullptr, &pSRV);

		if (nullptr == pSRV)
			return E_FAIL;

		m_Textures.push_back(pSRV);

	}

	return S_OK;
}

HRESULT CTexture::Initialize(void * pArg)
{
	return S_OK;
}

HRESULT CTexture::Bind_ShaderResourceView(CShader * pShader, const char * pConstantName, _uint iTextureIndex)
{
	if (iTextureIndex >= m_Textures.size())
		return E_FAIL;

	return pShader->Bind_ShaderResourceView(pConstantName, m_Textures[iTextureIndex]);	
}

HRESULT CTexture::Bind_ShaderResourceViews(CShader * pShader, const char * pConstantName)
{
	return pShader->Bind_ShaderResourceViews(pConstantName, &m_Textures.front(), m_Textures.size());	
}

CTexture * CTexture::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar * pTextureFilePath, _uint iNumTextures)
{
	CTexture*	pInstance = new CTexture(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pTextureFilePath, iNumTextures)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Created : CTexture");
	}

	return pInstance;
}

CComponent * CTexture::Clone(void * pArg)
{
	CTexture*	pInstance = new CTexture(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Cloned : CTexture");
	}

	return pInstance;
}


void CTexture::Free()
{
	__super::Free();

	for (auto& pSRV : m_Textures)
		Safe_Release(pSRV);

	m_Textures.clear();
}
