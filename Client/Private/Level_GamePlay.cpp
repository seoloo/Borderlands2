#include "stdafx.h"
#include "..\Public\Level_GamePlay.h"

#include "GameInstance.h"

#include "Level_Loading.h"

#include "Camera_Free.h"
#include "HealthVial.h"
#include "GameObject.h"
#include "Thresher_Collider.h"
#include "Camera_OnBone.h"

#pragma region Static Object
#include "AridDistanceTerrain.h"
#include "IceBlock01.h"
#include "Large_Rock1.h"
#include "DamWall.h"
#include "Container.h"
#include "TunnelDoor.h"
#include "CircularSupport.h"
#include "CrateSet.h"
#include "House.h"
#include "MetalBeam.h"
#include "MetalLadder.h"
#include "Pillar.h"
#include "WaterTower.h"
#include "BarCounter.h"
#include "BedGarbage.h"
#include "Boat.h"
#include "Dropship.h"
#include "DropzoneWall.h"
#include "JakobsTank.h"
#include "LynchwoodHotel.h"
#include "Yurt.h"
#include "Yurt2.h"
#pragma endregion

#include "Psycho.h"
#include "Psycho2.h"
#include "Psycho3.h"
#include "Skag.h"
#include "Stalker.h"
#include "SpiderAnt.h"
#include "Thresher.h"
#include "SoundMgr.h"

bool g_bMonsterCollidedMouse;
bool g_bDestroyedName;
std::wstring g_strMonsterName = TEXT(" ");
int	g_iMonsterHp;
bool g_bPsychoCollidedMouse;
bool g_bPsychoCollidedMouse2;
bool g_bPsychoCollidedMouse3;
bool g_bGoliathCollidedMouse;
bool g_bGoliathCollidedMouse2;
bool g_bGoliathCollidedMouse3;

bool	g_bThersherSummon;
int g_ArmNumber;
int g_Arm2Number;
int g_MeleeNumber;
int g_CollidedArmNumber;
int g_CollidedArm2Number;
int g_CollidedMeleeNumber;



CLevel_GamePlay::CLevel_GamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_GamePlay::Initialize()
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

	CSoundMgr::Get_Instance()->StopAll();
	//CSoundMgr::Get_Instance()->PlayBGM(L"Stage01_Boss_Chunlog_Normal_Loop.wav", 1.f);

	/*if (FAILED(Ready_Lights()))
		return E_FAIL;*/

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;
	
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Player"),
		TEXT("Prototype_GameObject_Player"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Snowing"),
		TEXT("Prototype_GameObject_Snowing"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Hammerlock"),
		TEXT("Prototype_GameObject_Hammerlock"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_UI(TEXT("Layer_UI"))))
		return E_FAIL;
	/*if (FAILED(Ready_Layer_Camera(TEXT("Camera_Free"))))
		return E_FAIL;*/

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_AreaFont"),
		TEXT("Prototype_GameObject_AreaFont"))))
		return E_FAIL;

#pragma region Static Object
	m_ObjectName = TEXT("Layer_Object") + to_wstring(m_iCount);

	/*if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("AridDistanceTerrain"),
		TEXT("Prototype_GameObject_AridDistanceTerrain"))))
		return E_FAIL;*/

	/*if (FAILED(Ready_Layer_IceBlock01(TEXT("Layer_Object"))))
		return E_FAIL;
	if (FAILED(Ready_Layer_Large_Rock1(TEXT("Layer_Object"))))
		return E_FAIL;*/
	if (FAILED(Ready_Layer_DamWall(TEXT("Layer_Object"))))
		return E_FAIL;
	if (FAILED(Ready_Layer_Container(TEXT("Layer_Object"))))
		return E_FAIL;
	if (FAILED(Ready_Layer_TunnelDoor(TEXT("Layer_TunnelDoor"))))
		return E_FAIL; 
	if (FAILED(Ready_Layer_CircularSupport(TEXT("Layer_Object"))))
		return E_FAIL;
	if (FAILED(Ready_Layer_House(TEXT("Layer_Object"))))
		return E_FAIL;
	if (FAILED(Ready_Layer_MetalBeam(TEXT("Layer_Object"))))
		return E_FAIL;
	if (FAILED(Ready_Layer_MetalLadder(TEXT("Layer_Object"))))
		return E_FAIL;
	if (FAILED(Ready_Layer_Pillar(TEXT("Layer_Object"))))
		return E_FAIL;
	if (FAILED(Ready_Layer_WaterTower(TEXT("Layer_Object"))))
		return E_FAIL;
	if (FAILED(Ready_Layer_CrateSet(TEXT("Layer_Object"))))
		return E_FAIL;
	if (FAILED(Ready_Layer_BarCounter(TEXT("Layer_Object"))))
		return E_FAIL;
	if (FAILED(Ready_Layer_BedGarbage(TEXT("Layer_Object"))))
		return E_FAIL;
	if (FAILED(Ready_Layer_Boat(TEXT("Layer_Object"))))
		return E_FAIL;
	if (FAILED(Ready_Layer_Dropship(TEXT("Layer_Object"))))
		return E_FAIL;
	if (FAILED(Ready_Layer_DropzoneWall(TEXT("Layer_DropzoneWall"))))
		return E_FAIL;
	if (FAILED(Ready_Layer_JakobsTank(TEXT("Layer_Object"))))
		return E_FAIL;
	if (FAILED(Ready_Layer_LynchwoodHotel(TEXT("Layer_Object"))))
		return E_FAIL;
	if (FAILED(Ready_Layer_Yurt(TEXT("Layer_Object"))))
		return E_FAIL;
	if (FAILED(Ready_Layer_Yurt2(TEXT("Layer_Object"))))
		return E_FAIL;

#pragma endregion

	Safe_Release(pGameInstance);

	return S_OK;
}

void CLevel_GamePlay::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (KEYDOWN('P'))
		g_bGamePlayEnd = true;

	if (g_bGamePlayEnd == true)
	{
		if (FAILED(pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_ENDING))))
		{
			Safe_Release(pGameInstance);
			return;
		}
	}

	if (g_bFirstDoorOpened)
	{
		Create_Wave();
		g_bFirstDoorOpened = false;
	}
	if (g_bSecondDoorOpened)
	{
		CSoundMgr::Get_Instance()->StopAll();
		//CSoundMgr::Get_Instance()->PlayBGM(L"BATTLE_BGM.ogg", 1.f);

		g_bBossWave = true;
		//Create_Thresher();

		//Create_SpiderAnt();


		g_bSecondDoorOpened = false;
	}
	if (g_bSpiderAntDead)
	{
		m_fCreateTimer += fTimeDelta;
		if (m_fCreateTimer > 1.5f)
		{
			//Create_Skag();
			m_fCreateTimer = 0.f;
			g_bSpiderAntDead = false;
		}
	}
	if (g_bSkagDead)
	{
		m_fCreateTimer += fTimeDelta;
		if (m_fCreateTimer > 1.5f)
		{
			CSoundMgr::Get_Instance()->StopAll();
			CSoundMgr::Get_Instance()->PlayBGM(L"Thresher.mp3", 1.f);


			m_fCreateTimer = 0.f;
			g_bSkagDead = false;
		}
	}

	/*if (g_bSpiderAntScene)
	{
		if (g_bMonsterScene)
		{
			if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_CutScene"),
				TEXT("Prototype_GameObject_CutScene"))))
				return;
		}
		
	}
	else if (g_bSkagScene)
	{
		if (g_bMonsterScene)
		{
			if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_CutScene"),
				TEXT("Prototype_GameObject_CutScene"))))
				return;
		}
	}
	else if (g_bThresherScene)
	{
		if (g_bMonsterScene)
		{
			if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_CutScene"),
				TEXT("Prototype_GameObject_CutScene"))))
				return;
		}
	}*/

	if (g_bMonsterCollidedMouse || g_bPsychoCollidedMouse || g_bPsychoCollidedMouse2 || g_bPsychoCollidedMouse3 || 
		g_bGoliathCollidedMouse|| g_bGoliathCollidedMouse2|| g_bGoliathCollidedMouse3)
	{
		if (m_iNumber == 0)
		{
			if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_MonsterHpBar"),
				TEXT("Prototype_GameObject_MonsterHpBar"))))
				return;

			m_iNumber++;
		}
	}
	if (g_bDestroyedName)
	{
		m_iNumber = 0;
		g_bDestroyedName = false;
	}

	if (g_bInteractionItem && g_InteractionCount == 0)
	{
		if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_InteractionItem"),
			TEXT("Prototype_GameObject_InteractionFont"))))
			return;

		if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_InteractionItem"),
			TEXT("Prototype_GameObject_Key_G"))))
			return;

		g_InteractionCount++;
	}

	if (g_bInteractionNPC && g_InteractionCount == 0)
	{
		if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_InteractionFont"),
			TEXT("Prototype_GameObject_InteractionFont"))))
			return;

		if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Key_G"),
			TEXT("Prototype_GameObject_Key_G"))))
			return;

		if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_TextFont"),
			TEXT("Prototype_GameObject_TextFont"))))
			return;

		g_InteractionCount++;

	}

	if (g_bBossWave)
	{
		m_fTimer += fTimeDelta;
		if (m_fTimer > 50.f)
		{
			CSoundMgr::Get_Instance()->PlaySoundW(L"warp.mp3", 1.f);

			if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_TreasureChest"),
				TEXT("Prototype_GameObject_TreasureChest"))))
				return;
			m_fTimer = 0.f;
		}
		
	}

	Safe_Release(pGameInstance);

}

void CLevel_GamePlay::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	SetWindowText(g_hWnd, TEXT("게임플레이레벨입니다."));
}

HRESULT CLevel_GamePlay::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Lights()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	LIGHTDESC		LightDesc;

	/* 방향성 광원을 추가한다. */
	ZeroMemory(&LightDesc, sizeof LightDesc);

	LightDesc.eType = LIGHTDESC::TYPE_DIRECTIONAL;
	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);

	LightDesc.vDiffuse = _float4(0.8f, 0.8f, 0.8f, 1.f);
	LightDesc.vAmbient = _float4(0.8f, 0.8f, 0.8f, 1.f);
	LightDesc.vSpecular = _float4(0.7f, 0.7f, 0.7f, 1.f);

	if (FAILED(pGameInstance->Add_Lights(m_pDevice, m_pContext, LightDesc)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Camera(const wstring & strLayerTag)
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
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, strLayerTag,
		TEXT("Prototype_GameObject_Camera_Free"), &CameraFreeDesc)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_BackGround(const wstring & strLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	/* For.Terrain */
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, strLayerTag,
		TEXT("Prototype_GameObject_Terrain"))))
		return E_FAIL;

	/* For.SKy */
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, strLayerTag,
		TEXT("Prototype_GameObject_Sky"))))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;

}

HRESULT CLevel_GamePlay::Ready_Layer_Effect(const wstring & strLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	///* For.Effect */

	//for (size_t i = 0; i < 30; i++)
	//{
	//	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, strLayerTag,
	//		TEXT("Prototype_GameObject_Effect"))))
	//		return E_FAIL;

	//}
	
	Safe_Release(pGameInstance);
	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_UI(const wstring& strLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, strLayerTag,
		TEXT("Prototype_GameObject_HpBar"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, strLayerTag,
		TEXT("Prototype_GameObject_HpBorder"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, strLayerTag,
		TEXT("Prototype_GameObject_HpIcon"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, strLayerTag,
		TEXT("Prototype_GameObject_GunBar"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, strLayerTag,
		TEXT("Prototype_GameObject_GunBorder"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, strLayerTag,
		TEXT("Prototype_GameObject_Sight"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, strLayerTag,
		TEXT("Prototype_GameObject_GunIcon"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, strLayerTag,
		TEXT("Prototype_GameObject_ShieldBar"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, strLayerTag,
		TEXT("Prototype_GameObject_ShieldBorder"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, strLayerTag,
		TEXT("Prototype_GameObject_ShieldIcon"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, strLayerTag,
		TEXT("Prototype_GameObject_UIFont"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, strLayerTag,
		TEXT("Prototype_GameObject_EXPBar"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, strLayerTag,
		TEXT("Prototype_GameObject_EXPBorder"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, strLayerTag,
		TEXT("Prototype_GameObject_MissionBox"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, strLayerTag,
		TEXT("Prototype_GameObject_MissionFont"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, strLayerTag,
		TEXT("Prototype_GameObject_TownMonsterHpBar"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, strLayerTag,
		TEXT("Prototype_GameObject_ThresherUI"))))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

void CLevel_GamePlay::Create_Wave()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CPsycho::MONSTERDESC Desc;
	Desc.iNumber = 1;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Psycho"),
		TEXT("Prototype_GameObject_Psycho"), &Desc)))
		return;

	/*CPsycho2::MONSTERDESC Desc2;
	Desc2.iNumber = 2;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Psycho2"),
		TEXT("Prototype_GameObject_Psycho2"), &Desc2)))
		return;

	CPsycho3::MONSTERDESC Desc3;
	Desc3.iNumber = 3;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Psycho3"),
		TEXT("Prototype_GameObject_Psycho3"), &Desc3)))
		return;*/

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Goliath"),
		TEXT("Prototype_GameObject_Goliath"))))
		return;

	/*if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Goliath2"),
		TEXT("Prototype_GameObject_Goliath2"))))
		return;*/

	Safe_Release(pGameInstance);
}

void CLevel_GamePlay::Create_Skag()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CSkag::MONSTERDESC Desc;
	Desc.CreatePos = XMVectorSet(90.f, 0.f, 90.f, 1.f);
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Skag"),
		TEXT("Prototype_GameObject_Skag"), &Desc)))
		return;

	Safe_Release(pGameInstance);
}

void CLevel_GamePlay::Create_SpiderAnt()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_SpiderAnt"),
		TEXT("Prototype_GameObject_SpiderAnt"))))
		return;

	Safe_Release(pGameInstance);
}

void CLevel_GamePlay::Create_Thresher()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Thresher"),
		TEXT("Prototype_GameObject_Thresher"))))
		return;

	Safe_Release(pGameInstance);
}

HRESULT CLevel_GamePlay::Ready_Layer_IceBlock01(const wstring& strLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CIceBlock01::OBJECTINFO Desc;

	ZeroMemory(&Desc, sizeof(CIceBlock01::OBJECTINFO));
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
			if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, strLayerTag,
				TEXT("Prototype_GameObject_IceBlock01"), &Desc)))
				return E_FAIL;
			m_iCount++;

		}
	}
	CloseHandle(hFile);

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Large_Rock1(const wstring& strLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CLarge_Rock1::OBJECTINFO Desc;

	ZeroMemory(&Desc, sizeof(CLarge_Rock1::OBJECTINFO));
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
			if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, strLayerTag,
				TEXT("Prototype_GameObject_Large_Rock1"), &Desc)))
				return E_FAIL;
			m_iCount++;

		}
	}
	CloseHandle(hFile);

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_DamWall(const wstring& strLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CDamWall::OBJECTINFO Desc;

	_ulong		dwByte = 0;
	HANDLE		hFile = CreateFile(TEXT("../../Client/Bin/Resources/Map/DamWall.txt"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
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
			if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, strLayerTag,
				TEXT("Prototype_GameObject_DamWall"), &Desc)))
				return E_FAIL;
			m_iCount++;

		}
	}
	CloseHandle(hFile);

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Container(const wstring& strLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CContainer::OBJECTINFO Desc;

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
			if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, strLayerTag,
				TEXT("Prototype_GameObject_Container"), &Desc)))
				return E_FAIL;
			m_iCount++;

		}
	}
	CloseHandle(hFile);

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_TunnelDoor(const wstring& strLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CTunnelDoor::OBJECTINFO Desc;

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
			if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, strLayerTag,
				TEXT("Prototype_GameObject_TunnelDoor"), &Desc)))
				return E_FAIL;
			m_iCount++;

		}
	}
	CloseHandle(hFile);

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_CircularSupport(const wstring& strLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CCircularSupport::OBJECTINFO Desc;

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
			if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, strLayerTag,
				TEXT("Prototype_GameObject_CircularSupport"), &Desc)))
				return E_FAIL;
			m_iCount++;

		}
	}
	CloseHandle(hFile);

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_House(const wstring& strLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CHouse::OBJECTINFO Desc;

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

		wstring targetString = L"obj_House";
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

		if (Desc.strName == L"obj_House")
		{
			if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, strLayerTag,
				TEXT("Prototype_GameObject_House"), &Desc)))
				return E_FAIL;
			m_iCount++;

		}
	}
	CloseHandle(hFile);

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_MetalBeam(const wstring& strLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CMetalBeam::OBJECTINFO Desc;

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
			if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, strLayerTag,
				TEXT("Prototype_GameObject_MetalBeam"), &Desc)))
				return E_FAIL;
			m_iCount++;

		}
	}
	CloseHandle(hFile);

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_MetalLadder(const wstring& strLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CMetalLadder::OBJECTINFO Desc;

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
			if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, strLayerTag,
				TEXT("Prototype_GameObject_MetalLadder"), &Desc)))
				return E_FAIL;
			m_iCount++;

		}
	}
	CloseHandle(hFile);

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Pillar(const wstring& strLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CPillar::OBJECTINFO Desc;

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
			if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, strLayerTag,
				TEXT("Prototype_GameObject_Pillar"), &Desc)))
				return E_FAIL;
			m_iCount++;

		}
	}
	CloseHandle(hFile);

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_CrateSet(const wstring& strLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CCrateSet::OBJECTINFO Desc;

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
			if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, strLayerTag,
				TEXT("Prototype_GameObject_CrateSet"), &Desc)))
				return E_FAIL;
			m_iCount++;

		}
	}
	CloseHandle(hFile);

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_WaterTower(const wstring& strLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CWaterTower::OBJECTINFO Desc;

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
			if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, strLayerTag,
				TEXT("Prototype_GameObject_WaterTower"), &Desc)))
				return E_FAIL;
			m_iCount++;

		}
	}
	CloseHandle(hFile);

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_BarCounter(const wstring& strLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CBarCounter::OBJECTINFO Desc;

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
			if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, strLayerTag,
				TEXT("Prototype_GameObject_BarCounter"), &Desc)))
				return E_FAIL;
			m_iCount++;

		}
	}
	CloseHandle(hFile);

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_BedGarbage(const wstring& strLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CBedGarbage::OBJECTINFO Desc;

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
			if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, strLayerTag,
				TEXT("Prototype_GameObject_BedGarbage"), &Desc)))
				return E_FAIL;
			m_iCount++;

		}
	}
	CloseHandle(hFile);

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Boat(const wstring& strLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CBoat::OBJECTINFO Desc;

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
			if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, strLayerTag,
				TEXT("Prototype_GameObject_Boat"), &Desc)))
				return E_FAIL;
			m_iCount++;

		}
	}
	CloseHandle(hFile);

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Dropship(const wstring& strLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CDropship::OBJECTINFO Desc;

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
			if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, strLayerTag,
				TEXT("Prototype_GameObject_Dropship"), &Desc)))
				return E_FAIL;
			m_iCount++;

		}
	}
	CloseHandle(hFile);

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_DropzoneWall(const wstring& strLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CDropzoneWall::OBJECTINFO Desc;

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
			if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, strLayerTag,
				TEXT("Prototype_GameObject_DropzoneWall"), &Desc)))
				return E_FAIL;
			m_iCount++;

		}
	}
	CloseHandle(hFile);

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_JakobsTank(const wstring& strLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CJakobsTank::OBJECTINFO Desc;

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
			if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, strLayerTag,
				TEXT("Prototype_GameObject_JakobsTank"), &Desc)))
				return E_FAIL;
			m_iCount++;

		}
	}
	CloseHandle(hFile);

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_LynchwoodHotel(const wstring& strLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CLynchwoodHotel::OBJECTINFO Desc;

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
			if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, strLayerTag,
				TEXT("Prototype_GameObject_LynchwoodHotel"), &Desc)))
				return E_FAIL;
			m_iCount++;

		}
	}
	CloseHandle(hFile);

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Yurt(const wstring& strLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CYurt::OBJECTINFO Desc;

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
			if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, strLayerTag,
				TEXT("Prototype_GameObject_Yurt"), &Desc)))
				return E_FAIL;
			m_iCount++;

		}
	}
	CloseHandle(hFile);

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Yurt2(const wstring& strLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CYurt2::OBJECTINFO Desc;

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
			if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, strLayerTag,
				TEXT("Prototype_GameObject_Yurt2"), &Desc)))
				return E_FAIL;
			m_iCount++;

		}
	}
	CloseHandle(hFile);

	Safe_Release(pGameInstance);

	return S_OK;
}

CLevel_GamePlay * CLevel_GamePlay::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_GamePlay*	pInstance = new CLevel_GamePlay(pDevice, pContext);	

	if (FAILED(pInstance->Initialize()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Created : CLevel_GamePlay");
	}

	return pInstance;
}

void CLevel_GamePlay::Free()
{
	__super::Free();
}