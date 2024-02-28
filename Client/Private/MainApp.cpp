#include "stdafx.h"
#include "..\Public\MainApp.h"
#include "GameInstance.h"
#include "Level_Loading.h"

#include "LoadingUI.h"

unsigned int g_CurLevel;
unsigned int g_PlayerLevel;
unsigned int g_PlayerEXP;
unsigned int g_AmountOfBullet;

int g_BulletCount;

bool g_bCamShake;
bool g_bCollidedNPC;
bool g_bTownEnd = false;
bool g_bGamePlayEnd = false;

bool g_bFirstDoorOpened;
bool g_bSecondDoorOpened;
bool g_bFirstWaveDone;
bool g_bSkagDead;
bool g_bSpiderAntDead;
bool g_bThresherDead;

bool	g_bTreasureUsed;
bool	g_bInteractionItem;
bool	g_bInteractionNPC;
int		g_InteractionCount;
bool	g_bInteractionDestroy;
int	g_DialogueCount;

bool g_bSpiderAntScene;
bool g_bSkagScene;
bool g_bThresherScene;
bool g_bMonsterScene;
bool g_bBossWave;

float g_fViewX;
float g_fViewY;

CMainApp::CMainApp()	
	: m_pGameInstance { CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pGameInstance);

	
	// 셰이더
	/*D3D11_RASTERIZER_DESC		RasterizerDesc;
	m_pContext->RSSetState();

	D3D11_BLEND_DESC			BlendDesc;
	m_pContext->OMSetBlendState();


	D3D11_DEPTH_STENCIL_DESC	DepthStencilDesc;
	m_pContext->OMSetDepthStencilState(pDepthStencilState);*/
}

HRESULT CMainApp::Initialize()
{

#ifdef _DEBUG
	//m_strFPS.resize(MAX_PATH);
#endif

	//D3D11_SAMPLER_DESC

	/* 메인앱의 초기화작업*/
	GRAPHICDESC			GraphicDesc;
	ZeroMemory(&GraphicDesc, sizeof GraphicDesc);

	GraphicDesc.hWnd = g_hWnd;
	GraphicDesc.iSizeX = g_iWinSizeX;
	GraphicDesc.iSizeY = g_iWinSizeY;
	GraphicDesc.eWinMode = GRAPHICDESC::WINMODE_WIN;

	if (FAILED(m_pGameInstance->Initialize_Engine(g_hInst, LEVEL_END, GraphicDesc, &m_pDevice, &m_pContext)))
		return E_FAIL;

	if (FAILED(Ready_Prototype_Component_ForStatic()))
		return E_FAIL;

	if (FAILED(Ready_Prototype_Collider_ForStatic()))
		return E_FAIL;

	if (FAILED(Open_Level(LEVEL_LOGO)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Font(m_pDevice, m_pContext, TEXT("Font_136"), TEXT("../Bin/Resources/Fonts/136ex.spritefont"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Font(m_pDevice, m_pContext, TEXT("Dongle"), TEXT("../Bin/Resources/Fonts/dongleex.spritefont"))))
		return E_FAIL;
	return S_OK;
}

void CMainApp::Tick(_float fTimeDelta)
{
	if (nullptr == m_pGameInstance)
		return;

#ifdef _DEBUG
	//m_fTimeAcc += fTimeDelta;
#endif

	m_pGameInstance->Tick_Engine(fTimeDelta);
}

HRESULT CMainApp::Render()
{
	if (nullptr == m_pGameInstance || 
		nullptr == m_pRenderer)
		return E_FAIL;

	m_pGameInstance->Clear_BackBuffer_View(_float4(0.f, 0.f, 0.f, 1.f));
	m_pGameInstance->Clear_DepthStencil_View();
		
	m_pRenderer->Draw_RenderGroup();

#ifdef _DEBUG

	/*++m_iRenderCnt;

	if (m_fTimeAcc >= 1.f)
	{
		wsprintf(&m_strFPS.front(), TEXT("FPS : %d"), m_iRenderCnt);

		m_iRenderCnt = 0;

		m_fTimeAcc = 0.f;
	}

	m_pGameInstance->Render_Font(TEXT("Font_136"), m_strFPS, _float2(0.f, 0.f), XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.5f);*/

#endif

	m_pGameInstance->Present();

	
	return S_OK;
}

HRESULT CMainApp::Open_Level(LEVEL eNextLevel)
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, eNextLevel))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_Component_ForStatic()
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	/* For.Prototype_Component_Renderer */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		m_pRenderer = CRenderer::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* 정점 구조체 안에 들어있는 멤버하나의 정보다. */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPosTex.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Rect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_Transform */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		CTransform::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	///* For.Prototype_Component_Shader_Rect */
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Rect"),
	//	CShader::Create(m_pDevice, pContext, TEXT("../Bin/ShaderFiles/Shader_Rect.hlsl")))))
	//	return E_FAIL;

	 /* 로딩 UI*/
	/* For.Prototype_GameObject_LoadingUI */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOADING, TEXT("Prototype_Component_Texture_LoadingUI"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Borderland/Loading/GamePlay.dds")))))
		return E_FAIL;

	/* For.Prototype_GameObject_LoadingUI */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOADING, TEXT("Prototype_Component_Texture_LoadingTown"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Borderland/Loading/Town.dds")))))
		return E_FAIL;

	/* For.Prototype_GameObject_LoadingUI */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOADING, TEXT("Prototype_Component_Texture_LoadingEnding"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Borderland/Loading/Sanctuary.dds")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_LoadingUI"),
		CLoadingUI::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	Safe_AddRef(m_pRenderer);
	
	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_Collider_ForStatic()
{
	/* For.Prototype_Component_Collider_AABB*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_AABB))))
		return E_FAIL;
	/* For.Prototype_Component_Collider_OBB*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_OBB))))
		return E_FAIL;
	/* For.Prototype_Component_Collider_Sphere*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_SPHERE))))
		return E_FAIL;

	return S_OK;
}

CMainApp * CMainApp::Create()
{
	CMainApp*	pInstance = new CMainApp();

	if (FAILED(pInstance->Initialize()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Created : CMainApp");
	}

	return pInstance;
}

void CMainApp::Free()
{
	Safe_Release(m_pRenderer);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	Safe_Release(m_pGameInstance);

	CGameInstance::Release_Engine();
}
