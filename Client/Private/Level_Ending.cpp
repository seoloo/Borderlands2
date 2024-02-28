#include "stdafx.h"
#include "Level_Ending.h"

#include "GameInstance.h"
#include "GameObject.h"

#include "Level_Loading.h"

#include "Camera_OnBone.h"
#include "Camera_Free.h"

#include "SoundMgr.h"

#pragma region Static Object
#include "DamWall.h"
#include "MetalBeam.h"
#include "MetalLadder.h"
#include "WaterTower.h"
#include "Dropship.h"
#include "Yurt.h"
#include "Yurt2.h"
#pragma endregion

#include "Skag.h"
#include "Stalker.h"

CLevel_Ending::CLevel_Ending(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Ending::Initialize()
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	g_bMissionClear = false;
	g_bFirstMissionClear = false;
	g_bSceondMissionClear = false;
	g_bThirdMissionClear = false;
	g_bMonsterCollidedMouse = false;
	g_bDestroyedName = false;
	g_strMonsterName = TEXT(" ");
	g_DialogueCount = 0;
	g_bInteractionNPC = false;
	g_InteractionCount = 0;

	/*if (FAILED(Ready_Lights()))
		return E_FAIL;*/

	/*if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;*/

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_ENDING, TEXT("Layer_Player"),
		TEXT("Prototype_GameObject_EndingPlayer"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_ENDING, TEXT("Layer_Hammerlock"),
		TEXT("Prototype_GameObject_EndingHammerlock"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_ENDING, TEXT("Layer_Roland"),
		TEXT("Prototype_GameObject_EndingRoland"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_ENDING, TEXT("Layer_Lilth"),
		TEXT("Prototype_GameObject_EndingLilth"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_ENDING, TEXT("Layer_Mordecai"),
		TEXT("Prototype_GameObject_EndingMordecai"))))
		return E_FAIL;

#pragma region UI
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_ENDING, TEXT("Layer_HpBar"),
		TEXT("Prototype_GameObject_EndingHpBar"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_ENDING, TEXT("Layer_HpIcon"),
		TEXT("Prototype_GameObject_EndingHpIcon"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_ENDING, TEXT("Layer_GunBorder"),
		TEXT("Prototype_GameObject_EndingGunBorder"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_ENDING, TEXT("Layer_GunBar"),
		TEXT("Prototype_GameObject_EndingGunBar"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_ENDING, TEXT("Layer_Sight"),
		TEXT("Prototype_GameObject_EndingSight"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_ENDING, TEXT("Layer_GunIcon"),
		TEXT("Prototype_GameObject_EndingGunIcon"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_ENDING, TEXT("Layer_UIFont"),
		TEXT("Prototype_GameObject_EndingUIFont"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_ENDING, TEXT("Layer_ShieldBar"),
		TEXT("Prototype_GameObject_EndingShieldBar"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_ENDING, TEXT("Layer_HpBorder"),
		TEXT("Prototype_GameObject_EndingHpBorder"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_ENDING, TEXT("Layer_ShieldBorder"),
		TEXT("Prototype_GameObject_EndingShieldBorder"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_ENDING, TEXT("Layer_ShieldIcon"),
		TEXT("Prototype_GameObject_EndingShieldIcon"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_ENDING, TEXT("Layer_EXPBorder"),
		TEXT("Prototype_GameObject_EndingEXPBorder"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_ENDING, TEXT("Layer_EXPBar"),
		TEXT("Prototype_GameObject_EndingEXPBar"))))
		return E_FAIL;
	
	/*if (FAILED(pGameInstance->Add_GameObject(LEVEL_ENDING, TEXT("Layer_MissionBox"),
		TEXT("Prototype_GameObject_EndingMissionBox"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_ENDING, TEXT("Layer_MissionFont"),
		TEXT("Prototype_GameObject_EndingMissionFont"))))
		return E_FAIL;*/

	

#pragma endregion


	if (FAILED(pGameInstance->Add_GameObject(LEVEL_ENDING, TEXT("Layer_AreaFont"),
		TEXT("Prototype_GameObject_EndingAreaFont"))))
		return E_FAIL;

#pragma region Static Object
	m_ObjectName = TEXT("Layer_Object") + to_wstring(m_iCount);

	if (FAILED(Ready_Layer_DamWall(TEXT("Layer_Object"))))
		return E_FAIL;
	if (FAILED(Ready_Layer_MetalBeam(TEXT("Layer_Object"))))
		return E_FAIL;
	if (FAILED(Ready_Layer_MetalLadder(TEXT("Layer_Object"))))
		return E_FAIL;
	if (FAILED(Ready_Layer_WaterTower(TEXT("Layer_Object"))))
		return E_FAIL;
	if (FAILED(Ready_Layer_Dropship(TEXT("Layer_Object"))))
		return E_FAIL;
	if (FAILED(Ready_Layer_Yurt(TEXT("Layer_Object"))))
		return E_FAIL;
	if (FAILED(Ready_Layer_Yurt2(TEXT("Layer_Object"))))
		return E_FAIL;

#pragma endregion

	
	Safe_Release(pGameInstance);

	return S_OK;
}

void CLevel_Ending::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (g_bInteractionNPC && g_InteractionCount == 0)
	{
		if (FAILED(pGameInstance->Add_GameObject(LEVEL_ENDING, TEXT("Layer_InteractionItem"),
			TEXT("Prototype_GameObject_EndingTextFont"))))
			return;
		if (FAILED(pGameInstance->Add_GameObject(LEVEL_ENDING, TEXT("Layer_InteractionItem"),
			TEXT("Prototype_GameObject_EndingInteractionFont"))))
			return;

		if (FAILED(pGameInstance->Add_GameObject(LEVEL_ENDING, TEXT("Layer_InteractionItem"),
			TEXT("Prototype_GameObject_EndingKey_G"))))
			return;
		if (KEYDOWN('G'))
		{
			
		}

		g_InteractionCount++;
	}

	Safe_Release(pGameInstance);
}

void CLevel_Ending::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	SetWindowText(g_hWnd, TEXT("마을레벨입니다."));
}

HRESULT CLevel_Ending::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	return S_OK;
}

HRESULT CLevel_Ending::Ready_Lights()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	LIGHTDESC		LightDesc;

	/* 방향성 광원을 추가한다. */
	ZeroMemory(&LightDesc, sizeof LightDesc);

	LightDesc.eType = LIGHTDESC::TYPE_DIRECTIONAL;
	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);

	LightDesc.vDiffuse = _float4(0.7f, 0.7f, 0.7f, 1.f);
	LightDesc.vAmbient = _float4(0.5f, 0.5f, 0.5f, 1.f);
	LightDesc.vSpecular = _float4(0.3f, 0.3f, 0.3f, 1.f);

	if (FAILED(pGameInstance->Add_Lights(m_pDevice, m_pContext, LightDesc)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Ending::Ready_Layer_Camera(const wstring& strLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CCamera_Free::CAMERAFREEDESC		CameraFreeDesc;

	//CameraFreeDesc.iData = 10;
	CameraFreeDesc.CameraDesc.iLevelIndex = LEVEL_STATIC;
	CameraFreeDesc.CameraDesc.strTranformTag = TEXT("Prototype_Component_Transform");
	// 위치
	CameraFreeDesc.CameraDesc.vEye = _float4(0.f, 1.f, -7.f, 1.f);
	// 보는방향
	CameraFreeDesc.CameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	// 시야각
	CameraFreeDesc.CameraDesc.fFovy = XMConvertToRadians(60.0f);
	// 투영행렬의 Aspect (해상도에 따른 크기변화 반영)
	CameraFreeDesc.CameraDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	// 가장 가까운 거리
	CameraFreeDesc.CameraDesc.fNear = 0.2f;
	// 가장 먼 거리
	CameraFreeDesc.CameraDesc.fFar = 300.f;
	// 초당 이동 속도
	CameraFreeDesc.CameraDesc.TransformDesc.fSpeedPerSec = 10.0f;
	// 초당 회전 속도
	CameraFreeDesc.CameraDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	/* For.Camera */
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_ENDING, strLayerTag,
		TEXT("Prototype_GameObject_EndingCamera_Free"), &CameraFreeDesc)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Ending::Ready_Layer_BackGround(const wstring& strLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	/* For.Terrain */
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_ENDING, strLayerTag,
		TEXT("Prototype_GameObject_EndingTerrain"))))
		return E_FAIL;

	/* For.SKy */
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_ENDING, strLayerTag,
		TEXT("Prototype_GameObject_EndingSky"))))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Ending::Ready_Layer_DamWall(const wstring& strLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CDamWall::OBJECTINFO Desc;

	_ulong		dwByte = 0;
	HANDLE		hFile = CreateFile(TEXT("../../Client/Bin/Resources/Map/Town.txt"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	size_t dataLength;
	wstring Name;
	_float3		Size = { 0.f,0.f,0.f };
	_float      Radian = { 0 };
	_vector		Pos = { XMVectorSet(0.f,0.f,0.f,0.f) };

	while (true)
	{
		ReadFile(hFile, &dataLength, sizeof(size_t), &dwByte, nullptr);

		if (dataLength == 0)
			break;

		if (0 == dwByte)
			break;

		Name.resize(dataLength);
		ReadFile(hFile, &Name[0], sizeof(wchar_t) * dataLength, &dwByte, nullptr);
		ReadFile(hFile, &Pos, sizeof(_vector), &dwByte, nullptr);
		ReadFile(hFile, &Size, sizeof(_float3), &dwByte, nullptr);
		ReadFile(hFile, &Radian, sizeof(_float), &dwByte, nullptr);

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
			if (FAILED(pGameInstance->Add_GameObject(LEVEL_ENDING, strLayerTag,
				TEXT("Prototype_GameObject_EndingDamWall"), &Desc)))
				return E_FAIL;
			m_iCount++;

		}
	}
	CloseHandle(hFile);

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Ending::Ready_Layer_MetalBeam(const wstring& strLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CMetalBeam::OBJECTINFO Desc;

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
			if (FAILED(pGameInstance->Add_GameObject(LEVEL_ENDING, strLayerTag,
				TEXT("Prototype_GameObject_EndingMetalBeam"), &Desc)))
				return E_FAIL;
			m_iCount++;

		}
	}
	CloseHandle(hFile);

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Ending::Ready_Layer_MetalLadder(const wstring& strLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CMetalLadder::OBJECTINFO Desc;

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
			if (FAILED(pGameInstance->Add_GameObject(LEVEL_ENDING, strLayerTag,
				TEXT("Prototype_GameObject_EndingMetalLadder"), &Desc)))
				return E_FAIL;
			m_iCount++;

		}
	}
	CloseHandle(hFile);

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Ending::Ready_Layer_WaterTower(const wstring& strLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CWaterTower::OBJECTINFO Desc;

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
			if (FAILED(pGameInstance->Add_GameObject(LEVEL_ENDING, strLayerTag,
				TEXT("Prototype_GameObject_EndingWaterTower"), &Desc)))
				return E_FAIL;
			m_iCount++;

		}
	}
	CloseHandle(hFile);

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Ending::Ready_Layer_Dropship(const wstring& strLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CDropship::OBJECTINFO Desc;

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
			if (FAILED(pGameInstance->Add_GameObject(LEVEL_ENDING, strLayerTag,
				TEXT("Prototype_GameObject_EndingDropship"), &Desc)))
				return E_FAIL;
			m_iCount++;

		}
	}
	CloseHandle(hFile);

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Ending::Ready_Layer_Yurt(const wstring& strLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CYurt::OBJECTINFO Desc;

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
			if (FAILED(pGameInstance->Add_GameObject(LEVEL_ENDING, strLayerTag,
				TEXT("Prototype_GameObject_EndingYurt"), &Desc)))
				return E_FAIL;
			m_iCount++;

		}
	}
	CloseHandle(hFile);

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Ending::Ready_Layer_Yurt2(const wstring& strLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CYurt2::OBJECTINFO Desc;

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
			if (FAILED(pGameInstance->Add_GameObject(LEVEL_ENDING, strLayerTag,
				TEXT("Prototype_GameObject_EndingYurt2"), &Desc)))
				return E_FAIL;
			m_iCount++;

		}
	}
	CloseHandle(hFile);

	Safe_Release(pGameInstance);

	return S_OK;
}

CLevel_Ending* CLevel_Ending::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Ending* pInstance = new CLevel_Ending(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Created : CLevel_Ending");
	}

	return pInstance;
}

void CLevel_Ending::Free()
{
	__super::Free();
}
