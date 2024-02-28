#pragma once

#include "Base.h"

BEGIN(Engine)

class CCustomFont final : public CBase
{
private:
	CCustomFont(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CCustomFont() = default;

public:
	HRESULT Initialize(const wstring& strFontFilePath);
	HRESULT Render_Font(const wstring& strText, const _float2 vPos, _fvector vColor, _float fScale, const _float fRotation = 0.f);

private:
	ID3D11Device*					m_pDevice = { nullptr };
	ID3D11DeviceContext*			m_pContext = { nullptr };

	SpriteBatch*					m_pBatch = { nullptr };
	SpriteFont*						m_pFont = { nullptr };

public:
	static CCustomFont* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strFontFilePath);
	virtual void Free() override;
};

END