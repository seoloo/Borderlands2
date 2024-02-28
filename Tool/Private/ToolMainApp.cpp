#include "ToolMainApp.h"
#include "GameInstance.h"
#include "Tool_Camera.h"
#include "Imgui_Manager.h"
#include "Tool_ObjectManager.h"
#include "Level_Tool.h"
#include "Level_ToolLoading.h"


#include "Tool_Terrain.h"
#include "Tool_Camera.h"

#pragma region StaticObject
#include "Tool_Object_Icebox01.h"
#include "Large_Rock1.h"
#include "Bench.h"
#include "CircularSupport.h"
#include "Container.h"
#include "Crate.h"
#include "CrateSet.h"
#include "House.h"
#include "Yurt.h"
#include "Yurt2.h"
#include "WaterTower.h"
#include "Rock1.h"
#include "TunnelDoor.h"
#include "Pillar.h"
#include "MetalBeam.h"
#include "MetalLadder.h"
#include "DamWall.h"
#include "BarCounter.h"
#include "BedGarbage.h"
#include "Dropship.h"
#include "JakobsTank.h"
#include "DropzoneWall.h"
#include "LynchwoodHotel.h"
#include "Boat.h"
#include "ForkLift.h"
#pragma endregion

CToolMainApp::CToolMainApp()
    :  m_pObject_Manager{ CTool_ObjectManager::Get_Instance() }
    , m_pGameInstance{ CGameInstance::Get_Instance() }
	, m_pImgui_Manager{ CImgui_Manager::Get_Instance() }
{
	Safe_AddRef(m_pGameInstance);
    Safe_AddRef(m_pObject_Manager);
	Safe_AddRef(m_pImgui_Manager);
}

HRESULT CToolMainApp::Initialize()
{
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

	/*if (FAILED(Open_Level(LEVEL_TOOL)))
		return E_FAIL;*/

	m_pImgui_Manager->Initialize(m_pDevice, m_pContext);


	if (FAILED(Ready_Lights()))
		return E_FAIL;
	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;
	if (FAILED(Ready_Layer_Terrain(TEXT("Layer_Terrain"))))
		return E_FAIL;

	return S_OK;
}

void CToolMainApp::Tick(_float fTimeDelta)
{
	if (nullptr == m_pGameInstance)
		return;

	m_pGameInstance->Tick_Engine(fTimeDelta);

	m_pImgui_Manager->Tick();

	m_pObject_Manager->Tick(fTimeDelta);

	Add_Layer();

}

HRESULT CToolMainApp::Render()
{
	if (nullptr == m_pGameInstance ||
		nullptr == m_pRenderer)
		return E_FAIL;

	m_pGameInstance->Clear_BackBuffer_View(_float4(0.f, 0.5f, 1.f, 1.f));
	m_pGameInstance->Clear_DepthStencil_View();

	m_pRenderer->Draw_RenderGroup();

	m_pImgui_Manager->Render();

	m_pGameInstance->Present();

	return S_OK;
}

HRESULT CToolMainApp::Open_Level(LEVEL eNextLevel)
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_ToolLoading::Create(m_pDevice, m_pContext, eNextLevel))))
		return E_FAIL;

	return S_OK;
}

HRESULT CToolMainApp::Ready_Prototype_Component_ForStatic()
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	/* For.Prototype_Component_Renderer */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		m_pRenderer = CRenderer::Create(m_pDevice, m_pContext))))
		return E_FAIL;

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

	Safe_AddRef(m_pRenderer);

	return S_OK;
}

HRESULT CToolMainApp::Ready_Prototype_Object_ForStatic()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

#pragma region Terrain

	/* For.Prototype_Component_Texture_SlateRock3_Dif */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_SlateRock3_Dif"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Borderland/Terrain/SlateRock3_Dif.dds")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_SnowDrift_01_Dif */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_SnowDrift_01_Dif"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Borderland/Terrain/SnowDrift_01_Dif.dds")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Bright_Lava_Tile2_Dif */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Bright_Lava_Tile2_Dif"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Borderland/Terrain/Bright_Lava_Tile2_Dif.dds")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Tile0 */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Tile0"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Terrain/Tile0.dds")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Snow */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Snow"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Borderland/Terrain/Snow.dds")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_FrozenLake */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_FrozenLake"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Borderland/Terrain/FrozenLake.dds")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Grassland */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Grassland"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Borderland/Terrain/Grassland.dds")))))
		return E_FAIL;



#pragma endregion


	/* For.Prototype_Component_VIBuffer_Terrain */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Terrain/Flatland.bmp")))))
		return E_FAIL;

	/* For.Prototype_Component_Navigation */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Map/NaviTest.txt"),3))))
		return E_FAIL;

#pragma region Static
	_matrix		PivotMatrix = XMMatrixIdentity();
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(0.f));

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_IceBlock01"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Client/Bin/Resources/Borderland/Thresher_Static/IceBlock01.fbx", PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Large_Rock1"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Client/Bin/Resources/Borderland/Thresher_Static/Large_Rock1.fbx", PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Bench"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Client/Bin/Resources/Borderland/Thresher_Static/obj_Bench.fbx", PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_CicularSupport"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Client/Bin/Resources/Borderland/Thresher_Static/obj_CircularSupport.fbx", PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Container"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Client/Bin/Resources/Borderland/Thresher_Static/obj_Container.fbx", PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Crate"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Client/Bin/Resources/Borderland/Thresher_Static/obj_Crate.fbx", PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_CrateSet"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Client/Bin/Resources/Borderland/Thresher_Static/obj_CrateSet.fbx", PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_House"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Client/Bin/Resources/Borderland/Thresher_Static/obj_House.fbx", PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Yurt"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Client/Bin/Resources/Borderland/Thresher_Static/obj_Yurt.fbx", PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Yurt2"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Client/Bin/Resources/Borderland/Thresher_Static/obj_Yurt2.fbx", PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_WaterTower"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Client/Bin/Resources/Borderland/Thresher_Static/obj_WaterTower.fbx", PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_TunnelDoor"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Client/Bin/Resources/Borderland/Thresher_Static/obj_TunnelDoor.fbx", PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Rock1"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Client/Bin/Resources/Borderland/Thresher_Static/obj_Rock1.fbx", PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Pillar"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Client/Bin/Resources/Borderland/Thresher_Static/obj_Pillar.fbx", PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_MetalBeam"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Client/Bin/Resources/Borderland/Thresher_Static/obj_MetalBeam.fbx", PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_MetalLadder"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Client/Bin/Resources/Borderland/Thresher_Static/obj_MetalLadder.fbx", PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_DamWall"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Client/Bin/Resources/Borderland/Thresher_Static/obj_DamWall.fbx", PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_BarCounter"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Client/Bin/Resources/Borderland/Thresher_Static/obj_BarCounter.fbx", PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_BedGarbage"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Client/Bin/Resources/Borderland/Thresher_Static/obj_BedGarbage.fbx", PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_JakobsTank"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Client/Bin/Resources/Borderland/Thresher_Static/obj_JakobsTank.fbx", PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_DropzoneWall"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Client/Bin/Resources/Borderland/Thresher_Static/obj_DropzoneWall.fbx", PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_LynchwoodHotel"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Client/Bin/Resources/Borderland/Thresher_Static/obj_LynchwoodHotel.fbx", PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Dropship"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Client/Bin/Resources/Borderland/Thresher_Static/obj_Dropship.fbx", PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Boat"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Client/Bin/Resources/Borderland/Thresher_Static/obj_Boat.fbx", PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_ForkLift"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Client/Bin/Resources/Borderland/Thresher_Static/obj_ForkLift.fbx", PivotMatrix))))
		return E_FAIL;

#pragma endregion

	/* For.Prototype_Component_Shader_VtxNorTex */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxNorTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxMesh */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxMesh.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxRectInstance */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxRectInstance"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxRectInstance.hlsl"), VTXRECTINSTANCE::Elements, VTXRECTINSTANCE::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Navigation */
	/*if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Map/NaviTest.txt")))))
		return E_FAIL;*/

	/* For.Terrain */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain"),
		CTool_Terrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Tool_Camera */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Tool_Camera"),
		CTool_Camera::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma region Static
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_IceBlock01"),
		CTool_Object_Icebox01::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Large_Rock1"),
		CLarge_Rock1::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Bench"),
		CBench::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_CircularSupport"),
		CCircularSupport::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Container"),
		CContainer::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Crate"),
		CCrate::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_CrateSet"),
		CCrateSet::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_House"),
		CHouse::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Yurt"),
		CYurt::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Yurt2"),
		CYurt2::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_WaterTower"),
		CWaterTower::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TunnelDoor"),
		CTunnelDoor::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Rock1"),
		CRock1::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Pillar"),
		CPillar::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MetalBeam"),
		CMetalBeam::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MetalLadder"),
		CMetalLadder::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_DamWall"),
		CDamWall::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BarCounter"),
		CBarCounter::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BedGarbage"),
		CBedGarbage::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Dropship"),
		CDropship::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_JakobsTank"),
		CJakobsTank::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_DropzoneWall"),
		CDropzoneWall::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_LynchwoodHotel"),
		CLynchwoodHotel::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Boat"),
		CBoat::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ForkLift"),
		CForkLift::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma endregion


	Safe_Release(pGameInstance);
	return S_OK;
}

HRESULT CToolMainApp::Ready_Lights()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	LIGHTDESC		LightDesc;

	/* 방향성 광원을 추가한다. */
	ZeroMemory(&LightDesc, sizeof LightDesc);

	LightDesc.eType = LIGHTDESC::TYPE_DIRECTIONAL;
	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);

	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	if (FAILED(pGameInstance->Add_Lights(m_pDevice, m_pContext, LightDesc)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CToolMainApp::Ready_Layer_Camera(const wstring& strLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CTool_Camera::CAMERAFREEDESC		ToolCameraDesc;

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

	/* For.Camera */
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOOL, strLayerTag,
		TEXT("Prototype_GameObject_Tool_Camera"), &ToolCameraDesc)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CToolMainApp::Ready_Layer_Terrain(const wstring& strLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	/* For.Terrain */
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOOL, strLayerTag,
		TEXT("Prototype_GameObject_Terrain"))))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

void CToolMainApp::Add_Layer()
{
	m_ObjectName = TEXT("Layer_Object") + to_wstring(m_iCount);

	/*if (m_pImgui_Manager->Get_Instance()->Get_FBXName() == TEXT("IceBlock01") &&
		m_pImgui_Manager->Get_Instance()->Get_Create() == true)
	{
		if (FAILED(Ready_Layer_IceBlock01(m_ObjectName)))
			return;
	}
	if (m_pImgui_Manager->Get_Instance()->Get_FBXName() == TEXT("Large_Rock1") &&
		m_pImgui_Manager->Get_Instance()->Get_Create() == true)
	{
		if (FAILED(Ready_Layer_Large_Rock1(m_ObjectName)))
			return;
	}

	if (m_pImgui_Manager->Get_Instance()->Get_FBXName() == TEXT("obj_Bench") &&
		m_pImgui_Manager->Get_Instance()->Get_Create() == true)
	{
		if (FAILED(Ready_Layer_Bench(m_ObjectName)))
			return;
	}

	if (m_pImgui_Manager->Get_Instance()->Get_FBXName() == TEXT("obj_CircularSupport") &&
		m_pImgui_Manager->Get_Instance()->Get_Create() == true)
	{
		if (FAILED(Ready_Layer_CircularSupport(m_ObjectName)))
			return;
	}

	if (m_pImgui_Manager->Get_Instance()->Get_FBXName() == TEXT("obj_Container") &&
		m_pImgui_Manager->Get_Instance()->Get_Create() == true)
	{
		if (FAILED(Ready_Layer_Container(m_ObjectName)))
			return;
	}

	if (m_pImgui_Manager->Get_Instance()->Get_FBXName() == TEXT("obj_Crate") &&
		m_pImgui_Manager->Get_Instance()->Get_Create() == true)
	{
		if (FAILED(Ready_Layer_Crate(m_ObjectName)))
			return;
	}

	if (m_pImgui_Manager->Get_Instance()->Get_FBXName() == TEXT("obj_CrateSet") &&
		m_pImgui_Manager->Get_Instance()->Get_Create() == true)
	{
		if (FAILED(Ready_Layer_CrateSet(m_ObjectName)))
			return;
	}*/

	if (m_pImgui_Manager->Get_Instance()->Get_FBXName() == TEXT("obj_House") &&
		m_pImgui_Manager->Get_Instance()->Get_Create() == true)
	{
		if (FAILED(Ready_Layer_House(m_ObjectName)))
			return;
	}

	if (m_pImgui_Manager->Get_Instance()->Get_FBXName() == TEXT("obj_Yurt") &&
		m_pImgui_Manager->Get_Instance()->Get_Create() == true)
	{
		if (FAILED(Ready_Layer_Yurt(m_ObjectName)))
			return;
	}

	if (m_pImgui_Manager->Get_Instance()->Get_FBXName() == TEXT("obj_Yurt2") &&
		m_pImgui_Manager->Get_Instance()->Get_Create() == true)
	{
		if (FAILED(Ready_Layer_Yurt2(m_ObjectName)))
			return;
	}

	if (m_pImgui_Manager->Get_Instance()->Get_FBXName() == TEXT("obj_WaterTower") &&
		m_pImgui_Manager->Get_Instance()->Get_Create() == true)
	{
		if (FAILED(Ready_Layer_WaterTower(m_ObjectName)))
			return;
	}

	if (m_pImgui_Manager->Get_Instance()->Get_FBXName() == TEXT("obj_TunnelDoor") &&
		m_pImgui_Manager->Get_Instance()->Get_Create() == true)
	{
		if (FAILED(Ready_Layer_TunnelDoor(m_ObjectName)))
			return;
	}
	if (m_pImgui_Manager->Get_Instance()->Get_FBXName() == TEXT("obj_Rock1") &&
		m_pImgui_Manager->Get_Instance()->Get_Create() == true)
	{
		if (FAILED(Ready_Layer_Rock1(m_ObjectName)))
			return;
	}

	if (m_pImgui_Manager->Get_Instance()->Get_FBXName() == TEXT("obj_Pillar") &&
		m_pImgui_Manager->Get_Instance()->Get_Create() == true)
	{
		if (FAILED(Ready_Layer_Pillar(m_ObjectName)))
			return;
	}

	if (m_pImgui_Manager->Get_Instance()->Get_FBXName() == TEXT("obj_MetalLadder") &&
		m_pImgui_Manager->Get_Instance()->Get_Create() == true)
	{
		if (FAILED(Ready_Layer_MetalLadder(m_ObjectName)))
			return;
	}

	if (m_pImgui_Manager->Get_Instance()->Get_FBXName() == TEXT("obj_MetalBeam") &&
		m_pImgui_Manager->Get_Instance()->Get_Create() == true)
	{
		if (FAILED(Ready_Layer_MetalBeam(m_ObjectName)))
			return;
	}

	if (m_pImgui_Manager->Get_Instance()->Get_FBXName() == TEXT("obj_DamWall") &&
		m_pImgui_Manager->Get_Instance()->Get_Create() == true)
	{
		if (FAILED(Ready_Layer_DamWall(m_ObjectName)))
			return;
	}

	/*if (m_pImgui_Manager->Get_Instance()->Get_FBXName() == TEXT("obj_BarCounter") &&
		m_pImgui_Manager->Get_Instance()->Get_Create() == true)
	{
		if (FAILED(Ready_Layer_BarCounter(m_ObjectName)))
			return;
	}

	if (m_pImgui_Manager->Get_Instance()->Get_FBXName() == TEXT("obj_BedGarbage") &&
		m_pImgui_Manager->Get_Instance()->Get_Create() == true)
	{
		if (FAILED(Ready_Layer_BedGarbage(m_ObjectName)))
			return;
	}*/

	if (m_pImgui_Manager->Get_Instance()->Get_FBXName() == TEXT("obj_Dropship") &&
		m_pImgui_Manager->Get_Instance()->Get_Create() == true)
	{
		if (FAILED(Ready_Layer_Dropship(m_ObjectName)))
			return;
	}

	if (m_pImgui_Manager->Get_Instance()->Get_FBXName() == TEXT("obj_JakobsTank") &&
		m_pImgui_Manager->Get_Instance()->Get_Create() == true)
	{
		if (FAILED(Ready_Layer_JakobsTank(m_ObjectName)))
			return;
	}

	if (m_pImgui_Manager->Get_Instance()->Get_FBXName() == TEXT("obj_DropzoneWall") &&
		m_pImgui_Manager->Get_Instance()->Get_Create() == true)
	{
		if (FAILED(Ready_Layer_DropzoneWall(m_ObjectName)))
			return;
	}

	if (m_pImgui_Manager->Get_Instance()->Get_FBXName() == TEXT("obj_LynchwoodHotel") &&
		m_pImgui_Manager->Get_Instance()->Get_Create() == true)
	{
		if (FAILED(Ready_Layer_LynchwoodHotel(m_ObjectName)))
			return;
	}

	if (m_pImgui_Manager->Get_Instance()->Get_FBXName() == TEXT("obj_Boat") &&
		m_pImgui_Manager->Get_Instance()->Get_Create() == true)
	{
		if (FAILED(Ready_Layer_Boat(m_ObjectName)))
			return;
	}

	if (m_pImgui_Manager->Get_Instance()->Get_FBXName() == TEXT("obj_ForkLift") &&
		m_pImgui_Manager->Get_Instance()->Get_Create() == true)
	{
		if (FAILED(Ready_Layer_ForkLift(m_ObjectName)))
			return;
	}

	if (m_pImgui_Manager->Get_Instance()->Get_Load())
	{
		/*if (FAILED(Ready_Layer_IceBlock01(m_ObjectName)))
			return;

		if (FAILED(Ready_Layer_Large_Rock1(m_ObjectName)))
			return;

		if (FAILED(Ready_Layer_Bench(m_ObjectName)))
			return;

		if (FAILED(Ready_Layer_CircularSupport(m_ObjectName)))
			return;

		if (FAILED(Ready_Layer_Container(m_ObjectName)))
			return;

		if (FAILED(Ready_Layer_Crate(m_ObjectName)))
			return;

		if (FAILED(Ready_Layer_CrateSet(m_ObjectName)))
			return;*/

		if (FAILED(Ready_Layer_House(m_ObjectName)))
			return;

		if (FAILED(Ready_Layer_Yurt(m_ObjectName)))
			return;

		if (FAILED(Ready_Layer_Yurt2(m_ObjectName)))
			return;

		if (FAILED(Ready_Layer_WaterTower(m_ObjectName)))
			return;

		if (FAILED(Ready_Layer_Rock1(m_ObjectName)))
			return;

		if (FAILED(Ready_Layer_Pillar(m_ObjectName)))
			return;

		if (FAILED(Ready_Layer_DamWall(m_ObjectName)))
			return;

		if (FAILED(Ready_Layer_TunnelDoor(m_ObjectName)))
			return;

		if (FAILED(Ready_Layer_MetalBeam(m_ObjectName)))
			return;

		if (FAILED(Ready_Layer_MetalLadder(m_ObjectName)))
			return;

		/*if (FAILED(Ready_Layer_BarCounter(m_ObjectName)))
			return;

		if (FAILED(Ready_Layer_BedGarbage(m_ObjectName)))
			return;*/

		if (FAILED(Ready_Layer_Dropship(m_ObjectName)))
			return;

		if (FAILED(Ready_Layer_JakobsTank(m_ObjectName)))
			return;

		if (FAILED(Ready_Layer_DropzoneWall(m_ObjectName)))
			return;

		if (FAILED(Ready_Layer_LynchwoodHotel(m_ObjectName)))
			return;

		if (FAILED(Ready_Layer_Boat(m_ObjectName)))
			return;

		if (FAILED(Ready_Layer_ForkLift(m_ObjectName)))
			return;

		m_pImgui_Manager->Get_Instance()->Set_Load(false);

	}
}

HRESULT CToolMainApp::Ready_Layer_IceBlock01(const wstring& strLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CTool_Object_Icebox01::OBJECTDESC Desc;

	_float3 PickingPos = m_pImgui_Manager->Get_Instance()->Get_PickingPos();

	if (!m_pImgui_Manager->Get_Instance()->Get_Load())
	{
		Desc.strName = m_pImgui_Manager->Get_Instance()->Get_ObjectName();
		Desc.CreatePos = XMVectorSet(PickingPos.x, PickingPos.y, PickingPos.z, 1.f);
		Desc.ObjectSize = { 1.f, 1.f, 1.f };
		Desc.fRadian = { 0.f };
		 
		if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOOL, strLayerTag,
			TEXT("Prototype_GameObject_IceBlock01"), &Desc)))
			return E_FAIL;

		m_pImgui_Manager->Get_Instance()->Set_Create(false);
		m_iCount++;
	}
	else
	{
		_ulong		dwByte = 0;
		HANDLE		hFile = CreateFile(TEXT("../../Client/Bin/Resources/Map/Test.txt"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
		size_t dataLength;
		wstring Name;
		_float3		Size = { 0.f,0.f,0.f };
		_float      Radian = { 0 };
		_vector		Pos = { XMVectorSet(0.f,0.f,0.f,0.f)};

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

			if (!ReadFile(hFile, &Pos, sizeof(_vector), &dwByte, nullptr))
				break;

			if (!ReadFile(hFile, &Size, sizeof(_float3), &dwByte, nullptr))
				break;

			if (!ReadFile(hFile, &Radian, sizeof(_float), &dwByte, nullptr))
				break;

			wstring targetString = L"IceBlock01";
			if (Name.find(targetString) != wstring::npos)
			{
				wstring extractedString = Name.substr(0, targetString.length());
				Desc.strName = extractedString;
			}
			else
				Desc.strName = L"";
			Desc.CreatePos = Pos;
			Desc.ObjectSize = Size;
			Desc.fRadian = Radian;

			if (Desc.strName == L"IceBlock01")
			{
				if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOOL, strLayerTag,
					TEXT("Prototype_GameObject_IceBlock01"), &Desc)))
					return E_FAIL;
			}
		}
		CloseHandle(hFile);

	}


	Safe_Release(pGameInstance);
	return S_OK;
}

HRESULT CToolMainApp::Ready_Layer_Large_Rock1(const wstring& strLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CLarge_Rock1::OBJECTDESC Desc;

	_float3 PickingPos = m_pImgui_Manager->Get_Instance()->Get_PickingPos();

	if (!m_pImgui_Manager->Get_Instance()->Get_Load())
	{
		Desc.strName = m_pImgui_Manager->Get_Instance()->Get_ObjectName();
		Desc.CreatePos = XMVectorSet(PickingPos.x, PickingPos.y, PickingPos.z, 1.f);
		Desc.ObjectSize = { 1.f, 1.f, 1.f };
		Desc.fRadian = { 0.f };

		if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOOL, strLayerTag,
			TEXT("Prototype_GameObject_Large_Rock1"), &Desc)))
			return E_FAIL;

		m_pImgui_Manager->Get_Instance()->Set_Create(false);
		m_iCount++;
	}
	else
	{
		_ulong		dwByte = 0;
		HANDLE		hFile = CreateFile(TEXT("../../Client/Bin/Resources/Map/Test.txt"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
		size_t dataLength;
		wstring Name;
		_float3		Size = { 0.f,0.f,0.f };
		_float      Radian = { 0 };
		_vector		Pos = { XMVectorSet(0.f,0.f,0.f,0.f) };

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

			if (!ReadFile(hFile, &Pos, sizeof(_vector), &dwByte, nullptr))
				break;

			if (!ReadFile(hFile, &Size, sizeof(_float3), &dwByte, nullptr))
				break;

			if (!ReadFile(hFile, &Radian, sizeof(_float), &dwByte, nullptr))
				break;

			wstring targetString = L"Large_Rock1";
			if (Name.find(targetString) != wstring::npos)
			{
				wstring extractedString = Name.substr(0, targetString.length());
				Desc.strName = extractedString;
			}
			else
				Desc.strName = L"";
			Desc.CreatePos = Pos;
			Desc.ObjectSize = Size;
			Desc.fRadian = Radian;

			if (Desc.strName == L"Large_Rock1")
			{
				if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOOL, strLayerTag,
					TEXT("Prototype_GameObject_Large_Rock1"), &Desc)))
					return E_FAIL;
			}
		}
		CloseHandle(hFile);

	}


	Safe_Release(pGameInstance);
	return S_OK;
}

HRESULT CToolMainApp::Ready_Layer_Bench(const wstring& strLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CBench::OBJECTDESC Desc;

	_float3 PickingPos = m_pImgui_Manager->Get_Instance()->Get_PickingPos();

	if (!m_pImgui_Manager->Get_Instance()->Get_Load())
	{
		Desc.strName = m_pImgui_Manager->Get_Instance()->Get_ObjectName();
		Desc.CreatePos = XMVectorSet(PickingPos.x, PickingPos.y, PickingPos.z, 1.f);
		Desc.ObjectSize = { 1.f, 1.f, 1.f };
		Desc.fRadian = { 0.f };

		if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOOL, strLayerTag,
			TEXT("Prototype_GameObject_Bench"), &Desc)))
			return E_FAIL;

		m_pImgui_Manager->Get_Instance()->Set_Create(false);
		m_iCount++;
	}
	else
	{
		_ulong		dwByte = 0;
		HANDLE		hFile = CreateFile(TEXT("../../Client/Bin/Resources/Map/Test.txt"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
		size_t dataLength;
		wstring Name;
		_float3		Size = { 0.f,0.f,0.f };
		_float      Radian = { 0 };
		_vector		Pos = { XMVectorSet(0.f,0.f,0.f,0.f) };

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

			if (!ReadFile(hFile, &Pos, sizeof(_vector), &dwByte, nullptr))
				break;

			if (!ReadFile(hFile, &Size, sizeof(_float3), &dwByte, nullptr))
				break;

			if (!ReadFile(hFile, &Radian, sizeof(_float), &dwByte, nullptr))
				break;

			wstring targetString = L"obj_Bench";
			size_t foundPos = Name.find(targetString);
			if (foundPos != wstring::npos)
			{
				/*wstring extractedString = Name.substr(foundPos, targetString.length());*/
				Desc.strName = targetString;
			}
			else
				Desc.strName = L"";
			Desc.CreatePos = Pos;
			Desc.ObjectSize = Size;
			Desc.fRadian = Radian;

			if (Desc.strName == L"obj_Bench")
			{
				if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOOL, strLayerTag,
					TEXT("Prototype_GameObject_Bench"), &Desc)))
					return E_FAIL;
			}
		}
		CloseHandle(hFile);

	}


	Safe_Release(pGameInstance);
	return S_OK;
}

HRESULT CToolMainApp::Ready_Layer_CircularSupport(const wstring& strLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CCircularSupport::OBJECTDESC Desc;

	_float3 PickingPos = m_pImgui_Manager->Get_Instance()->Get_PickingPos();

	if (!m_pImgui_Manager->Get_Instance()->Get_Load())
	{
		Desc.strName = m_pImgui_Manager->Get_Instance()->Get_ObjectName();
		Desc.CreatePos = XMVectorSet(PickingPos.x, PickingPos.y, PickingPos.z, 1.f);
		Desc.ObjectSize = { 1.f, 1.f, 1.f };
		Desc.fRadian = { 0.f };

		if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOOL, strLayerTag,
			TEXT("Prototype_GameObject_CircularSupport"), &Desc)))
			return E_FAIL;

		m_pImgui_Manager->Get_Instance()->Set_Create(false);
		m_iCount++;
	}
	else
	{
		_ulong		dwByte = 0;
		HANDLE		hFile = CreateFile(TEXT("../../Client/Bin/Resources/Map/Test.txt"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
		size_t dataLength;
		wstring Name;
		_float3		Size = { 0.f,0.f,0.f };
		_float      Radian = { 0 };
		_vector		Pos = { XMVectorSet(0.f,0.f,0.f,0.f) };

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

			if (!ReadFile(hFile, &Pos, sizeof(_vector), &dwByte, nullptr))
				break;

			if (!ReadFile(hFile, &Size, sizeof(_float3), &dwByte, nullptr))
				break;

			if (!ReadFile(hFile, &Radian, sizeof(_float), &dwByte, nullptr))
				break;

			wstring targetString = L"obj_CircularSupport";
			if (Name.find(targetString) != wstring::npos)
			{
				wstring extractedString = Name.substr(0, targetString.length());
				Desc.strName = extractedString;
			}
			else
				Desc.strName = L"";
			Desc.CreatePos = Pos;
			Desc.ObjectSize = Size;
			Desc.fRadian = Radian;

			if (Desc.strName == L"obj_CircularSupport")
			{
				if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOOL, strLayerTag,
					TEXT("Prototype_GameObject_CircularSupport"), &Desc)))
					return E_FAIL;
			}
		}
		CloseHandle(hFile);

	}


	Safe_Release(pGameInstance);
	return S_OK;
}

HRESULT CToolMainApp::Ready_Layer_Container(const wstring& strLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CContainer::OBJECTDESC Desc;

	_float3 PickingPos = m_pImgui_Manager->Get_Instance()->Get_PickingPos();

	if (!m_pImgui_Manager->Get_Instance()->Get_Load())
	{
		Desc.strName = m_pImgui_Manager->Get_Instance()->Get_ObjectName();
		Desc.CreatePos = XMVectorSet(PickingPos.x, PickingPos.y, PickingPos.z, 1.f);
		Desc.ObjectSize = { 1.f, 1.f, 1.f };
		Desc.fRadian = { 0.f };

		if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOOL, strLayerTag,
			TEXT("Prototype_GameObject_Container"), &Desc)))
			return E_FAIL;

		m_pImgui_Manager->Get_Instance()->Set_Create(false);
		m_iCount++;
	}
	else
	{
		_ulong		dwByte = 0;
		HANDLE		hFile = CreateFile(TEXT("../../Client/Bin/Resources/Map/Container.txt"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
		size_t dataLength;
		wstring Name;
		_float3		Size = { 0.f,0.f,0.f };
		_float      Radian = { 0 };
		_vector		Pos = { XMVectorSet(0.f,0.f,0.f,0.f) };

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

			if (!ReadFile(hFile, &Pos, sizeof(_vector), &dwByte, nullptr))
				break;

			if (!ReadFile(hFile, &Size, sizeof(_float3), &dwByte, nullptr))
				break;

			if (!ReadFile(hFile, &Radian, sizeof(_float), &dwByte, nullptr))
				break;

			wstring targetString = L"obj_Container";
			if (Name.find(targetString) != wstring::npos)
			{
				wstring extractedString = Name.substr(0, targetString.length());
				Desc.strName = extractedString;
			}
			else
				Desc.strName = L"";
			Desc.CreatePos = Pos;
			Desc.ObjectSize = Size;
			Desc.fRadian = Radian;

			if (Desc.strName == L"obj_Container")
			{
				if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOOL, strLayerTag,
					TEXT("Prototype_GameObject_Container"), &Desc)))
					return E_FAIL;
			}
		}
		CloseHandle(hFile);

	}


	Safe_Release(pGameInstance);
	return S_OK;
}

HRESULT CToolMainApp::Ready_Layer_Crate(const wstring& strLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CCrate::OBJECTDESC Desc;

	_float3 PickingPos = m_pImgui_Manager->Get_Instance()->Get_PickingPos();

	if (!m_pImgui_Manager->Get_Instance()->Get_Load())
	{
		Desc.strName = m_pImgui_Manager->Get_Instance()->Get_ObjectName();
		Desc.CreatePos = XMVectorSet(PickingPos.x, PickingPos.y, PickingPos.z, 1.f);
		Desc.ObjectSize = { 1.f, 1.f, 1.f };
		Desc.fRadian = { 0.f };

		if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOOL, strLayerTag,
			TEXT("Prototype_GameObject_Crate"), &Desc)))
			return E_FAIL;

		m_pImgui_Manager->Get_Instance()->Set_Create(false);
		m_iCount++;
	}
	else
	{
		_ulong		dwByte = 0;
		HANDLE		hFile = CreateFile(TEXT("../../Client/Bin/Resources/Map/Test.txt"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
		size_t dataLength;
		wstring Name;
		_float3		Size = { 0.f,0.f,0.f };
		_float      Radian = { 0 };
		_vector		Pos = { XMVectorSet(0.f,0.f,0.f,0.f) };

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

			if (!ReadFile(hFile, &Pos, sizeof(_vector), &dwByte, nullptr))
				break;

			if (!ReadFile(hFile, &Size, sizeof(_float3), &dwByte, nullptr))
				break;

			if (!ReadFile(hFile, &Radian, sizeof(_float), &dwByte, nullptr))
				break;

			wstring targetString = L"obj_Crate";
			if (Name.find(targetString) != wstring::npos)
			{
				wstring extractedString = Name.substr(0, targetString.length());
				Desc.strName = extractedString;
			}
			else
				Desc.strName = L"";
			Desc.CreatePos = Pos;
			Desc.ObjectSize = Size;
			Desc.fRadian = Radian;

			if (Desc.strName == L"obj_Crate")
			{
				if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOOL, strLayerTag,
					TEXT("Prototype_GameObject_Crate"), &Desc)))
					return E_FAIL;
			}
		}
		CloseHandle(hFile);

	}


	Safe_Release(pGameInstance);
	return S_OK;
}

HRESULT CToolMainApp::Ready_Layer_CrateSet(const wstring& strLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CCrateSet::OBJECTDESC Desc;

	_float3 PickingPos = m_pImgui_Manager->Get_Instance()->Get_PickingPos();

	if (!m_pImgui_Manager->Get_Instance()->Get_Load())
	{
		Desc.strName = m_pImgui_Manager->Get_Instance()->Get_ObjectName();
		Desc.CreatePos = XMVectorSet(PickingPos.x, PickingPos.y, PickingPos.z, 1.f);
		Desc.ObjectSize = { 1.f, 1.f, 1.f };
		Desc.fRadian = { 0.f };

		if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOOL, strLayerTag,
			TEXT("Prototype_GameObject_CrateSet"), &Desc)))
			return E_FAIL;

		m_pImgui_Manager->Get_Instance()->Set_Create(false);
		m_iCount++;
	}
	else
	{
		_ulong		dwByte = 0;
		HANDLE		hFile = CreateFile(TEXT("../../Client/Bin/Resources/Map/Test.txt"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
		size_t dataLength;
		wstring Name;
		_float3		Size = { 0.f,0.f,0.f };
		_float      Radian = { 0 };
		_vector		Pos = { XMVectorSet(0.f,0.f,0.f,0.f) };

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

			if (!ReadFile(hFile, &Pos, sizeof(_vector), &dwByte, nullptr))
				break;

			if (!ReadFile(hFile, &Size, sizeof(_float3), &dwByte, nullptr))
				break;

			if (!ReadFile(hFile, &Radian, sizeof(_float), &dwByte, nullptr))
				break;

			wstring targetString = L"obj_CrateSet";
			if (Name.find(targetString) != wstring::npos)
			{
				wstring extractedString = Name.substr(0, targetString.length());
				Desc.strName = extractedString;
			}
			else
				Desc.strName = L"";
			Desc.CreatePos = Pos;
			Desc.ObjectSize = Size;
			Desc.fRadian = Radian;

			if (Desc.strName == L"obj_CrateSet")
			{
				if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOOL, strLayerTag,
					TEXT("Prototype_GameObject_CrateSet"), &Desc)))
					return E_FAIL;
			}
		}
		CloseHandle(hFile);

	}


	Safe_Release(pGameInstance);
	return S_OK;
}

HRESULT CToolMainApp::Ready_Layer_House(const wstring& strLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CHouse::OBJECTDESC Desc;

	_float3 PickingPos = m_pImgui_Manager->Get_Instance()->Get_PickingPos();

	if (!m_pImgui_Manager->Get_Instance()->Get_Load())
	{
		Desc.strName = m_pImgui_Manager->Get_Instance()->Get_ObjectName();
		Desc.CreatePos = XMVectorSet(PickingPos.x, PickingPos.y, PickingPos.z, 1.f);
		Desc.ObjectSize = { 1.f, 1.f, 1.f };
		Desc.fRadian = { 0.f };

		if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOOL, strLayerTag,
			TEXT("Prototype_GameObject_House"), &Desc)))
			return E_FAIL;

		m_pImgui_Manager->Get_Instance()->Set_Create(false);
		m_iCount++;
	}
	else
	{
		_ulong		dwByte = 0;
		HANDLE		hFile = CreateFile(TEXT("../../Client/Bin/Resources/Map/Town.txt"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
		size_t dataLength;
		wstring Name;
		_float3		Size = { 0.f,0.f,0.f };
		_float      Radian = { 0 };
		_vector		Pos = { XMVectorSet(0.f,0.f,0.f,0.f) };

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

			if (!ReadFile(hFile, &Pos, sizeof(_vector), &dwByte, nullptr))
				break;

			if (!ReadFile(hFile, &Size, sizeof(_float3), &dwByte, nullptr))
				break;

			if (!ReadFile(hFile, &Radian, sizeof(_float), &dwByte, nullptr))
				break;

			wstring targetString = L"obj_House";
			if (Name.find(targetString) != wstring::npos)
			{
				size_t targetLength = targetString.length();
				wstring extractedString = Name.substr(0, targetLength);
				Desc.strName = extractedString;
			}
			else
				Desc.strName = L"";
			Desc.CreatePos = Pos;
			Desc.ObjectSize = Size;
			Desc.fRadian = Radian;

			if (Desc.strName == L"obj_House")
			{
				if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOOL, strLayerTag,
					TEXT("Prototype_GameObject_House"), &Desc)))
					return E_FAIL;
			}
		}
		CloseHandle(hFile);

	}


	Safe_Release(pGameInstance);
	return S_OK;
}

HRESULT CToolMainApp::Ready_Layer_Rock1(const wstring& strLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CRock1::OBJECTDESC Desc;

	_float3 PickingPos = m_pImgui_Manager->Get_Instance()->Get_PickingPos();

	if (!m_pImgui_Manager->Get_Instance()->Get_Load())
	{
		Desc.strName = m_pImgui_Manager->Get_Instance()->Get_ObjectName();
		Desc.CreatePos = XMVectorSet(PickingPos.x, PickingPos.y, PickingPos.z, 1.f);
		Desc.ObjectSize = { 1.f, 1.f, 1.f };
		Desc.fRadian = { 0.f };

		if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOOL, strLayerTag,
			TEXT("Prototype_GameObject_Rock1"), &Desc)))
			return E_FAIL;

		m_pImgui_Manager->Get_Instance()->Set_Create(false);
		m_iCount++;
	}
	else
	{
		_ulong		dwByte = 0;
		HANDLE		hFile = CreateFile(TEXT("../../Client/Bin/Resources/Map/Test.txt"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
		size_t dataLength;
		wstring Name;
		_float3		Size = { 0.f,0.f,0.f };
		_float      Radian = { 0 };
		_vector		Pos = { XMVectorSet(0.f,0.f,0.f,0.f) };

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

			if (!ReadFile(hFile, &Pos, sizeof(_vector), &dwByte, nullptr))
				break;

			if (!ReadFile(hFile, &Size, sizeof(_float3), &dwByte, nullptr))
				break;

			if (!ReadFile(hFile, &Radian, sizeof(_float), &dwByte, nullptr))
				break;

			wstring targetString = L"obj_Rock1";
			if (Name.find(targetString) != wstring::npos)
			{
				wstring extractedString = Name.substr(0, targetString.length());
				Desc.strName = extractedString;
			}
			else
				Desc.strName = L"";
			Desc.CreatePos = Pos;
			Desc.ObjectSize = Size;
			Desc.fRadian = Radian;

			if (Desc.strName == L"obj_Rock1")
			{
				if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOOL, strLayerTag,
					TEXT("Prototype_GameObject_Rock1"), &Desc)))
					return E_FAIL;
			}
		}
		CloseHandle(hFile);

	}


	Safe_Release(pGameInstance);
	return S_OK;
}

HRESULT CToolMainApp::Ready_Layer_TunnelDoor(const wstring& strLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CTunnelDoor::OBJECTDESC Desc;

	_float3 PickingPos = m_pImgui_Manager->Get_Instance()->Get_PickingPos();

	if (!m_pImgui_Manager->Get_Instance()->Get_Load())
	{
		Desc.strName = m_pImgui_Manager->Get_Instance()->Get_ObjectName();
		Desc.CreatePos = XMVectorSet(PickingPos.x, PickingPos.y, PickingPos.z, 1.f);
		Desc.ObjectSize = { 1.f, 1.f, 1.f };
		Desc.fRadian = { 0.f };

		if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOOL, strLayerTag,
			TEXT("Prototype_GameObject_TunnelDoor"), &Desc)))
			return E_FAIL;

		m_pImgui_Manager->Get_Instance()->Set_Create(false);
		m_iCount++;
	}
	else
	{
		_ulong		dwByte = 0;
		HANDLE		hFile = CreateFile(TEXT("../../Client/Bin/Resources/Map/Test.txt"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
		size_t dataLength;
		wstring Name;
		_float3		Size = { 0.f,0.f,0.f };
		_float      Radian = { 0 };
		_vector		Pos = { XMVectorSet(0.f,0.f,0.f,0.f) };

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

			if (!ReadFile(hFile, &Pos, sizeof(_vector), &dwByte, nullptr))
				break;

			if (!ReadFile(hFile, &Size, sizeof(_float3), &dwByte, nullptr))
				break;

			if (!ReadFile(hFile, &Radian, sizeof(_float), &dwByte, nullptr))
				break;

			wstring targetString = L"obj_TunnelDoor";
			if (Name.find(targetString) != wstring::npos)
			{
				wstring extractedString = Name.substr(0, targetString.length());
				Desc.strName = extractedString;
			}
			else
				Desc.strName = L"";
			Desc.CreatePos = Pos;
			Desc.ObjectSize = Size;
			Desc.fRadian = Radian;

			if (Desc.strName == L"obj_TunnelDoor")
			{
				if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOOL, strLayerTag,
					TEXT("Prototype_GameObject_TunnelDoor"), &Desc)))
					return E_FAIL;
			}
		}
		CloseHandle(hFile);

	}


	Safe_Release(pGameInstance);
	return S_OK;
}

HRESULT CToolMainApp::Ready_Layer_WaterTower(const wstring& strLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CWaterTower::OBJECTDESC Desc;

	_float3 PickingPos = m_pImgui_Manager->Get_Instance()->Get_PickingPos();

	if (!m_pImgui_Manager->Get_Instance()->Get_Load())
	{
		Desc.strName = m_pImgui_Manager->Get_Instance()->Get_ObjectName();
		Desc.CreatePos = XMVectorSet(PickingPos.x, PickingPos.y, PickingPos.z, 1.f);
		Desc.ObjectSize = { 1.f, 1.f, 1.f };
		Desc.fRadian = { 0.f };

		if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOOL, strLayerTag,
			TEXT("Prototype_GameObject_WaterTower"), &Desc)))
			return E_FAIL;

		m_pImgui_Manager->Get_Instance()->Set_Create(false);
		m_iCount++;
	}
	else
	{
		_ulong		dwByte = 0;
		HANDLE		hFile = CreateFile(TEXT("../../Client/Bin/Resources/Map/Town.txt"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
		size_t dataLength;
		wstring Name;
		_float3		Size = { 0.f,0.f,0.f };
		_float      Radian = { 0 };
		_vector		Pos = { XMVectorSet(0.f,0.f,0.f,0.f) };

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

			if (!ReadFile(hFile, &Pos, sizeof(_vector), &dwByte, nullptr))
				break;

			if (!ReadFile(hFile, &Size, sizeof(_float3), &dwByte, nullptr))
				break;

			if (!ReadFile(hFile, &Radian, sizeof(_float), &dwByte, nullptr))
				break;

			wstring targetString = L"obj_WaterTower";
			if (Name.find(targetString) != wstring::npos)
			{
				wstring extractedString = Name.substr(0, targetString.length());
				Desc.strName = extractedString;
			}
			else
				Desc.strName = L"";
			Desc.CreatePos = Pos;
			Desc.ObjectSize = Size;
			Desc.fRadian = Radian;

			if (Desc.strName == L"obj_WaterTower")
			{
				if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOOL, strLayerTag,
					TEXT("Prototype_GameObject_WaterTower"), &Desc)))
					return E_FAIL;
			}
		}
		CloseHandle(hFile);

	}


	Safe_Release(pGameInstance);
	return S_OK;
}

HRESULT CToolMainApp::Ready_Layer_Pillar(const wstring& strLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CPillar::OBJECTDESC Desc;

	_float3 PickingPos = m_pImgui_Manager->Get_Instance()->Get_PickingPos();

	if (!m_pImgui_Manager->Get_Instance()->Get_Load())
	{
		Desc.strName = m_pImgui_Manager->Get_Instance()->Get_ObjectName();
		Desc.CreatePos = XMVectorSet(PickingPos.x, PickingPos.y, PickingPos.z, 1.f);
		Desc.ObjectSize = { 1.f, 1.f, 1.f };
		Desc.fRadian = { 0.f };

		if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOOL, strLayerTag,
			TEXT("Prototype_GameObject_Pillar"), &Desc)))
			return E_FAIL;

		m_pImgui_Manager->Get_Instance()->Set_Create(false);
		m_iCount++;
	}
	else
	{
		_ulong		dwByte = 0;
		HANDLE		hFile = CreateFile(TEXT("../../Client/Bin/Resources/Map/Town.txt"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
		size_t dataLength;
		wstring Name;
		_float3		Size = { 0.f,0.f,0.f };
		_float      Radian = { 0 };
		_vector		Pos = { XMVectorSet(0.f,0.f,0.f,0.f) };

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

			if (!ReadFile(hFile, &Pos, sizeof(_vector), &dwByte, nullptr))
				break;

			if (!ReadFile(hFile, &Size, sizeof(_float3), &dwByte, nullptr))
				break;

			if (!ReadFile(hFile, &Radian, sizeof(_float), &dwByte, nullptr))
				break;

			wstring targetString = L"obj_Pillar";
			if (Name.find(targetString) != wstring::npos)
			{
				wstring extractedString = Name.substr(0, targetString.length());
				Desc.strName = extractedString;
			}
			else
				Desc.strName = L"";
			Desc.CreatePos = Pos;
			Desc.ObjectSize = Size;
			Desc.fRadian = Radian;

			if (Desc.strName == L"obj_Pillar")
			{
				if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOOL, strLayerTag,
					TEXT("Prototype_GameObject_Pillar"), &Desc)))
					return E_FAIL;
			}
		}
		CloseHandle(hFile);

	}


	Safe_Release(pGameInstance);
	return S_OK;
}

HRESULT CToolMainApp::Ready_Layer_Yurt(const wstring& strLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CYurt::OBJECTDESC Desc;

	_float3 PickingPos = m_pImgui_Manager->Get_Instance()->Get_PickingPos();

	if (!m_pImgui_Manager->Get_Instance()->Get_Load())
	{
		Desc.strName = m_pImgui_Manager->Get_Instance()->Get_ObjectName();
		Desc.CreatePos = XMVectorSet(PickingPos.x, PickingPos.y, PickingPos.z, 1.f);
		Desc.ObjectSize = { 1.f, 1.f, 1.f };
		Desc.fRadian = { 0.f };

		if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOOL, strLayerTag,
			TEXT("Prototype_GameObject_Yurt"), &Desc)))
			return E_FAIL;

		m_pImgui_Manager->Get_Instance()->Set_Create(false);
		m_iCount++;
	}
	else
	{
		_ulong		dwByte = 0;
		HANDLE		hFile = CreateFile(TEXT("../../Client/Bin/Resources/Map/Town.txt"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
		size_t dataLength;
		wstring Name;
		_float3		Size = { 0.f,0.f,0.f };
		_float      Radian = { 0 };
		_vector		Pos = { XMVectorSet(0.f,0.f,0.f,0.f) };

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

			if (!ReadFile(hFile, &Pos, sizeof(_vector), &dwByte, nullptr))
				break;

			if (!ReadFile(hFile, &Size, sizeof(_float3), &dwByte, nullptr))
				break;

			if (!ReadFile(hFile, &Radian, sizeof(_float), &dwByte, nullptr))
				break;

			wstring targetString = L"obj_Yurt";
			if (Name.find(targetString) != wstring::npos)
			{
				wstring extractedString = Name.substr(0, targetString.length());
				Desc.strName = extractedString;
			}
			else
				Desc.strName = L"";
			Desc.CreatePos = Pos;
			Desc.ObjectSize = Size;
			Desc.fRadian = Radian;

			if (Desc.strName == L"obj_Yurt")
			{
				if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOOL, strLayerTag,
					TEXT("Prototype_GameObject_Yurt"), &Desc)))
					return E_FAIL;
			}
		}
		CloseHandle(hFile);

	}


	Safe_Release(pGameInstance);
	return S_OK;
}

HRESULT CToolMainApp::Ready_Layer_Yurt2(const wstring& strLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CYurt2::OBJECTDESC Desc;

	_float3 PickingPos = m_pImgui_Manager->Get_Instance()->Get_PickingPos();

	if (!m_pImgui_Manager->Get_Instance()->Get_Load())
	{
		Desc.strName = m_pImgui_Manager->Get_Instance()->Get_ObjectName();
		Desc.CreatePos = XMVectorSet(PickingPos.x, PickingPos.y, PickingPos.z, 1.f);
		Desc.ObjectSize = { 1.f, 1.f, 1.f };
		Desc.fRadian = { 0.f };

		if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOOL, strLayerTag,
			TEXT("Prototype_GameObject_Yurt2"), &Desc)))
			return E_FAIL;

		m_pImgui_Manager->Get_Instance()->Set_Create(false);
		m_iCount++;
	}
	else
	{
		_ulong		dwByte = 0;
		HANDLE		hFile = CreateFile(TEXT("../../Client/Bin/Resources/Map/Town.txt"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
		size_t dataLength;
		wstring Name;
		_float3		Size = { 0.f,0.f,0.f };
		_float      Radian = { 0 };
		_vector		Pos = { XMVectorSet(0.f,0.f,0.f,0.f) };

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

			if (!ReadFile(hFile, &Pos, sizeof(_vector), &dwByte, nullptr))
				break;

			if (!ReadFile(hFile, &Size, sizeof(_float3), &dwByte, nullptr))
				break;

			if (!ReadFile(hFile, &Radian, sizeof(_float), &dwByte, nullptr))
				break;

			wstring targetString = L"obj_Yurt2";
			if (Name.find(targetString) != wstring::npos)
			{
				wstring extractedString = Name.substr(0, targetString.length());
				Desc.strName = extractedString;
			}
			else
				Desc.strName = L"";
			Desc.CreatePos = Pos;
			Desc.ObjectSize = Size;
			Desc.fRadian = Radian;

			if (Desc.strName == L"obj_Yurt2")
			{
				if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOOL, strLayerTag,
					TEXT("Prototype_GameObject_Yurt2"), &Desc)))
					return E_FAIL;
			}
		}
		CloseHandle(hFile);

	}


	Safe_Release(pGameInstance);
	return S_OK;
}

HRESULT CToolMainApp::Ready_Layer_MetalBeam(const wstring& strLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CMetalBeam::OBJECTDESC Desc;

	_float3 PickingPos = m_pImgui_Manager->Get_Instance()->Get_PickingPos();

	if (!m_pImgui_Manager->Get_Instance()->Get_Load())
	{
		Desc.strName = m_pImgui_Manager->Get_Instance()->Get_ObjectName();
		Desc.CreatePos = XMVectorSet(PickingPos.x, PickingPos.y, PickingPos.z, 1.f);
		Desc.ObjectSize = { 1.f, 1.f, 1.f };
		Desc.fRadian = { 0.f };

		if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOOL, strLayerTag,
			TEXT("Prototype_GameObject_MetalBeam"), &Desc)))
			return E_FAIL;

		m_pImgui_Manager->Get_Instance()->Set_Create(false);
		m_iCount++;
	}
	else
	{
		_ulong		dwByte = 0;
		HANDLE		hFile = CreateFile(TEXT("../../Client/Bin/Resources/Map/Town.txt"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
		size_t dataLength;
		wstring Name;
		_float3		Size = { 0.f,0.f,0.f };
		_float      Radian = { 0 };
		_vector		Pos = { XMVectorSet(0.f,0.f,0.f,0.f) };

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

			if (!ReadFile(hFile, &Pos, sizeof(_vector), &dwByte, nullptr))
				break;

			if (!ReadFile(hFile, &Size, sizeof(_float3), &dwByte, nullptr))
				break;

			if (!ReadFile(hFile, &Radian, sizeof(_float), &dwByte, nullptr))
				break;

			wstring targetString = L"obj_MetalBeam";
			if (Name.find(targetString) != wstring::npos)
			{
				wstring extractedString = Name.substr(0, targetString.length());
				Desc.strName = extractedString;
			}
			else
				Desc.strName = L"";
			Desc.CreatePos = Pos;
			Desc.ObjectSize = Size;
			Desc.fRadian = Radian;

			if (Desc.strName == L"obj_MetalBeam")
			{
				if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOOL, strLayerTag,
					TEXT("Prototype_GameObject_MetalBeam"), &Desc)))
					return E_FAIL;
			}
		}
		CloseHandle(hFile);

	}


	Safe_Release(pGameInstance);
	return S_OK;
}

HRESULT CToolMainApp::Ready_Layer_MetalLadder(const wstring& strLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CMetalLadder::OBJECTDESC Desc;

	_float3 PickingPos = m_pImgui_Manager->Get_Instance()->Get_PickingPos();

	if (!m_pImgui_Manager->Get_Instance()->Get_Load())
	{
		Desc.strName = m_pImgui_Manager->Get_Instance()->Get_ObjectName();
		Desc.CreatePos = XMVectorSet(PickingPos.x, PickingPos.y, PickingPos.z, 1.f);
		Desc.ObjectSize = { 1.f, 1.f, 1.f };
		Desc.fRadian = { 0.f };

		if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOOL, strLayerTag,
			TEXT("Prototype_GameObject_MetalLadder"), &Desc)))
			return E_FAIL;

		m_pImgui_Manager->Get_Instance()->Set_Create(false);
		m_iCount++;
	}
	else
	{
		_ulong		dwByte = 0;
		HANDLE		hFile = CreateFile(TEXT("../../Client/Bin/Resources/Map/Town.txt"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
		size_t dataLength;
		wstring Name;
		_float3		Size = { 0.f,0.f,0.f };
		_float      Radian = { 0 };
		_vector		Pos = { XMVectorSet(0.f,0.f,0.f,0.f) };

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

			if (!ReadFile(hFile, &Pos, sizeof(_vector), &dwByte, nullptr))
				break;

			if (!ReadFile(hFile, &Size, sizeof(_float3), &dwByte, nullptr))
				break;

			if (!ReadFile(hFile, &Radian, sizeof(_float), &dwByte, nullptr))
				break;

			wstring targetString = L"obj_MetalLadder";
			if (Name.find(targetString) != wstring::npos)
			{
				wstring extractedString = Name.substr(0, targetString.length());
				Desc.strName = extractedString;
			}
			else
				Desc.strName = L"";
			Desc.CreatePos = Pos;
			Desc.ObjectSize = Size;
			Desc.fRadian = Radian;

			if (Desc.strName == L"obj_MetalLadder")
			{
				if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOOL, strLayerTag,
					TEXT("Prototype_GameObject_MetalLadder"), &Desc)))
					return E_FAIL;
			}
		}
		CloseHandle(hFile);

	}


	Safe_Release(pGameInstance);
	return S_OK;
}

HRESULT CToolMainApp::Ready_Layer_DamWall(const wstring& strLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CDamWall::OBJECTDESC Desc;

	_float3 PickingPos = m_pImgui_Manager->Get_Instance()->Get_PickingPos();

	if (!m_pImgui_Manager->Get_Instance()->Get_Load())
	{
		Desc.strName = m_pImgui_Manager->Get_Instance()->Get_ObjectName();
		Desc.CreatePos = XMVectorSet(PickingPos.x, PickingPos.y, PickingPos.z, 1.f);
		Desc.ObjectSize = { 1.f, 1.f, 1.f };
		Desc.fRadian = { 0.f };

		if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOOL, strLayerTag,
			TEXT("Prototype_GameObject_DamWall"), &Desc)))
			return E_FAIL;

		m_pImgui_Manager->Get_Instance()->Set_Create(false);
		m_iCount++;
	}
	else
	{
		_ulong		dwByte = 0;
		HANDLE		hFile = CreateFile(TEXT("../../Client/Bin/Resources/Map/Town.txt"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
		size_t dataLength;
		wstring Name;
		_float3		Size = { 0.f,0.f,0.f };
		_float      Radian = { 0 };
		_vector		Pos = { XMVectorSet(0.f,0.f,0.f,0.f) };

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

			if (!ReadFile(hFile, &Pos, sizeof(_vector), &dwByte, nullptr))
				break;

			if (!ReadFile(hFile, &Size, sizeof(_float3), &dwByte, nullptr))
				break;

			if (!ReadFile(hFile, &Radian, sizeof(_float), &dwByte, nullptr))
				break;

			wstring targetString = L"obj_DamWall";
			if (Name.find(targetString) != wstring::npos)
			{
				wstring extractedString = Name.substr(0, targetString.length());
				Desc.strName = extractedString;
			}
			else
				Desc.strName = L"";
			Desc.CreatePos = Pos;
			Desc.ObjectSize = Size;
			Desc.fRadian = Radian;

			if (Desc.strName == L"obj_DamWall")
			{
				if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOOL, strLayerTag,
					TEXT("Prototype_GameObject_DamWall"), &Desc)))
					return E_FAIL;
			}
		}
		CloseHandle(hFile);

	}


	Safe_Release(pGameInstance);
	return S_OK;
}

HRESULT CToolMainApp::Ready_Layer_BarCounter(const wstring& strLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CBarCounter::OBJECTDESC Desc;

	_float3 PickingPos = m_pImgui_Manager->Get_Instance()->Get_PickingPos();

	if (!m_pImgui_Manager->Get_Instance()->Get_Load())
	{
		Desc.strName = m_pImgui_Manager->Get_Instance()->Get_ObjectName();
		Desc.CreatePos = XMVectorSet(PickingPos.x, PickingPos.y, PickingPos.z, 1.f);
		Desc.ObjectSize = { 1.f, 1.f, 1.f };
		Desc.fRadian = { 0.f };

		if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOOL, strLayerTag,
			TEXT("Prototype_GameObject_BarCounter"), &Desc)))
			return E_FAIL;

		m_pImgui_Manager->Get_Instance()->Set_Create(false);
		m_iCount++;
	}
	else
	{
		_ulong		dwByte = 0;
		HANDLE		hFile = CreateFile(TEXT("../../Client/Bin/Resources/Map/Test.txt"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
		size_t dataLength;
		wstring Name;
		_float3		Size = { 0.f,0.f,0.f };
		_float      Radian = { 0 };
		_vector		Pos = { XMVectorSet(0.f,0.f,0.f,0.f) };

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

			if (!ReadFile(hFile, &Pos, sizeof(_vector), &dwByte, nullptr))
				break;

			if (!ReadFile(hFile, &Size, sizeof(_float3), &dwByte, nullptr))
				break;

			if (!ReadFile(hFile, &Radian, sizeof(_float), &dwByte, nullptr))
				break;

			wstring targetString = L"obj_BarCounter";
			if (Name.find(targetString) != wstring::npos)
			{
				wstring extractedString = Name.substr(0, targetString.length());
				Desc.strName = extractedString;
			}
			else
				Desc.strName = L"";
			Desc.CreatePos = Pos;
			Desc.ObjectSize = Size;
			Desc.fRadian = Radian;

			if (Desc.strName == L"obj_BarCounter")
			{
				if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOOL, strLayerTag,
					TEXT("Prototype_GameObject_BarCounter"), &Desc)))
					return E_FAIL;
			}
		}
		CloseHandle(hFile);

	}


	Safe_Release(pGameInstance);
	return S_OK;
}

HRESULT CToolMainApp::Ready_Layer_BedGarbage(const wstring& strLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CBedGarbage::OBJECTDESC Desc;

	_float3 PickingPos = m_pImgui_Manager->Get_Instance()->Get_PickingPos();

	if (!m_pImgui_Manager->Get_Instance()->Get_Load())
	{
		Desc.strName = m_pImgui_Manager->Get_Instance()->Get_ObjectName();
		Desc.CreatePos = XMVectorSet(PickingPos.x, PickingPos.y, PickingPos.z, 1.f);
		Desc.ObjectSize = { 1.f, 1.f, 1.f };
		Desc.fRadian = { 0.f };

		if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOOL, strLayerTag,
			TEXT("Prototype_GameObject_BedGarbage"), &Desc)))
			return E_FAIL;

		m_pImgui_Manager->Get_Instance()->Set_Create(false);
		m_iCount++;
	}
	else
	{
		_ulong		dwByte = 0;
		HANDLE		hFile = CreateFile(TEXT("../../Client/Bin/Resources/Map/Test.txt"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
		size_t dataLength;
		wstring Name;
		_float3		Size = { 0.f,0.f,0.f };
		_float      Radian = { 0 };
		_vector		Pos = { XMVectorSet(0.f,0.f,0.f,0.f) };

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

			if (!ReadFile(hFile, &Pos, sizeof(_vector), &dwByte, nullptr))
				break;

			if (!ReadFile(hFile, &Size, sizeof(_float3), &dwByte, nullptr))
				break;

			if (!ReadFile(hFile, &Radian, sizeof(_float), &dwByte, nullptr))
				break;

			wstring targetString = L"obj_BedGarbage";
			if (Name.find(targetString) != wstring::npos)
			{
				wstring extractedString = Name.substr(0, targetString.length());
				Desc.strName = extractedString;
			}
			else
				Desc.strName = L"";
			Desc.CreatePos = Pos;
			Desc.ObjectSize = Size;
			Desc.fRadian = Radian;

			if (Desc.strName == L"obj_BedGarbage")
			{
				if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOOL, strLayerTag,
					TEXT("Prototype_GameObject_BedGarbage"), &Desc)))
					return E_FAIL;
			}
		}
		CloseHandle(hFile);

	}


	Safe_Release(pGameInstance);
	return S_OK;
}

HRESULT CToolMainApp::Ready_Layer_Dropship(const wstring& strLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CDropship::OBJECTDESC Desc;

	_float3 PickingPos = m_pImgui_Manager->Get_Instance()->Get_PickingPos();

	if (!m_pImgui_Manager->Get_Instance()->Get_Load())
	{
		Desc.strName = m_pImgui_Manager->Get_Instance()->Get_ObjectName();
		Desc.CreatePos = XMVectorSet(PickingPos.x, PickingPos.y, PickingPos.z, 1.f);
		Desc.ObjectSize = { 1.f, 1.f, 1.f };
		Desc.fRadian = { 0.f };

		if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOOL, strLayerTag,
			TEXT("Prototype_GameObject_Dropship"), &Desc)))
			return E_FAIL;

		m_pImgui_Manager->Get_Instance()->Set_Create(false);
		m_iCount++;
	}
	else
	{
		_ulong		dwByte = 0;
		HANDLE		hFile = CreateFile(TEXT("../../Client/Bin/Resources/Map/Town.txt"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
		size_t dataLength;
		wstring Name;
		_float3		Size = { 0.f,0.f,0.f };
		_float      Radian = { 0 };
		_vector		Pos = { XMVectorSet(0.f,0.f,0.f,0.f) };

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

			if (!ReadFile(hFile, &Pos, sizeof(_vector), &dwByte, nullptr))
				break;

			if (!ReadFile(hFile, &Size, sizeof(_float3), &dwByte, nullptr))
				break;

			if (!ReadFile(hFile, &Radian, sizeof(_float), &dwByte, nullptr))
				break;

			wstring targetString = L"obj_Dropship";
			if (Name.find(targetString) != wstring::npos)
			{
				wstring extractedString = Name.substr(0, targetString.length());
				Desc.strName = extractedString;
			}
			else
				Desc.strName = L"";
			Desc.CreatePos = Pos;
			Desc.ObjectSize = Size;
			Desc.fRadian = Radian;

			if (Desc.strName == L"obj_Dropship")
			{
				if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOOL, strLayerTag,
					TEXT("Prototype_GameObject_Dropship"), &Desc)))
					return E_FAIL;
			}
		}
		CloseHandle(hFile);

	}


	Safe_Release(pGameInstance);
	return S_OK;
}

HRESULT CToolMainApp::Ready_Layer_JakobsTank(const wstring& strLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CJakobsTank::OBJECTDESC Desc;

	_float3 PickingPos = m_pImgui_Manager->Get_Instance()->Get_PickingPos();

	if (!m_pImgui_Manager->Get_Instance()->Get_Load())
	{
		Desc.strName = m_pImgui_Manager->Get_Instance()->Get_ObjectName();
		Desc.CreatePos = XMVectorSet(PickingPos.x, PickingPos.y, PickingPos.z, 1.f);
		Desc.ObjectSize = { 1.f, 1.f, 1.f };
		Desc.fRadian = { 0.f };

		if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOOL, strLayerTag,
			TEXT("Prototype_GameObject_JakobsTank"), &Desc)))
			return E_FAIL;

		m_pImgui_Manager->Get_Instance()->Set_Create(false);
		m_iCount++;
	}
	else
	{
		_ulong		dwByte = 0;
		HANDLE		hFile = CreateFile(TEXT("../../Client/Bin/Resources/Map/Town.txt"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
		size_t dataLength;
		wstring Name;
		_float3		Size = { 0.f,0.f,0.f };
		_float      Radian = { 0 };
		_vector		Pos = { XMVectorSet(0.f,0.f,0.f,0.f) };

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

			if (!ReadFile(hFile, &Pos, sizeof(_vector), &dwByte, nullptr))
				break;

			if (!ReadFile(hFile, &Size, sizeof(_float3), &dwByte, nullptr))
				break;

			if (!ReadFile(hFile, &Radian, sizeof(_float), &dwByte, nullptr))
				break;

			wstring targetString = L"obj_JakobsTank";
			if (Name.find(targetString) != wstring::npos)
			{
				wstring extractedString = Name.substr(0, targetString.length());
				Desc.strName = extractedString;
			}
			else
				Desc.strName = L"";
			Desc.CreatePos = Pos;
			Desc.ObjectSize = Size;
			Desc.fRadian = Radian;

			if (Desc.strName == L"obj_JakobsTank")
			{
				if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOOL, strLayerTag,
					TEXT("Prototype_GameObject_JakobsTank"), &Desc)))
					return E_FAIL;
			}
		}
		CloseHandle(hFile);

	}


	Safe_Release(pGameInstance);
	return S_OK;
}

HRESULT CToolMainApp::Ready_Layer_DropzoneWall(const wstring& strLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CDropzoneWall::OBJECTDESC Desc;

	_float3 PickingPos = m_pImgui_Manager->Get_Instance()->Get_PickingPos();

	if (!m_pImgui_Manager->Get_Instance()->Get_Load())
	{
		Desc.strName = m_pImgui_Manager->Get_Instance()->Get_ObjectName();
		Desc.CreatePos = XMVectorSet(PickingPos.x, PickingPos.y, PickingPos.z, 1.f);
		Desc.ObjectSize = { 1.f, 1.f, 1.f };
		Desc.fRadian = { 0.f };

		if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOOL, strLayerTag,
			TEXT("Prototype_GameObject_DropzoneWall"), &Desc)))
			return E_FAIL;

		m_pImgui_Manager->Get_Instance()->Set_Create(false);
		m_iCount++;
	}
	else
	{
		_ulong		dwByte = 0;
		HANDLE		hFile = CreateFile(TEXT("../../Client/Bin/Resources/Map/Town.txt"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
		size_t dataLength;
		wstring Name;
		_float3		Size = { 0.f,0.f,0.f };
		_float      Radian = { 0 };
		_vector		Pos = { XMVectorSet(0.f,0.f,0.f,0.f) };

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

			if (!ReadFile(hFile, &Pos, sizeof(_vector), &dwByte, nullptr))
				break;

			if (!ReadFile(hFile, &Size, sizeof(_float3), &dwByte, nullptr))
				break;

			if (!ReadFile(hFile, &Radian, sizeof(_float), &dwByte, nullptr))
				break;

			wstring targetString = L"obj_DropzoneWall";
			if (Name.find(targetString) != wstring::npos)
			{
				wstring extractedString = Name.substr(0, targetString.length());
				Desc.strName = extractedString;
			}
			else
				Desc.strName = L"";
			Desc.CreatePos = Pos;
			Desc.ObjectSize = Size;
			Desc.fRadian = Radian;

			if (Desc.strName == L"obj_DropzoneWall")
			{
				if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOOL, strLayerTag,
					TEXT("Prototype_GameObject_DropzoneWall"), &Desc)))
					return E_FAIL;
			}
		}
		CloseHandle(hFile);

	}


	Safe_Release(pGameInstance);
	return S_OK;
}

HRESULT CToolMainApp::Ready_Layer_LynchwoodHotel(const wstring& strLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CLynchwoodHotel::OBJECTDESC Desc;

	_float3 PickingPos = m_pImgui_Manager->Get_Instance()->Get_PickingPos();

	if (!m_pImgui_Manager->Get_Instance()->Get_Load())
	{
		Desc.strName = m_pImgui_Manager->Get_Instance()->Get_ObjectName();
		Desc.CreatePos = XMVectorSet(PickingPos.x, PickingPos.y, PickingPos.z, 1.f);
		Desc.ObjectSize = { 1.f, 1.f, 1.f };
		Desc.fRadian = { 0.f };

		if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOOL, strLayerTag,
			TEXT("Prototype_GameObject_LynchwoodHotel"), &Desc)))
			return E_FAIL;

		m_pImgui_Manager->Get_Instance()->Set_Create(false);
		m_iCount++;
	}
	else
	{
		_ulong		dwByte = 0;
		HANDLE		hFile = CreateFile(TEXT("../../Client/Bin/Resources/Map/Town.txt"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
		size_t dataLength;
		wstring Name;
		_float3		Size = { 0.f,0.f,0.f };
		_float      Radian = { 0 };
		_vector		Pos = { XMVectorSet(0.f,0.f,0.f,0.f) };

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

			if (!ReadFile(hFile, &Pos, sizeof(_vector), &dwByte, nullptr))
				break;

			if (!ReadFile(hFile, &Size, sizeof(_float3), &dwByte, nullptr))
				break;

			if (!ReadFile(hFile, &Radian, sizeof(_float), &dwByte, nullptr))
				break;

			wstring targetString = L"obj_LynchwoodHotel";
			if (Name.find(targetString) != wstring::npos)
			{
				wstring extractedString = Name.substr(0, targetString.length());
				Desc.strName = extractedString;
			}
			else
				Desc.strName = L"";
			Desc.CreatePos = Pos;
			Desc.ObjectSize = Size;
			Desc.fRadian = Radian;

			if (Desc.strName == L"obj_LynchwoodHotel")
			{
				if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOOL, strLayerTag,
					TEXT("Prototype_GameObject_LynchwoodHotel"), &Desc)))
					return E_FAIL;
			}
		}
		CloseHandle(hFile);

	}


	Safe_Release(pGameInstance);
	return S_OK;
}

HRESULT CToolMainApp::Ready_Layer_Boat(const wstring& strLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CBoat::OBJECTDESC Desc;

	_float3 PickingPos = m_pImgui_Manager->Get_Instance()->Get_PickingPos();

	if (!m_pImgui_Manager->Get_Instance()->Get_Load())
	{
		Desc.strName = m_pImgui_Manager->Get_Instance()->Get_ObjectName();
		Desc.CreatePos = XMVectorSet(PickingPos.x, PickingPos.y, PickingPos.z, 1.f);
		Desc.ObjectSize = { 1.f, 1.f, 1.f };
		Desc.fRadian = { 0.f };

		if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOOL, strLayerTag,
			TEXT("Prototype_GameObject_Boat"), &Desc)))
			return E_FAIL;

		m_pImgui_Manager->Get_Instance()->Set_Create(false);
		m_iCount++;
	}
	else
	{
		_ulong		dwByte = 0;
		HANDLE		hFile = CreateFile(TEXT("../../Client/Bin/Resources/Map/Town.txt"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
		size_t dataLength;
		wstring Name;
		_float3		Size = { 0.f,0.f,0.f };
		_float      Radian = { 0 };
		_vector		Pos = { XMVectorSet(0.f,0.f,0.f,0.f) };

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

			if (!ReadFile(hFile, &Pos, sizeof(_vector), &dwByte, nullptr))
				break;

			if (!ReadFile(hFile, &Size, sizeof(_float3), &dwByte, nullptr))
				break;

			if (!ReadFile(hFile, &Radian, sizeof(_float), &dwByte, nullptr))
				break;

			wstring targetString = L"obj_Boat";
			if (Name.find(targetString) != wstring::npos)
			{
				wstring extractedString = Name.substr(0, targetString.length());
				Desc.strName = extractedString;
			}
			else
				Desc.strName = L"";
			Desc.CreatePos = Pos;
			Desc.ObjectSize = Size;
			Desc.fRadian = Radian;

			if (Desc.strName == L"obj_Boat")
			{
				if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOOL, strLayerTag,
					TEXT("Prototype_GameObject_Boat"), &Desc)))
					return E_FAIL;
			}
		}
		CloseHandle(hFile);

	}


	Safe_Release(pGameInstance);
	return S_OK;
}

HRESULT CToolMainApp::Ready_Layer_ForkLift(const wstring& strLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CForkLift::OBJECTDESC Desc;

	_float3 PickingPos = m_pImgui_Manager->Get_Instance()->Get_PickingPos();

	if (!m_pImgui_Manager->Get_Instance()->Get_Load())
	{
		Desc.strName = m_pImgui_Manager->Get_Instance()->Get_ObjectName();
		Desc.CreatePos = XMVectorSet(PickingPos.x, PickingPos.y, PickingPos.z, 1.f);
		Desc.ObjectSize = { 1.f, 1.f, 1.f };
		Desc.fRadian = { 0.f };

		if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOOL, strLayerTag,
			TEXT("Prototype_GameObject_ForkLift"), &Desc)))
			return E_FAIL;

		m_pImgui_Manager->Get_Instance()->Set_Create(false);
		m_iCount++;
	}
	else
	{
		_ulong		dwByte = 0;
		HANDLE		hFile = CreateFile(TEXT("../../Client/Bin/Resources/Map/Town.txt"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
		size_t dataLength;
		wstring Name;
		_float3		Size = { 0.f,0.f,0.f };
		_float      Radian = { 0 };
		_vector		Pos = { XMVectorSet(0.f,0.f,0.f,0.f) };

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

			if (!ReadFile(hFile, &Pos, sizeof(_vector), &dwByte, nullptr))
				break;

			if (!ReadFile(hFile, &Size, sizeof(_float3), &dwByte, nullptr))
				break;

			if (!ReadFile(hFile, &Radian, sizeof(_float), &dwByte, nullptr))
				break;

			wstring targetString = L"obj_ForkLift";
			if (Name.find(targetString) != wstring::npos)
			{
				wstring extractedString = Name.substr(0, targetString.length());
				Desc.strName = extractedString;
			}
			else
				Desc.strName = L"";
			Desc.CreatePos = Pos;
			Desc.ObjectSize = Size;
			Desc.fRadian = Radian;

			if (Desc.strName == L"obj_ForkLift")
			{
				if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOOL, strLayerTag,
					TEXT("Prototype_GameObject_ForkLift"), &Desc)))
					return E_FAIL;
			}
		}
		CloseHandle(hFile);

	}


	Safe_Release(pGameInstance);
	return S_OK;
}

CToolMainApp* CToolMainApp::Create()
{
	CToolMainApp* pInstance = new CToolMainApp();

	if (FAILED(pInstance->Initialize()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Created : CToolMainApp");
	}

	return pInstance;
}

void CToolMainApp::Free()
{
	CImgui_Manager::Destroy_Instance();
	Safe_Release(m_pImgui_Manager);

	CTool_ObjectManager::Destroy_Instance();
	Safe_Release(m_pObject_Manager);

	Safe_Release(m_pRenderer);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);

	CGameInstance::Release_Engine();
}
