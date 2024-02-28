#include "stdafx.h"
#include "Level_Town.h"

#include "GameInstance.h"
#include "GameObject.h"

#include "Level_Loading.h"

#include "Camera_OnBone.h"
#include "Camera_Free.h"

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
#include "SoundMgr.h"

CLevel_Town::CLevel_Town(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Town::Initialize()
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	g_AmountOfBullet = 500;
	g_BulletCount = 50;

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

	if (FAILED(Ready_Lights()))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOWN, TEXT("Layer_Player"),
		TEXT("Prototype_GameObject_TownPlayer"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOWN, TEXT("Layer_Hammerlock"),
		TEXT("Prototype_GameObject_TownHammerlock"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOWN, TEXT("Layer_Roland"),
		TEXT("Prototype_GameObject_TownRoland"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOWN, TEXT("Layer_Lilth"),
		TEXT("Prototype_GameObject_TownLilth"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOWN, TEXT("Layer_Mordecai"),
		TEXT("Prototype_GameObject_TownMordecai"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOWN, TEXT("Layer_Hammerlock"),
		TEXT("Prototype_GameObject_ClapTrap"))))
		return E_FAIL;

#pragma region UI
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOWN, TEXT("Layer_HpBar"),
		TEXT("Prototype_GameObject_TownHpBar"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOWN, TEXT("Layer_HpIcon"),
		TEXT("Prototype_GameObject_TownHpIcon"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOWN, TEXT("Layer_GunBorder"),
		TEXT("Prototype_GameObject_TownGunBorder"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOWN, TEXT("Layer_GunBar"),
		TEXT("Prototype_GameObject_TownGunBar"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOWN, TEXT("Layer_Sight"),
		TEXT("Prototype_GameObject_TownSight"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOWN, TEXT("Layer_GunIcon"),
		TEXT("Prototype_GameObject_TownGunIcon"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOWN, TEXT("Layer_UIFont"),
		TEXT("Prototype_GameObject_TownUIFont"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOWN, TEXT("Layer_ShieldBar"),
		TEXT("Prototype_GameObject_TownShieldBar"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOWN, TEXT("Layer_HpBorder"),
		TEXT("Prototype_GameObject_TownHpBorder"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOWN, TEXT("Layer_ShieldBorder"),
		TEXT("Prototype_GameObject_TownShieldBorder"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOWN, TEXT("Layer_ShieldIcon"),
		TEXT("Prototype_GameObject_TownShieldIcon"))))
		return E_FAIL;

	/*if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOWN, TEXT("Layer_TextUI"),
		TEXT("Prototype_GameObject_TownTextUI"))))
		return E_FAIL;*/

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOWN, TEXT("Layer_EXPBorder"),
		TEXT("Prototype_GameObject_TownEXPBorder"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOWN, TEXT("Layer_EXPBar"),
		TEXT("Prototype_GameObject_TownEXPBar"))))
		return E_FAIL;
	
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOWN, TEXT("Layer_MissionBox"),
		TEXT("Prototype_GameObject_TownMissionBox"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOWN, TEXT("Layer_MissionFont"),
		TEXT("Prototype_GameObject_TownMissionFont"))))
		return E_FAIL;

	

#pragma endregion

	/*if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOWN, TEXT("Layer_AreaFont"),
		TEXT("Prototype_GameObject_TownAreaFont"))))
		return E_FAIL;*/

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

void CLevel_Town::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	/*m_fTimer2 += fTimeDelta;
	if (m_fTimer2 > 30.f)
	{
		if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOWN, TEXT("Layer_TreasureChest"),
			TEXT("Prototype_GameObject_TownTreasureChest"))))
			return;
		m_fTimer2 = 0.f;
	}*/


	if (KEYDOWN('P'))
		g_bTownEnd = true;

	if (g_bTownEnd == true)
	{
		if (FAILED(pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_GAMEPLAY))))
		{
			//pGameInstance->Remove_Lights();
			Safe_Release(pGameInstance);
			return;
		}
	}

	/*if (g_bMonsterCollidedMouse)
	{
		if (m_iNumber == 0)
		{
			if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOWN, TEXT("Layer_MonsterHpBar"),
				TEXT("Prototype_GameObject_TownMonsterHpBar"))))
				return;

			m_iNumber++;
		}
	}
	if (g_bDestroyedName)
	{
		m_iNumber = 0;
		g_bDestroyedName = false;
	}*/

	if (g_bInteractionItem && g_InteractionCount ==0)
	{
		if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOWN, TEXT("Layer_InteractionItem"),
			TEXT("Prototype_GameObject_TownInteractionFont"))))
			return;

		if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOWN, TEXT("Layer_InteractionItem"),
			TEXT("Prototype_GameObject_TownKey_G"))))
			return;

		g_InteractionCount++;
	}

	if (g_bInteractionNPC /*&& g_InteractionCount == 0*/)
	{
		if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOWN, TEXT("Layer_InteractionItem"),
			TEXT("Prototype_GameObject_TownInteractionFont"))))
			return;

		if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOWN, TEXT("Layer_InteractionItem"),
			TEXT("Prototype_GameObject_TownKey_G"))))
			return;
		if (KEYDOWN('G'))
		{
			if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOWN, TEXT("Layer_InteractionItem"),
				TEXT("Prototype_GameObject_TownTextFont"))))
				return;
		}

		g_InteractionCount++;
	}

	m_fTimer += fTimeDelta;
	if (m_fTimer >= 1.f)
	{
		_ulong		dwByte = 0;
		HANDLE		hFile = CreateFile(TEXT("../../Client/Bin/Resources/Effect/DigRock.txt"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
		//HANDLE		hFile = CreateFile(TEXT("../../Client/Bin/Resources/Effect/BleedingSnow.txt"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
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
			ReadFile(hFile, &Name[0], sizeof(wchar_t) * dataLength, &dwByte, nullptr);
			ReadFile(hFile, &Desc.iNumInstance, sizeof(_int), &dwByte, nullptr);
			ReadFile(hFile, &Desc.fLifeTime, sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &Desc.vPivotPos, sizeof(_float4), &dwByte, nullptr);
			ReadFile(hFile, &Desc.vRange, sizeof(_float3), &dwByte, nullptr);
			ReadFile(hFile, &Desc.fMinSpeed, sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &Desc.fMaxSpeed, sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &Desc.bFalling, sizeof(_bool), &dwByte, nullptr);
			ReadFile(hFile, &Desc.bRising, sizeof(_bool), &dwByte, nullptr);
			ReadFile(hFile, &Desc.bExplosion, sizeof(_bool), &dwByte, nullptr);
			ReadFile(hFile, &Desc.bBleeding, sizeof(_bool), &dwByte, nullptr);
			ReadFile(hFile, &Desc.bDig, sizeof(_bool), &dwByte, nullptr);
			ReadFile(hFile, &Desc.bGreen, sizeof(_bool), &dwByte, nullptr);

			random_device		RandomDevice;
			mt19937_64			RandomNumber;
			uniform_real_distribution<float>		RandomX;
			uniform_real_distribution<float>		RandomZ;

			RandomNumber = mt19937_64(RandomDevice());

			RandomX = uniform_real_distribution<float>(5.f, 10.f);
			RandomZ = uniform_real_distribution<float>(10.f, 15.f);

			_vector TempPos = XMVectorSet(13.f, 0.f, 10.f, 1.f);

			XMStoreFloat4(&Desc.vPivotPos, TempPos);

			Desc.bDig = true;
			Desc.bFog = false;
			Desc.bRed = false;
			/*Desc.bGray = true;
			Desc.fMinSpeed = 20.f;
			Desc.fMaxSpeed = 50.f;*/

			/*if (FAILED(pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Layer_Effect_Blast"),
				TEXT("Prototype_GameObject_TownBlast"), &Desc)))
				return;*/
		}
		CloseHandle(hFile);

		//CSoundMgr::Get_Instance()->PlaySoundW(L"Baggi_Land.wav", 1.f);

		m_fTimer = 0.f;
	}

	Safe_Release(pGameInstance);
}

void CLevel_Town::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	SetWindowText(g_hWnd, TEXT("마을레벨입니다."));
}

HRESULT CLevel_Town::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();



	return S_OK;
}

HRESULT CLevel_Town::Ready_Lights()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	LIGHTDESC		LightDesc;

	/* 방향성 광원을 추가한다. */
	ZeroMemory(&LightDesc, sizeof LightDesc);

	LightDesc.eType = LIGHTDESC::TYPE_DIRECTIONAL;
	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);

	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.8f, 0.8f, 0.8f, 1.f);
	LightDesc.vSpecular = _float4(0.3f, 0.3f, 0.3f, 1.f);

	if (FAILED(pGameInstance->Add_Lights(m_pDevice, m_pContext, LightDesc)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Town::Ready_Layer_Camera(const wstring& strLayerTag)
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
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOWN, strLayerTag,
		TEXT("Prototype_GameObject_Camera_Free"), &CameraFreeDesc)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Town::Ready_Layer_BackGround(const wstring& strLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	/* For.Terrain */
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOWN, strLayerTag,
		TEXT("Prototype_GameObject_TownTerrain"))))
		return E_FAIL;

	/* For.SKy */
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOWN, strLayerTag,
		TEXT("Prototype_GameObject_TownSky"))))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Town::Ready_Layer_DamWall(const wstring& strLayerTag)
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
			if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOWN, strLayerTag,
				TEXT("Prototype_GameObject_TownDamWall"), &Desc)))
				return E_FAIL;
			m_iCount++;

		}
	}
	CloseHandle(hFile);

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Town::Ready_Layer_MetalBeam(const wstring& strLayerTag)
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
			if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOWN, strLayerTag,
				TEXT("Prototype_GameObject_TownMetalBeam"), &Desc)))
				return E_FAIL;
			m_iCount++;

		}
	}
	CloseHandle(hFile);

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Town::Ready_Layer_MetalLadder(const wstring& strLayerTag)
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
			if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOWN, strLayerTag,
				TEXT("Prototype_GameObject_MetalLadder"), &Desc)))
				return E_FAIL;
			m_iCount++;

		}
	}
	CloseHandle(hFile);

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Town::Ready_Layer_WaterTower(const wstring& strLayerTag)
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
			if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOWN, strLayerTag,
				TEXT("Prototype_GameObject_TownWaterTower"), &Desc)))
				return E_FAIL;
			m_iCount++;

		}
	}
	CloseHandle(hFile);

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Town::Ready_Layer_Dropship(const wstring& strLayerTag)
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
			if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOWN, strLayerTag,
				TEXT("Prototype_GameObject_TownDropship"), &Desc)))
				return E_FAIL;
			m_iCount++;

		}
	}
	CloseHandle(hFile);

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Town::Ready_Layer_Yurt(const wstring& strLayerTag)
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
			if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOWN, strLayerTag,
				TEXT("Prototype_GameObject_TownYurt"), &Desc)))
				return E_FAIL;
			m_iCount++;

		}
	}
	CloseHandle(hFile);

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Town::Ready_Layer_Yurt2(const wstring& strLayerTag)
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
			if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOWN, strLayerTag,
				TEXT("Prototype_GameObject_TownYurt2"), &Desc)))
				return E_FAIL;
			m_iCount++;

		}
	}
	CloseHandle(hFile);

	Safe_Release(pGameInstance);

	return S_OK;
}

CLevel_Town* CLevel_Town::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Town* pInstance = new CLevel_Town(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Created : CLevel_Town");
	}

	return pInstance;
}

void CLevel_Town::Free()
{
	__super::Free();


}
