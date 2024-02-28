#include "Renderer.h"
#include "GameObject.h"
#include "BlendObject.h"
#include "Target_Manager.h"
#include "VIBuffer_Rect.h"
#include "Shader.h"
#include "Light_Manager.h"
#include "PipeLine.h"

CRenderer::CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
	, m_pTarget_Manager(CTarget_Manager::Get_Instance())
	, m_pLight_Manager(CLight_Manager::Get_Instance())
{
	/*Safe_AddRef(m_pLight_Manager);
	Safe_AddRef(m_pTarget_Manager);*/
	
}

HRESULT CRenderer::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	_uint		iNumViews = 1;
	D3D11_VIEWPORT			ViewPortDesc;

	m_pContext->RSGetViewports(&iNumViews, &ViewPortDesc);

	/* Target_Diffuse */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Diffuse"), ViewPortDesc.Width, ViewPortDesc.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(1.f, 1.f, 1.f, 0.f))))
		return E_FAIL;

	/* Target_Normal */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Normal"), ViewPortDesc.Width, ViewPortDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;

	/* Target_Depth */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Depth"), ViewPortDesc.Width, ViewPortDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 1.f, 0.f, 1.f))))
		return E_FAIL;

	/* Target_Shade */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Shade"), ViewPortDesc.Width, ViewPortDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;

	/* Target_Specular */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Specular"), ViewPortDesc.Width, ViewPortDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.0f, 0.0f, 0.0f, 0.0f))))
		return E_FAIL;


	_uint		iShadowMapCX = 800;
	_uint		iShadowMapCY = 600;

	D3D11_TEXTURE2D_DESC	descDepth;
	ZeroMemory(&descDepth, sizeof(descDepth));
	descDepth.Width = iShadowMapCX;
	descDepth.Height = iShadowMapCY;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDepth.SampleDesc.Count = 0;
	descDepth.SampleDesc.Quality = 1;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;

	ID3D11Texture2D* pDepthStencilBuffer = nullptr;
	ID3D11DepthStencilView* pDepthStencilView = nullptr;
	ID3D11ShaderResourceView* pSRV = { nullptr };

	/*if (FAILED(m_pDevice->CreateTexture2D(&descDepth, nullptr, &pDepthStencilBuffer)))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateShaderResourceView(&pDepthStencilBuffer, nullptr, &pSRV)))
		return E_FAIL;*/

	// For.Target_ShadowDepth
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, L"Target_ShadowDepth", iShadowMapCX, iShadowMapCY, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;

	/*if (FAILED(m_pDevice->CreateDepthStencilView(pDepthStencilBuffer, nullptr, &pDepthStencilView)))
		return E_FAIL;*/



	/* MRT_GameObjects */
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Diffuse"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Normal"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Depth"))))
		return E_FAIL;

	// For.MRT_ShadowDepth(Shadow)
	if (FAILED(m_pTarget_Manager->Add_MRT(L"MRT_LightDepth", L"Target_ShadowDepth")))
		return E_FAIL;

	/* MRT_LightAcc */
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Shade"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Specular"))))
		return E_FAIL;

	m_pVIBuffer = CVIBuffer_Rect::Create(m_pDevice, m_pContext);

	if (nullptr == m_pVIBuffer)
		return E_FAIL;

	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Deferred.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);

	if (nullptr == m_pShader)
		return E_FAIL;

	XMStoreFloat4x4(&m_WorldMatrix_Deferred, XMMatrixScaling(ViewPortDesc.Width, ViewPortDesc.Height, 1.f));
	XMStoreFloat4x4(&m_ViewMatrix_Deferred, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix_Deferred, XMMatrixOrthographicLH(ViewPortDesc.Width, ViewPortDesc.Height, 0.f, 1.f));

#ifdef _DEBUG
	/*if (FAILED(m_pTarget_Manager->Ready_Debug(TEXT("Target_Diffuse"), 50.0f, 300.0f, 50.f, 50.f)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_Debug(TEXT("Target_Normal"), 50.0f, 350.f, 50.f, 50.f)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_Debug(TEXT("Target_Depth"), 50.0f, 400.0f, 50.f, 50.f)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_Debug(TEXT("Target_Shade"), 100.0f, 300.f, 50.f, 50.f)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_Debug(TEXT("Target_Specular"), 100.0f, 350.f, 50.f, 50.f)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_Debug(TEXT("Target_ShadowDepth"), 100.0f, 400.f, 50.f, 50.f)))
		return E_FAIL;*/
#endif
	Safe_Release(pDepthStencilBuffer);
	Safe_Release(pDepthStencilView);

	return S_OK;
}

HRESULT CRenderer::Add_RenderGroup(RENDERGROUP eRenderGroup, CGameObject * pGameObject)
{
	if (eRenderGroup >= RENDER_END)
		return E_FAIL;

	m_RenderGroup[eRenderGroup].push_back(pGameObject);

	Safe_AddRef(pGameObject);

	return S_OK;
}

HRESULT CRenderer::Draw_RenderGroup()
{
	if (FAILED(Render_Priority()))
		return E_FAIL;

	if (FAILED(Render_ShadowDepth()))
		return E_FAIL;

	if (FAILED(Render_NonBlend()))
		return E_FAIL;

	/* 받아온 정보(디퓨즈, 노말)로 빛 연산을 하고 셰이드 타겟에 기록하자. . */
	if (FAILED(Render_LightAcc()))
		return E_FAIL;
	if (FAILED(Render_Deferred()))
		return E_FAIL;

	/* 디퍼드와 전혀 상관없는 렌더링. */
	if (FAILED(Render_NonLight()))
		return E_FAIL;
	if (FAILED(Render_Blend()))
		return E_FAIL;
	if (FAILED(Render_UI()))
		return E_FAIL;

#ifdef _DEBUG
	/*if (FAILED(Render_Debug()))
		return E_FAIL;*/
#endif

	return S_OK;
}

#ifdef _DEBUG
HRESULT CRenderer::Add_DebugComponent(CComponent* pDebugCom)
{
	/*m_DebugComponents.push_back(pDebugCom);

	Safe_AddRef(pDebugCom);*/

	return S_OK;
}
#endif

HRESULT CRenderer::Render_Priority()
{
	for (auto& pGameObject : m_RenderGroup[RENDER_PRIORITY])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderGroup[RENDER_PRIORITY].clear();

	return S_OK;
}

HRESULT CRenderer::Render_ShadowDepth()
{
	m_pTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_LightDepth"));

	for (auto& pGameObject : m_RenderGroup[RENDER_SHADOWDEPTH])
	{
		if (nullptr != pGameObject)
			pGameObject->Render_Shadow();

		Safe_Release(pGameObject);
	}

	m_RenderGroup[RENDER_SHADOWDEPTH].clear();

	CPipeLine* pPipeLine = CPipeLine::Get_Instance();

	if (FAILED(m_pTarget_Manager->Bind_ShaderResource(m_pShader, TEXT("Target_Normal"), "g_NormalTexture")))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Bind_ShaderResource(m_pShader, TEXT("Target_Depth"), "g_DepthTexture")))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrixInv", &pPipeLine->Get_Transform_Float4x4_Inverse(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	m_pTarget_Manager->End_MRT(m_pContext, TEXT("MRT_LightDepth"));

	return S_OK;
}

HRESULT CRenderer::Render_NonBlend()
{
	/* Diffuse + Normal */
	m_pTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_GameObjects"));

	for (auto& pGameObject : m_RenderGroup[RENDER_NONBLEND])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderGroup[RENDER_NONBLEND].clear();

	m_pTarget_Manager->End_MRT(m_pContext, TEXT("MRT_GameObjects"));

	return S_OK;
}

HRESULT CRenderer::Render_LightAcc()
{
	/* Shade + Specular*/
	m_pTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_LightAcc"));

	/* 셰이더에 노멀 타겟을 바인딩한다. */
	if (FAILED(m_pTarget_Manager->Bind_ShaderResource(m_pShader, TEXT("Target_Normal"), "g_NormalTexture")))
		return E_FAIL;

	/* 셰이더에 뎁스 타겟을 바인딩한다. */
	if (FAILED(m_pTarget_Manager->Bind_ShaderResource(m_pShader, TEXT("Target_Depth"), "g_DepthTexture")))
		return E_FAIL;

	/* 셰이더에 월드을 바인딩한다. */
	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix_Deferred)))
		return E_FAIL;

	/* 셰이더에 뷰을 바인딩한다. */
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix_Deferred)))
		return E_FAIL;

	/* 셰이더에 투영을 바인딩한다. */
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix_Deferred)))
		return E_FAIL;

	CPipeLine* pPipeLine = CPipeLine::Get_Instance();

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrixInv", &pPipeLine->Get_Transform_Float4x4_Inverse(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrixInv", &pPipeLine->Get_Transform_Float4x4_Inverse(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_RawValue("g_vCamPosition", &pPipeLine->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;

	m_pLight_Manager->Render_Lights(m_pShader, m_pVIBuffer);

	/* 백버퍼로 복구한다. */
	m_pTarget_Manager->End_MRT(m_pContext, TEXT("MRT_LightAcc"));

	return S_OK;
}

HRESULT CRenderer::Render_Deferred()
{
	/* 백버퍼에다가 빛연산이 들어간 최종적인 장면을 그려주자 .*/

	/* 셰이더에 월드을 바인딩한다. */
	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix_Deferred)))
		return E_FAIL;

	/* 셰이더에 뷰을 바인딩한다. */
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix_Deferred)))
		return E_FAIL;

	/* 셰이더에 투영을 바인딩한다. */
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix_Deferred)))
		return E_FAIL;

	/* 디퓨즈 텍스쳐를 셰이더에 바인딩 .*/
	if (FAILED(m_pTarget_Manager->Bind_ShaderResource(m_pShader, TEXT("Target_Diffuse"), "g_DiffuseTexture")))
		return E_FAIL;

	/* 셰이드 텍스쳐를 셰이더에 바인딩 .*/
	if (FAILED(m_pTarget_Manager->Bind_ShaderResource(m_pShader, TEXT("Target_Shade"), "g_ShadeTexture")))
		return E_FAIL;

	/* 스펙큘러 텍스쳐를 셰이더에 바인딩 .*/
	if (FAILED(m_pTarget_Manager->Bind_ShaderResource(m_pShader, TEXT("Target_Specular"), "g_SpecularTexture")))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Bind_ShaderResource(m_pShader, TEXT("Target_ShadowDepth"), "g_ShadowDepthTexture")))
		return E_FAIL;

	m_pShader->Begin(3);
	m_pVIBuffer->Render();

	return S_OK;
}

HRESULT CRenderer::Render_NonLight()
{
	for (auto& pGameObject : m_RenderGroup[RENDER_NONLIGHT])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderGroup[RENDER_NONLIGHT].clear();

	return S_OK;
}

_bool Compare(CGameObject* pSour, CGameObject* pDest)
{
	return ((CBlendObject*)pSour)->Get_CamDistance() > ((CBlendObject*)pDest)->Get_CamDistance();
}

HRESULT CRenderer::Render_Blend()
{	
	m_RenderGroup[RENDER_BLEND].sort([](CGameObject* pSour, CGameObject* pDest)->_bool
	{
		return ((CBlendObject*)pSour)->Get_CamDistance() > ((CBlendObject*)pDest)->Get_CamDistance();
	});

	//m_RenderGroup[RENDER_BLEND].sort(Compare);

	for (auto& pGameObject : m_RenderGroup[RENDER_BLEND])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderGroup[RENDER_BLEND].clear();

	return S_OK; 
}

HRESULT CRenderer::Render_UI()
{
	for (auto& pGameObject : m_RenderGroup[RENDER_UI])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderGroup[RENDER_UI].clear();

	return S_OK;
}

#ifdef _DEBUG
HRESULT CRenderer::Render_Debug()
{
	/*for (auto& pDebugCom : m_DebugComponents)
	{
		pDebugCom->Render();
		Safe_Release(pDebugCom);
	}
	m_DebugComponents.clear();

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix_Deferred)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix_Deferred)))
		return E_FAIL;

	m_pTarget_Manager->Debug_Render(m_pShader, m_pVIBuffer, TEXT("MRT_GameObjects"));
	m_pTarget_Manager->Debug_Render(m_pShader, m_pVIBuffer, TEXT("MRT_LightDepth"));
	m_pTarget_Manager->Debug_Render(m_pShader, m_pVIBuffer, TEXT("MRT_LightAcc"));*/

	return S_OK;
}
#endif

CRenderer * CRenderer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRenderer*	pInstance = new CRenderer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Created : CRenderer");
	}

	return pInstance;
}

CComponent * CRenderer::Clone(void * pArg)
{
	AddRef();

	return this;	
}

void CRenderer::Free()
{
	__super::Free();

	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pShader);

	Safe_Release(m_pLight_Manager);
	Safe_Release(m_pTarget_Manager);

	for (size_t i = 0; i < RENDER_END; i++)
	{
		for (auto& pGameObject : m_RenderGroup[i])
			Safe_Release(pGameObject);

		m_RenderGroup[i].clear();
	}

	
}
