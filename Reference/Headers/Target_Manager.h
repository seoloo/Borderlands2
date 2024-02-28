#pragma once
#include "Base.h"

BEGIN(Engine)
/* ����Ÿ�ٵ��� �����Ѵ�. */
class CTarget_Manager :
    public CBase
{
	DECLARE_SINGLETON(CTarget_Manager)
private:
	CTarget_Manager();
	virtual ~CTarget_Manager() = default;

public:
	/* ����Ÿ���� �����Ͽ� �߰��ϳ�. */
	HRESULT Add_RenderTarget(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strTargetTag, _uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, const _float4& vClearColor);

	/* ������ �߰��س��� ����Ÿ�ٵ��� ��ġ�� ���ÿ� ���ε��Ǿ���ϴ� �������� ���� �д�. */
	HRESULT Add_MRT(const wstring& strMRTTag, const wstring& strTargetTag);

	/*  ���� ��Ƴ��� list<class CRenderTarget*>�� ����Ÿ�ٵ��� ��ġ�� ������� ���ε��Ѵ�. */
	HRESULT Begin_MRT(ID3D11DeviceContext* pContext, const wstring& strMRTTag);
	HRESULT End_MRT(ID3D11DeviceContext* pContext, const wstring& strMRTTag);

	HRESULT Bind_ShaderResource(class CShader* pShader, const wstring& strTargetTag, const char* pConstantName);

#ifdef _DEBUG
public:
	HRESULT Ready_Debug(const wstring& strTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY);
	HRESULT Debug_Render(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer, const wstring& strMRTTag);
#endif

private:
	/* ��ġ�� ���ÿ� ���εǴ� Ÿ�ٵ��� ���� ����*/
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