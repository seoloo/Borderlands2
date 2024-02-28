#include "..\Public\CustomFont.h"

CCustomFont::CCustomFont(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CCustomFont::Initialize(const wstring& strFontFilePath)
{
	m_pBatch = new SpriteBatch(m_pContext);

	m_pFont = new SpriteFont(m_pDevice, strFontFilePath.c_str());

	return S_OK;
}

HRESULT CCustomFont::Render_Font(const wstring& strText, const _float2 vPos, _fvector vColor, _float fScale, const _float fRotation)
{
	m_pBatch->Begin();

	m_pFont->DrawString(m_pBatch, strText.c_str(), vPos, vColor, fRotation, _float2(0.0f, 0.f), fScale);

	m_pBatch->End();

	return S_OK;
}

CCustomFont * CCustomFont::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const wstring & strFontFilePath)
{
	CCustomFont*	pInstance = new CCustomFont(pDevice, pContext);

	if (FAILED(pInstance->Initialize(strFontFilePath)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Created : CCustomFont");
	}

	return pInstance;
}

void CCustomFont::Free()
{
	Safe_Delete(m_pBatch);
	Safe_Delete(m_pFont);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
