#include "..\Public\Level_Tool.h"

#include "GameInstance.h"
#include "Tool_Camera.h"
#include "Imgui_Manager.h"
#include "Tool_Object_Icebox01.h"

#include "ToolMainApp.h"

CLevel_Tool::CLevel_Tool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Tool::Initialize()
{	
	if (FAILED(__super::Initialize()))
		return E_FAIL;

	if (FAILED(Ready_Lights()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Terrain(TEXT("Layer_Terrain"))))
		return E_FAIL;

	m_pImgui_Manager->Initialize(m_pDevice, m_pContext);

	return S_OK;
}

void CLevel_Tool::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
	m_pImgui_Manager->Tick();


	wstring ObjectName = TEXT("Layer_Object") + to_wstring(m_iCount);

	if (m_pImgui_Manager->Get_Instance()->Get_FBXName() == TEXT("IceBlock06") &&
		m_pImgui_Manager->Get_Instance()->Get_Create())
	{
		if (FAILED(Ready_Layer_IceBlock06(ObjectName)))
			return;
	}
	if (m_pImgui_Manager->Get_Instance()->Get_FBXName() == TEXT("IceBlock01") &&
		m_pImgui_Manager->Get_Instance()->Get_Create() == true)
	{
		if (FAILED(Ready_Layer_IceBlock01(ObjectName)))
			return;
	}
	
}

void CLevel_Tool::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	SetWindowText(g_hWnd, TEXT("Tool"));
}

HRESULT CLevel_Tool::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;
	m_pImgui_Manager->Render();

	return S_OK;
}

HRESULT CLevel_Tool::Ready_Lights()
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

HRESULT CLevel_Tool::Ready_Layer_Terrain(const wstring& strLayerTag)
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

HRESULT CLevel_Tool::Ready_Layer_Camera(const wstring& strLayerTag)
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

HRESULT CLevel_Tool::Ready_Layer_Object(const wstring& strLayerTag)
{
	CGameInstance* pGameinstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameinstance);

	Safe_Release(pGameinstance);

	return S_OK;
}

HRESULT CLevel_Tool::Ready_Layer_IceBlock01(const wstring& strLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CTool_Object_Icebox01::OBJECTDESC Desc;
	ZeroMemory(&Desc, sizeof(CTool_Object_Icebox01::OBJECTDESC));

	_float3 PickingPos = m_pImgui_Manager->Get_Instance()->Get_PickingPos();

	Desc.strName = m_pImgui_Manager->Get_Instance()->Get_ObjectName();
	Desc.CreatePos = XMVectorSet(PickingPos.x, PickingPos.y, PickingPos.z, 1.f);
	Desc.ObjectSize = { 1.f, 1.f, 1.f };
	Desc.fRadian = { 0.f };

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOOL, strLayerTag,
		TEXT("Prototype_GameObject_IceBlock01"), &Desc)))
		return E_FAIL;

	m_pImgui_Manager->Get_Instance()->Set_Create(false);
	m_iCount++;

	Safe_Release(pGameInstance);
	return S_OK;
}

HRESULT CLevel_Tool::Ready_Layer_IceBlock06(const wstring& strLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOOL, strLayerTag,
		TEXT("Prototype_GameObject_IceBlock06"))))
		return E_FAIL;

	m_pImgui_Manager->Get_Instance()->Set_Create(false);
	m_iCount++;

	Safe_Release(pGameInstance);
	return S_OK;
}

CLevel_Tool* CLevel_Tool::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Tool*	pInstance = new CLevel_Tool(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Created : CLevel_Tool");
	}

	return pInstance;
}

void CLevel_Tool::Free()
{
	__super::Free();

	CImgui_Manager::Destroy_Instance();

	Safe_Release(m_pImgui_Manager);
}