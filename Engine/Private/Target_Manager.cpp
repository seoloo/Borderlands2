#include "..\Public\Target_Manager.h"
#include "RenderTarget.h"

IMPLEMENT_SINGLETON(CTarget_Manager)

CTarget_Manager::CTarget_Manager()
{

}

HRESULT CTarget_Manager::Add_RenderTarget(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strTargetTag, _uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, const _float4& vClearColor)
{
	if (nullptr != Find_RenderTarget(strTargetTag))
		return E_FAIL;

	CRenderTarget* pRenderTarget = CRenderTarget::Create(pDevice, pContext, iSizeX, iSizeY, ePixelFormat, vClearColor);

	if (nullptr == pRenderTarget)
		return E_FAIL;

	m_RenderTargets.emplace(strTargetTag, pRenderTarget);

	return S_OK;
}

HRESULT CTarget_Manager::Add_MRT(const wstring& strMRTTag, const wstring& strTargetTag)
{
	CRenderTarget* pRenderTarget = Find_RenderTarget(strTargetTag);

	if (nullptr == pRenderTarget)
		return E_FAIL;

	list<CRenderTarget*>* pMRTList = Find_MRT(strMRTTag);

	if (nullptr == pMRTList)
	{
		list<CRenderTarget*>	MRTList;
		MRTList.push_back(pRenderTarget);
		m_MRTs.emplace(strMRTTag, MRTList);
	}
	else
		pMRTList->push_back(pRenderTarget);

	//Safe_AddRef(pRenderTarget);

	return S_OK;
}

HRESULT CTarget_Manager::Begin_MRT(ID3D11DeviceContext* pContext, const wstring& strMRTTag)
{
	ID3D11ShaderResourceView* pSRV[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT] = {
	nullptr
	};

	pContext->PSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, pSRV);

	list<CRenderTarget*>* pMRTList = Find_MRT(strMRTTag);

	pContext->OMGetRenderTargets(1, &m_pBackBufferView, &m_pDepthStencilView);

	ID3D11RenderTargetView* pRenderTargets[8] = { nullptr };

	_uint		iNumRTVs = 0;



	for (auto& pRenderTarget : *pMRTList)
	{
		pRenderTarget->Clear();
		pRenderTargets[iNumRTVs++] = pRenderTarget->Get_RTV();
	}

	pContext->OMSetRenderTargets(iNumRTVs, pRenderTargets, m_pDepthStencilView);

	return S_OK;
}

HRESULT CTarget_Manager::End_MRT(ID3D11DeviceContext* pContext, const wstring& strMRTTag)
{
	ID3D11RenderTargetView* pRenderTargets[8] = { nullptr };

	pContext->OMSetRenderTargets(8, pRenderTargets, nullptr);

	pContext->OMSetRenderTargets(1, &m_pBackBufferView, m_pDepthStencilView);

	Safe_Release(m_pDepthStencilView);
	Safe_Release(m_pBackBufferView);

	return S_OK;
}

HRESULT CTarget_Manager::Bind_ShaderResource(CShader* pShader, const wstring& strTargetTag, const char* pConstantName)
{
	CRenderTarget* pRenderTarget = Find_RenderTarget(strTargetTag);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	return pRenderTarget->Bind_ShaderResource(pShader, pConstantName);


}

#ifdef _DEBUG
HRESULT CTarget_Manager::Ready_Debug(const wstring& strTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY)
{
	CRenderTarget* pRenderTarget = Find_RenderTarget(strTargetTag);

	if (nullptr == pRenderTarget)
		return E_FAIL;

	return pRenderTarget->Ready_Debug(fX, fY, fSizeX, fSizeY);
}
HRESULT CTarget_Manager::Debug_Render(CShader* pShader, CVIBuffer_Rect* pVIBuffer, const wstring& strMRTTag)
{
	list<CRenderTarget*>* pMRTList = Find_MRT(strMRTTag);

	for (auto& pRenderTarget : *pMRTList)
	{
		pRenderTarget->Debug_Render(pShader, pVIBuffer);
	}

	return S_OK;
}
#endif

CRenderTarget* CTarget_Manager::Find_RenderTarget(const wstring& strTargetTag)
{
	auto	iter = m_RenderTargets.find(strTargetTag);

	if (iter == m_RenderTargets.end())
		return nullptr;

	return iter->second;
}

list<class CRenderTarget*>* CTarget_Manager::Find_MRT(const wstring& strMRTTag)
{
	auto	iter = m_MRTs.find(strMRTTag);

	if (iter == m_MRTs.end())
		return nullptr;

	return &iter->second;
}

void CTarget_Manager::Free()
{
	for (auto& Pair : m_MRTs)
	{
		for (auto& pRenderTarget : Pair.second)
		{
			Safe_Release(pRenderTarget);
		}
		Pair.second.clear();
	}

	m_MRTs.clear();

	for (auto& Pair : m_RenderTargets)
		Safe_Release(Pair.second);

	m_RenderTargets.clear();
}
