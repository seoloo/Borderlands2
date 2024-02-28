#pragma once
#include "Base.h"

BEGIN(Engine)
/* 렌더타겟들을 보관한다. */
class CTarget_Manager :
    public CBase
{
	DECLARE_SINGLETON(CTarget_Manager)
private:
	CTarget_Manager();
	virtual ~CTarget_Manager() = default;

public:
	/* 렌더타겟을 생성하여 추가하낟. */
	HRESULT Add_RenderTarget(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strTargetTag, _uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, const _float4& vClearColor);

	/* 위에서 추가해놓은 렌더타겟들을 장치에 동시에 바인딩되어야하는 기준으로 묶어 둔다. */
	HRESULT Add_MRT(const wstring& strMRTTag, const wstring& strTargetTag);

	/*  내가 모아놓은 list<class CRenderTarget*>에 렌더타겟들을 장치에 순서대로 바인딩한다. */
	HRESULT Begin_MRT(ID3D11DeviceContext* pContext, const wstring& strMRTTag);
	HRESULT End_MRT(ID3D11DeviceContext* pContext, const wstring& strMRTTag);

	HRESULT Bind_ShaderResource(class CShader* pShader, const wstring& strTargetTag, const char* pConstantName);

#ifdef _DEBUG
public:
	HRESULT Ready_Debug(const wstring& strTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY);
	HRESULT Debug_Render(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer, const wstring& strMRTTag);
#endif

private:
	/* 장치에 동시에 바인되는 타겟들을 묶기 위함*/
	map<const wstring, list<class CRenderTarget*>>		m_MRTs;
	map<const wstring, class CRenderTarget*>			m_RenderTargets;

private:
	ID3D11RenderTargetView* m_pBackBufferView = { nullptr };
	ID3D11DepthStencilView* m_pDepthStencilView = { nullptr };

private:
	class CRenderTarget* Find_RenderTarget(const wstring& strTargetTag);
	list<class CRenderTarget*>* Find_MRT(const wstring& strMRTTag);

public:
	virtual void Free() override;
};

END