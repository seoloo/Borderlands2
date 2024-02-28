#pragma once
#include "Base.h"

BEGIN(Engine)
class CFont_Manager final : public CBase
{
	DECLARE_SINGLETON(CFont_Manager)

private:
	CFont_Manager();
	virtual ~CFont_Manager() = default;

public:
	HRESULT Add_Font(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strFontTag, const wstring& strFontFilePath);
	HRESULT Render_Font(const wstring& strFontTag, const wstring& strText, const _float2 vPos, _fvector vColor, _float fScale, const _float fRotation);

private:
	map<const wstring, class CCustomFont*>			m_Fonts;

private:
	class CCustomFont* Find_Font(const wstring& strFontTag);

public:
	virtual void Free() override;

};

END