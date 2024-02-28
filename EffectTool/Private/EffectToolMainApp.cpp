#include "EffectToolMainApp.h"
#include "GameInstance.h"
#include "EffectImgui_Manager.h"

#include "Effect_Terrain.h"
#include "EffectTool_Camera.h"
#include "Snow.h"
#include "Bleeding.h"

CEffectToolMainApp::CEffectToolMainApp()
	: m_pGameInstance{ CGameInstance::Get_Instance() }
	, m_pImgui_Manager{ CEffectImgui_Manager::Get_Instance() }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pImgui_Manager);

}

HRESULT CEffectToolMainApp::Initialize()
{
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

	if (FAILED(Ready_Prototype_Object_ForStatic()))
		return E_FAIL;

	m_pImgui_Manager->Initialize(m_pDevice, m_pContext);

	if (FAILED(Ready_Lights()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Terrain(TEXT("Layer_Terrain"))))
		return E_FAIL;

	/*if (FAILED(Ready_Layer_Effect(TEXT("Layer_Effect"))))
		return E_FAIL;*/

	return S_OK;
}

void CEffectToolMainApp::Tick(_float fTimeDelta)
{
	if (nullptr == m_pGameInstance)
		return;

	m_pGameInstance->Tick_Engine(fTimeDelta);

	m_pImgui_Manager->Tick();

	Add_Layer();
}

HRESULT CEffectToolMainApp::Render()
{
	if (nullptr == m_pGameInstance || nullptr == m_pRenderer)
		return E_FAIL;

	m_pGameInstance->Clear_BackBuffer_View(_float4(0.f, 0.f, 0.f, 1.f));
	m_pGameInstance->Clear_DepthStencil_View();

	m_pRenderer->Draw_RenderGroup();

	m_pImgui_Manager->Render();

	m_pGameInstance->Present();

	return S_OK;
}

HRESULT CEffectToolMainApp::Ready_Prototype_Component_ForStatic()
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

	return S_OK;
}

HRESULT CEffectToolMainApp::Ready_Prototype_Object_ForStatic()
{
#pragma region Texture
	/* For.Prototype_Component_Texture_Snow */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_SnowTerrain"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Borderland/Terrain/Snow.dds")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Blood_Decal */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Bleeding"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Borderland/Effect/Blood_Flipbook%d.png"), 24))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Snow */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Snow"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Borderland/Effect/Snow.png")))))
		return E_FAIL;

#pragma endregion

	/* For.Prototype_Component_VIBuffer_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Terrain/Flatland.bmp")))))
		return E_FAIL;


#pragma region Shader
	/* For.Prototype_Component_Shader_VtxNorTex */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxNorTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxMesh */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxMesh.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxCubeTex */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxCubeTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxCubeTex.hlsl"), VTXCUBETEX::Elements, VTXCUBETEX::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxRectInstance */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxRectInstance"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxRectInstance.hlsl"), VTXRECTINSTANCE::Elements, VTXRECTINSTANCE::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxPointInstance */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPointInstance"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPointInstance.hlsl"), VTXPOINTINSTANCE::Elements, VTXPOINTINSTANCE::iNumElements))))
		return E_FAIL;

#pragma endregion

	/* For.Prototype_Component_VIBuffer_Rect_Instance*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect_Instance"),
		CVIBuffer_Rect_Instance::Create(m_pDevice, m_pContext, 300))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Point_Instance*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Point_Instance"),
		CVIBuffer_Point_Instance::Create(m_pDevice, m_pContext, 100))))
		return E_FAIL;


#pragma region Objects_Prototype
	/* For.Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain"),
		CEffect_Terrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Tool_Camera */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Tool_Camera"),
		CEffectTool_Camera::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Snow*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Snow"),
		CSnow::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Bleeding*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Bleeding"),
		CBleeding::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion

	return S_OK;
}

HRESULT CEffectToolMainApp::Add_Layer()
{
	CVIBuffer_Point_Instance::POINTINSTANCEDESC		Desc;
	Desc.iNumInstance = m_pImgui_Manager->Get_EffectInfo().iNumber;
	Desc.fLifeTime = m_pImgui_Manager->Get_EffectInfo().fLifeTime;
	Desc.vPivotPos = m_pImgui_Manager->Get_EffectInfo().vPivotPos;
	Desc.vRange = m_pImgui_Manager->Get_EffectInfo().vRange;
	Desc.fMinSpeed = m_pImgui_Manager->Get_EffectInfo().fMinSpeed;
	Desc.fMaxSpeed = m_pImgui_Manager->Get_EffectInfo().fMaxSpeed;
	Desc.bRising = m_pImgui_Manager->Get_EffectInfo().bRising;
	Desc.bFalling = m_pImgui_Manager->Get_EffectInfo().bFalling;
	Desc.bExplosion = m_pImgui_Manager->Get_EffectInfo().bExplosion;
	Desc.bBleeding = m_pImgui_Manager->Get_EffectInfo().bBleeding;
	Desc.bGreen = m_pImgui_Manager->Get_EffectInfo().bGreen;
	Desc.bDig = m_pImgui_Manager->Get_EffectInfo().bDig;

	if (m_pImgui_Manager->Get_Create() && m_pImgui_Manager->Get_PNGName() == TEXT("Snow") && !m_pImgui_Manager->Get_Load())
	{
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Layer_Effect_Snow"),
			TEXT("Prototype_GameObject_Snow"),&Desc)))
			return E_FAIL;

		m_pImgui_Manager->Set_Create(false);
	}
	if (m_pImgui_Manager->Get_Load())
	{
		/*if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Layer_Effect_Snow"),
			TEXT("Prototype_GameObject_Snow"), &Desc)))
			return E_FAIL;*/
		Falling_Snow();
		m_pImgui_Manager->Set_Load(false);
	}

	if (m_pImgui_Manager->Get_Create() && m_pImgui_Manager->Get_PNGName() == TEXT("Blood_Flipbook0") && !m_pImgui_Manager->Get_Load())
	{
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Layer_Effect_Bleeding"),
			TEXT("Prototype_GameObject_Bleeding"), &Desc)))
			return E_FAIL;

		m_pImgui_Manager->Set_Create(false);
	}
	if (m_pImgui_Manager->Get_Load())
	{
		Bleeding();
		m_pImgui_Manager->Set_Load(false);
	}
		
	return S_OK;
}

HRESULT CEffectToolMainApp::Ready_Lights()
{
	LIGHTDESC		LightDesc;

	/* 방향성 광원을 추가한다. */
	ZeroMemory(&LightDesc, sizeof LightDesc);

	LightDesc.eType = LIGHTDESC::TYPE_DIRECTIONAL;
	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);

	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	if (FAILED(m_pGameInstance->Add_Lights(m_pDevice, m_pContext, LightDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffectToolMainApp::Ready_Layer_Camera(const wstring& strLayerTag)
{
	CEffectTool_Camera::CAMERAFREEDESC		ToolCameraDesc;

	//CameraFreeDesc.iData = 10;
	ToolCameraDesc.CameraDesc.iLevelIndex = LEVEL_STATIC;
	ToolCameraDesc.CameraDesc.strTranformTag = TEXT("Prototype_Component_Transform");
	ToolCameraDesc.CameraDesc.vEye = _float4(0.f, 10.f, -7.f, 1.f);
	ToolCameraDesc.CameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	ToolCameraDesc.CameraDesc.fFovy = XMConvertToRadians(60.0f);
	ToolCameraDesc.CameraDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	ToolCameraDesc.CameraDesc.fNear = 0.2f;
	ToolCameraDesc.CameraDesc.fFar = 300.f;
	ToolCameraDesc.CameraDesc.TransformDesc.fSpeedPerSec = 10.0f;
	ToolCameraDesc.CameraDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Layer_Camera"),
		TEXT("Prototype_GameObject_Tool_Camera"), &ToolCameraDesc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CEffectToolMainApp::Ready_Layer_Terrain(const wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Layer_Terrain"),
		TEXT("Prototype_GameObject_Terrain"))))
		return E_FAIL;
	return S_OK;
}

HRESULT CEffectToolMainApp::Ready_Layer_Effect(const wstring& strLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_STATIC, strLayerTag,
		TEXT("Prototype_GameObject_Snow"))))
		return E_FAIL;

	Safe_Release(pGameInstance);
	return S_OK;
}

HRESULT CEffectToolMainApp::Falling_Snow()
{
	_ulong		dwByte = 0;
	HANDLE		hFile = CreateFile(TEXT("../../Client/Bin/Resources/Effect/Dig.txt"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	size_t		dataLength;
	wstring		Name;

	CVIBuffer_Point_Instance::POINTINSTANCEDESC		Desc;
	while (true)
	{
		if (!ReadFile(hFile, &dataLength, sizeof(size_t), &dwByte, nullptr))
			break;

		if (dataLength == 0)
			break;

		if (0 == dwByte)
			break;

		Name.resize(dataLength);
		if (!ReadFile(hFile, &Name[0], sizeof(wchar_t) * dataLength, &dwByte, nullptr))
			break;
		if (!ReadFile(hFile, &Desc.iNumInstance, sizeof(_int), &dwByte, nullptr))
			break;
		if (!ReadFile(hFile, &Desc.fLifeTime, sizeof(_float), &dwByte, nullptr))
			break;
		if (!ReadFile(hFile, &Desc.vPivotPos, sizeof(_float4), &dwByte, nullptr))
			break;
		if (!ReadFile(hFile, &Desc.vRange, sizeof(_float3), &dwByte, nullptr))
			break;
		if (!ReadFile(hFile, &Desc.fMinSpeed, sizeof(_float), &dwByte, nullptr))
			break;
		if (!ReadFile(hFile, &Desc.fMaxSpeed, sizeof(_float), &dwByte, nullptr))
			break;
		if (!ReadFile(hFile, &Desc.bFalling, sizeof(_bool), &dwByte, nullptr))
			break;
		if (!ReadFile(hFile, &Desc.bRising, sizeof(_bool), &dwByte, nullptr))
			break;
		if (!ReadFile(hFile, &Desc.bExplosion, sizeof(_bool), &dwByte, nullptr))
			break;
		if (!ReadFile(hFile, &Desc.bBleeding, sizeof(_bool), &dwByte, nullptr))
			break;
		if (!ReadFile(hFile, &Desc.bDig, sizeof(_bool), &dwByte, nullptr))
			break;

		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Layer_Effect_Snow"),
			TEXT("Prototype_GameObject_Snow"), &Desc)))
			return E_FAIL;

	}
	CloseHandle(hFile);

	return S_OK;
}

HRESULT CEffectToolMainApp::Bleeding()
{
	_ulong		dwByte = 0;
	HANDLE		hFile = CreateFile(TEXT("../../Client/Bin/Resources/Effect/FallingSnow.txt"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	size_t		dataLength;
	wstring		Name;

	CVIBuffer_Point_Instance::POINTINSTANCEDESC		Desc;
	while (true)
	{
		if (!ReadFile(hFile, &dataLength, sizeof(size_t), &dwByte, nullptr))
			break;

		if (dataLength == 0)
			break;

		if (0 == dwByte)
			break;

		Name.resize(dataLength);
		if (!ReadFile(hFile, &Name[0], sizeof(wchar_t) * dataLength, &dwByte, nullptr))
			break;
		if (!ReadFile(hFile, &Desc.iNumInstance, sizeof(_int), &dwByte, nullptr))
			break;
		if (!ReadFile(hFile, &Desc.fLifeTime, sizeof(_float), &dwByte, nullptr))
			break;
		if (!ReadFile(hFile, &Desc.vPivotPos, sizeof(_float4), &dwByte, nullptr))
			break;
		if (!ReadFile(hFile, &Desc.vRange, sizeof(_float3), &dwByte, nullptr))
			break;
		if (!ReadFile(hFile, &Desc.fMinSpeed, sizeof(_float), &dwByte, nullptr))
			break;
		if (!ReadFile(hFile, &Desc.fMaxSpeed, sizeof(_float), &dwByte, nullptr))
			break;
		if (!ReadFile(hFile, &Desc.bFalling, sizeof(_bool), &dwByte, nullptr))
			break;
		if (!ReadFile(hFile, &Desc.bRising, sizeof(_bool), &dwByte, nullptr))
			break;
		if (!ReadFile(hFile, &Desc.bExplosion, sizeof(_bool), &dwByte, nullptr))
			break;
		if (!ReadFile(hFile, &Desc.bBleeding, sizeof(_bool), &dwByte, nullptr))
			break;
		if (!ReadFile(hFile, &Desc.bDig, sizeof(_bool), &dwByte, nullptr))
			break;

		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Layer_Effect_Bleeding"),
			TEXT("Prototype_GameObject_Blood_Decal"), &Desc)))
			return E_FAIL;

	}
	CloseHandle(hFile);

	return S_OK;
}

CEffectToolMainApp* CEffectToolMainApp::Create()
{
	CEffectToolMainApp* pInstance = new CEffectToolMainApp();

	if (FAILED(pInstance->Initialize()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Created : CEffectToolMainApp");
	}

	return pInstance;
}

void CEffectToolMainApp::Free()
{
	CEffectImgui_Manager::Destroy_Instance();
	Safe_Release(m_pImgui_Manager);
	Safe_Release(m_pRenderer);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	Safe_Release(m_pGameInstance);

	CGameInstance::Release_Engine();
}
