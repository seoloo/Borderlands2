#include "stdafx.h"
#include "..\Public\Loader.h"

#include "GameInstance.h"

#include "BackGround.h"
#include "LoadingUI.h"

/* Object */
#include "Terrain.h"
#include "Sky.h"
#include "Player.h"
#include "TownPlayer.h"
#include "Thresher.h"
#include "Thresher_Arm.h"
#include "Thresher_Arm2.h"
#include "Thresher_Melee.h"
#include "Missile.h"
#include "HealthVial.h"
#include "Pistol.h"
#include "Camera_Free.h"
#include "Camera_OnBone.h"
#include "Camera_Pistol.h"
#include "SpiderAnt.h"
#include "Stalker.h"
#include "Goliath.h"
#include "Goliath2.h"
#include "Goliath3.h"
#include "Psycho.h"
#include "Psycho2.h"
#include "Psycho3.h"
#include "Axe.h"
#include "Psycho_Face.h"
#include "Hammerlock.h"
#include "ClapTrap.h"
#include "Skag.h"
#include "Horn.h"
#include "TreasureChest.h"
#include "AmmoPickUp.h"
#include "Roland.h"
#include "Lilth.h"
#include "Mordecai.h"

/* Static_Object */
#include "AridDistanceTerrain.h"
#include "CrateSet.h"
#include "CircularSupport.h"
#include "IceBlock01.h"
#include "Large_Rock1.h"
#include "DamWall.h"
#include "Container.h"
#include "TunnelDoor.h"
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

/*Collider*/
#include "Thresher_Collider.h"
#include "SpiderAnt_Collider.h"
#include "SpiderAnt_LegCollider.h"
#include "Stalker_Collider.h"
#include "Stalker_HeaderCollider.h"
#include "Stalker_TailCollider.h"
#include "SpiderAnt_HeadCollider.h"
#include "Thresher_TentacleCollider.h"
#include "Thresher_ArmCollider.h"
#include "Thresher_Arm2Collider.h"
#include "Thresher_MeleeCollider.h"
#include "Goliath_HeadCollider.h"
#include "Goliath_ArmCollider.h"
#include "Goliath_BodyCollider.h"
#include "Goliath_LegCollider.h"
#include "Psycho_BodyCollider.h"
#include "Psycho_LegCollider.h"
#include "Thresher_ArmsTopCollider.h"
#include "Skag_HeadCollider.h"
#include "Skag_Collider.h"
#include "Skag_ToungueCollider.h"

/* UI */
#include "HpBar.h"
#include "HpIcon.h"
#include "HpBorder.h"
#include "GunBar.h"
#include "GunIcon.h"
#include "Sight.h"
#include "Zoom.h"
#include "UIFont.h"
#include "ShieldBar.h"
#include "ShieldIcon.h"
#include "ShieldBorder.h"
#include "GunBorder.h"
#include "Infinity.h"
#include "TextUI.h"
#include "MissionBox.h"
#include "MissionFont.h"
#include "EXPBorder.h"
#include "EXPBar.h"
#include "MonsterHpBar.h"
#include "ThresherUI.h"
#include "InteractionFont.h"
#include "Key_G.h"
#include "TextFont.h"
#include "ShieldUI.h"
#include "AttackedUI.h"
#include "CutScene.h"
#include "DamageFont.h"
#include "Critical.h"
#include "AreaFont.h"

/*Effect*/
#include "Muzzle.h"
#include "Snow_Blue.h"
#include "Blood.h"
#include "Rock.h"
#include "AppearRock.h"
#include "Snowing.h"
#include "Catridge.h"
#include "Monster_Fire.h"
#include "Fog.h"
#include "Blast.h"
#include "Explosion.h"
#include "Smoke.h"
#include "Dust.h"
#include "Lightning.h"
#include "EMPBlue.h"
#include "EMPRed.h"
#include "Vapor.h"
#include "Energy.h"

CLoader::CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice) 
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

unsigned int APIENTRY ThreadEntry(void* pArg)
{
	CLoader*		pLoader = (CLoader*)pArg;

	/*  모든 컴객체를 초기화하낟. */
	CoInitializeEx(nullptr, 0);

	if (FAILED(pLoader->LoadingForNextLevel()))
		return 1;

	return 0;
}

HRESULT CLoader::Initialize(LEVEL eNextLevel)
{
	m_eNextLevel = eNextLevel;	

	InitializeCriticalSection(&m_CriticalSection);

	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, ThreadEntry, this, 0, nullptr);
	if (0 == m_hThread)
		return E_FAIL;	

	return S_OK;
}

HRESULT CLoader::LoadingForNextLevel()
{
	EnterCriticalSection(&m_CriticalSection);	

	HRESULT		hr = { 0 };

	switch (m_eNextLevel)
	{
	case LEVEL_LOGO:
		hr = Loading_ForLogo();
		break;
	case LEVEL_TOWN:
		hr = Loading_ForTown();
		break;
	case LEVEL_GAMEPLAY:
		hr = Loading_ForGamePlay();
		break;
	case LEVEL_ENDING:
		hr = Loading_ForEnding();
		break;
	}

	if (FAILED(hr))
		goto except;		

	LeaveCriticalSection(&m_CriticalSection);
	return S_OK;

except:
	LeaveCriticalSection(&m_CriticalSection);
	return E_FAIL;
}

HRESULT CLoader::Loading_ForLogo()
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	lstrcpy(m_szLoading, TEXT("텍스쳐를 로딩 중입니다."));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_BackGround"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Borderland/Logo/Logo.dds")))))
		return E_FAIL;

	lstrcpy(m_szLoading, TEXT("모델을 로딩 중입니다."));
	

	lstrcpy(m_szLoading, TEXT("셰이더를 로딩 중입니다."));
	

	lstrcpy(m_szLoading, TEXT("객체원형 로딩 중입니다."));

	/* For.Prototype_GameObject_BackGround */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BackGround"),
		CBackGround::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	lstrcpy(m_szLoading, TEXT("로딩이 완료되었습니다."));

	Safe_Release(pGameInstance);

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_ForTown()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	lstrcpy(m_szLoading, TEXT("텍스쳐를 로딩 중입니다."));

	/* For.Prototype_Component_Texture_Terrain */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOWN, TEXT("Prototype_Component_Texture_Terrain"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Borderland/Terrain/SnowDrift_01_Dif.dds")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Sky */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOWN, TEXT("Prototype_Component_Texture_Sky"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Borderland/SkyBox/Sky_%d.dds"), 4))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Mask */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOWN, TEXT("Prototype_Component_Texture_Mask"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Mask.dds")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Brush */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOWN, TEXT("Prototype_Component_Texture_Brush"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Brush.png")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOWN, TEXT("Prototype_Component_Texture_DissolveTex"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Borderland/Effect/Noise.png")))))
		return E_FAIL;

#pragma region Effect
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOWN, TEXT("Prototype_Component_Texture_Snowing"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Borderland/Effect/Snowing.png")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOWN, TEXT("Prototype_Component_Texture_Muzzle"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Borderland/Effect/Muzzle%d.png"), 4))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOWN, TEXT("Prototype_Component_Texture_Catridge"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Borderland/Effect/Catridge.png")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOWN, TEXT("Prototype_Component_Texture_Fog"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Borderland/Effect/Fog.png"),1))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOWN, TEXT("Prototype_Component_Texture_Blast"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Borderland/Effect/Blast%d.png"), 9))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOWN, TEXT("Prototype_Component_Texture_Explosion"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Borderland/Effect/Explosion%d.png"), 16))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOWN, TEXT("Prototype_Component_Texture_Rock"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Borderland/Effect/Rock.png")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOWN, TEXT("Prototype_Component_Texture_Smoke"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Borderland/Effect/Smoke%d.png"), 8))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOWN, TEXT("Prototype_Component_Texture_Lightning"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Borderland/Effect/Lightning%d.png"), 4))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOWN, TEXT("Prototype_Component_Texture_Dust"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Borderland/Effect/Dust%d.png"), 32))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOWN, TEXT("Prototype_Component_Texture_EMPBlue"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Borderland/Effect/EMP%d.png"), 4))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOWN, TEXT("Prototype_Component_Texture_EMPRed"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Borderland/Effect/EMPRed%d.png"), 4))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOWN, TEXT("Prototype_Component_Texture_Energy"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Borderland/Effect/Energy%d.png"), 4))))
		return E_FAIL;

	
#pragma endregion	

#pragma region UI Texture
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOWN, TEXT("Prototype_Component_Texture_HpBar"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Borderland/UI/HP_Bar.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOWN, TEXT("Prototype_Component_Texture_HpIcon"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Borderland/UI/HP_Icon.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOWN, TEXT("Prototype_Component_Texture_HpBorder"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Borderland/UI/HP_Border.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOWN, TEXT("Prototype_Component_Texture_GunBar"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Borderland/UI/Gun_Bar.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOWN, TEXT("Prototype_Component_Texture_GunIcon"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Borderland/UI/SMG.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOWN, TEXT("Prototype_Component_Texture_Sight"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Borderland/UI/Tex_Hyperion_Sight_SMG.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOWN, TEXT("Prototype_Component_Texture_Zoom"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Borderland/UI/Zoom.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOWN, TEXT("Prototype_Component_Texture_ShieldBar"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Borderland/UI/Shield_Bar.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOWN, TEXT("Prototype_Component_Texture_ShieldBorder"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Borderland/UI/Shield_Border.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOWN, TEXT("Prototype_Component_Texture_ShieldIcon"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Borderland/UI/Shield_Icon.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOWN, TEXT("Prototype_Component_Texture_GunBorder"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Borderland/UI/Gun_Border.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOWN, TEXT("Prototype_Component_Texture_Infinity"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Borderland/UI/Infinity.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOWN, TEXT("Prototype_Component_Texture_TextUI"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Borderland/UI/TextUI.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOWN, TEXT("Prototype_Component_Texture_MissionBox"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Borderland/UI/MissionBox.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOWN, TEXT("Prototype_Component_Texture_MissionClearBox"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Borderland/UI/MissionClearBox.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOWN, TEXT("Prototype_Component_Texture_EXPBorder"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Borderland/UI/EXPBorder.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOWN, TEXT("Prototype_Component_Texture_EXPBar"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Borderland/UI/EXPBar.dds")))))
		return E_FAIL;


	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOWN, TEXT("Prototype_Component_Texture_Key_G"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Borderland/UI/Key_G.dds")))))
		return E_FAIL;


	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOWN, TEXT("Prototype_Component_Texture_MonsterHpBar"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Borderland/UI/MonsterHpBar.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOWN, TEXT("Prototype_Component_Texture_ShieldUI"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Borderland/UI/ShieldUI.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOWN, TEXT("Prototype_Component_Texture_AttackedUI"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Borderland/UI/AttackedUI.dds")))))
		return E_FAIL;



	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOWN, TEXT("Prototype_Component_Texture_ThresherScene"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Borderland/Scene/ThresherScene.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOWN, TEXT("Prototype_Component_Texture_SkagScene"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Borderland/Scene/SkagScene.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOWN, TEXT("Prototype_Component_Texture_SpiderAntScene"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Borderland/Scene/SpiderAntScene.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOWN, TEXT("Prototype_Component_Texture_ThresherTex"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Borderland/Scene/ThresherTex.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOWN, TEXT("Prototype_Component_Texture_SkagTex"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Borderland/Scene/SkagTex.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOWN, TEXT("Prototype_Component_Texture_SpiderAntTex"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Borderland/Scene/SpiderAntTex.dds")))))
		return E_FAIL;

#pragma endregion

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOWN, TEXT("Prototype_Component_Texture_DamageFont"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Borderland/UI/Number%d.dds"), 10))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOWN, TEXT("Prototype_Component_Texture_Critical"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Borderland/UI/Critical%d.dds"), 2))))
		return E_FAIL;

	lstrcpy(m_szLoading, TEXT("모델을 로딩 중입니다."));

	/* For.Prototype_Component_VIBuffer_Terrain */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOWN, TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Borderland/Terrain/Flatland.bmp")))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Cube*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOWN, TEXT("Prototype_Component_VIBuffer_Cube"),
		CVIBuffer_Cube::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Rect_Instance*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOWN, TEXT("Prototype_Component_VIBuffer_Rect_Instance"),
		CVIBuffer_Rect_Instance::Create(m_pDevice, m_pContext, 300))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Point_Instance*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOWN, TEXT("Prototype_Component_VIBuffer_Point_Instance"),
		CVIBuffer_Point_Instance::Create(m_pDevice, m_pContext, 100))))
		return E_FAIL;

	_matrix		PivotMatrix = XMMatrixIdentity();

	/* For.Prototype_Component_Model_Player */
	PivotMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOWN, TEXT("Prototype_Component_Model_TownPlayer"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Borderland/Player/Player_Pistol.fbx", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Pistol */
	PivotMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f) * XMMatrixRotationY(XMConvertToRadians(82.f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOWN, TEXT("Prototype_Component_Model_Pistol"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Borderland/Weapon/SMG.fbx", PivotMatrix))))
		return E_FAIL;

	PivotMatrix = XMMatrixScaling(0.007f, 0.008f, 0.008f) * XMMatrixRotationY(XMConvertToRadians(255.0f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOWN, TEXT("Prototype_Component_Model_Hammerlock"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Borderland/NPC/Hammerlock.fbx", PivotMatrix))))
		return E_FAIL;

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationZ(XMConvertToRadians(270.0f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOWN, TEXT("Prototype_Component_Model_ClapTrap"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Borderland/NPC/ClapTrap.fbx", PivotMatrix))))
		return E_FAIL;

	PivotMatrix = XMMatrixScaling(0.007f, 0.007f, 0.007f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOWN, TEXT("Prototype_Component_Model_Roland"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Borderland/NPC/Roland.fbx", PivotMatrix))))
		return E_FAIL;

	PivotMatrix = XMMatrixScaling(0.007f, 0.007f, 0.007f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOWN, TEXT("Prototype_Component_Model_Lilth"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Borderland/NPC/Lilth.fbx", PivotMatrix))))
		return E_FAIL;

	PivotMatrix = XMMatrixScaling(0.007f, 0.007f, 0.007f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOWN, TEXT("Prototype_Component_Model_Mordecai"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Borderland/NPC/Mordecai.fbx", PivotMatrix))))
		return E_FAIL;

	PivotMatrix = XMMatrixScaling(0.004f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOWN, TEXT("Prototype_Component_Model_TreasureChest"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Borderland/Item/TreasureChest.fbx", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_HealthVial */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOWN, TEXT("Prototype_Component_Model_HealthVial"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Borderland/Item/HealthVial.fbx"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOWN, TEXT("Prototype_Component_Model_AmmoPickUp"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Borderland/Item/AmmoPickUp.fbx"))))
		return E_FAIL;

#pragma region StaicObject
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOWN, TEXT("Prototype_Component_Model_DamWall"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Borderland/Thresher_Static/obj_DamWall.fbx", PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOWN, TEXT("Prototype_Component_Model_MetalBeam"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Borderland/Thresher_Static/obj_MetalBeam.fbx", PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOWN, TEXT("Prototype_Component_Model_MetalLadder"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Borderland/Thresher_Static/obj_MetalLadder.fbx", PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOWN, TEXT("Prototype_Component_Model_WaterTower"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Borderland/Thresher_Static/obj_WaterTower.fbx", PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOWN, TEXT("Prototype_Component_Model_Dropship"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Borderland/Thresher_Static/obj_Dropship.fbx", PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOWN, TEXT("Prototype_Component_Model_Yurt"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Borderland/Thresher_Static/obj_Yurt.fbx", PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOWN, TEXT("Prototype_Component_Model_Yurt2"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Borderland/Thresher_Static/obj_Yurt2.fbx", PivotMatrix))))
		return E_FAIL;

#pragma endregion

	lstrcpy(m_szLoading, TEXT("셰이더를 로딩 중입니다."));

	/* For.Prototype_Component_Shader_VtxNorTex */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOWN, TEXT("Prototype_Component_Shader_VtxNorTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxMesh */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOWN, TEXT("Prototype_Component_Shader_VtxMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxMesh.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxAnimMesh */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOWN, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnimMesh.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxCubeTex */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOWN, TEXT("Prototype_Component_Shader_VtxCubeTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxCubeTex.hlsl"), VTXCUBETEX::Elements, VTXCUBETEX::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxRectInstance */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOWN, TEXT("Prototype_Component_Shader_VtxRectInstance"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxRectInstance.hlsl"), VTXRECTINSTANCE::Elements, VTXRECTINSTANCE::iNumElements))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOWN, TEXT("Prototype_Component_Shader_VtxPointInstance"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPointInstance.hlsl"), VTXPOINTINSTANCE::Elements, VTXPOINTINSTANCE::iNumElements))))
		return E_FAIL;

	lstrcpy(m_szLoading, TEXT("네비게이션 로딩 중입니다."));

	/* For.Prototype_Component_Navigation */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOWN, TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Map/NaviTownTest.txt"), 3))))
		return E_FAIL;

	lstrcpy(m_szLoading, TEXT("객체원형 로딩 중입니다."));

	/* For.Terrain */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TownTerrain"),
		CTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Sky */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TownSky"),
		CSky::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Camera_Free */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TownCamera_Free"),
		CCamera_Free::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Camera_OnBone */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TownCamera_OnBone"),
		CCamera_OnBone::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma region Object
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TownRoland"),
		CRoland::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TownLilth"),
		CLilth::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TownMordecai"),
		CMordecai::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TownPlayer"),
		CTownPlayer::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TownPistol"),
		CPistol::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TownHammerlock"),
		CHammerlock::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ClapTrap"),
		CClapTrap::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	

	/*if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TownStalker"),
		CStalker::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TownSpiderAnt"),
		CSpiderAnt::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TownSkag"),
		CSkag::Create(m_pDevice, m_pContext))))
		return E_FAIL;*/

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TownHealthVial"),
		CHealthVial::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TownTreasureChest"),
		CTreasureChest::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TownAmmoPickUp"),
		CAmmoPickUp::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma endregion

#pragma region Static_Object

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TownDamWall"),
		CDamWall::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TownMetalBeam"),
		CMetalBeam::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TownWaterTower"),
		CWaterTower::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TownDropship"),
		CDropship::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TownYurt"),
		CYurt::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TownYurt2"),
		CYurt2::Create(m_pDevice, m_pContext))))
		return E_FAIL;


#pragma endregion

#pragma region UI
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TownHpBar"),
		CHpBar::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TownHpIcon"),
		CHpIcon::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TownHpBorder"),
		CHpBorder::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TownGunBar"),
		CGunBar::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TownGunIcon"),
		CGunIcon::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TownSight"),
		CSight::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TownZoom"),
		CZoom::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TownUIFont"),
		CUIFont::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TownShieldBar"),
		CShieldBar::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TownShieldBorder"),
		CShieldBorder::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TownShieldIcon"),
		CShieldIcon::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TownGunBorder"),
		CGunBorder::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TownInfinity"),
		CInfinity::Create(m_pDevice, m_pContext))))
		return E_FAIL;*/

	/*if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TownTextUI"),
		CTextUI::Create(m_pDevice, m_pContext))))
		return E_FAIL;*/

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TownMissionBox"),
		CMissionBox::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TownMissionFont"),
		CMissionFont::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TownEXPBar"),
		CEXPBar::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TownEXPBorder"),
		CEXPBorder::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TownMonsterHpBar"),
		CMonsterHpBar::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TownInteractionFont"),
		CInteractionFont::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TownKey_G"),
		CKey_G::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TownTextFont"),
		CTextFont::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TownDamageFont"),
		CDamageFont::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TownShieldUI"),
		CShieldUI::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TownAttackedUI"),
		CAttackedUI::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TownCutScene"),
		CCutScene::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TownCritical"),
		CCritical::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TownAreaFont"),
		CAreaFont::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion

	
#pragma region Effect
	/*if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TownSnowing"),
		CSnowing::Create(m_pDevice, m_pContext))))
		return E_FAIL;*/

	/* For.Prototype_GameObject_Muzzle */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TownMuzzle"),
		CMuzzle::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	/*if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Catridge"),
		CCatridge::Create(m_pDevice, m_pContext))))
		return E_FAIL;*/

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TownFog"),
		CFog::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TownBlast"),
		CBlast::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TownExplosion"),
		CExplosion::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TownSmoke"),
		CSmoke::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TownLightning"),
		CLightning::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TownDust"),
		CDust::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TownEMPBlue"),
		CEMPBlue::Create(m_pDevice, m_pContext))))
		return E_FAIL;*/

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TownEMPBlue"),
		CEMPBlue::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TownEnergy"),
		CEnergy::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TownAppearRock"),
		CAppearRock::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
#pragma endregion

	lstrcpy(m_szLoading, TEXT("로딩이 완료되었습니다."));

	Safe_Release(pGameInstance);

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_ForGamePlay()
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	lstrcpy(m_szLoading, TEXT("텍스쳐를 로딩 중입니다."));

#pragma region Texture
	/* For.Prototype_Component_Texture_Terrain */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Terrain"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Borderland/Terrain/SnowDrift_01_Dif.dds")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Sky */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Sky"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Borderland/SkyBox/Sky_%d.dds"), 4))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Mask */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Mask"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Mask.dds")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Brush */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Brush"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Brush.png")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Muzzle"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Borderland/Effect/Muzzle%d.png"), 4))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Snow_Blue"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Borderland/Effect/Snow.png")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Snowing"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Borderland/Effect/Snowing.png")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Rock"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Borderland/Effect/Rock.png")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Blood_Decal */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Blood"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Borderland/Effect/Blood_Flipbook%d.png"), 24))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Blast"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Borderland/Effect/Blast%d.png"), 9))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Catridge"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Borderland/Effect/Catridge.png")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Monster_Fire"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Borderland/Effect/EmpBlue%d.png"), 4))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Explosion"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Borderland/Effect/Explosion%d.png"), 16))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Smoke"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Borderland/Effect/Smoke%d.png"), 8))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Lightning"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Borderland/Effect/Lightning%d.png"), 4))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Dust"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Borderland/Effect/Dust%d.png"), 32))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_EMPBlue"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Borderland/Effect/EMP%d.png"), 4))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_EMPRed"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Borderland/Effect/EMP%d.png"), 4))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Vapor"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Borderland/Effect/Vapor%d.png"), 33))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Energy"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Borderland/Effect/Energy%d.png"), 4))))
		return E_FAIL;
#pragma endregion

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_DissolveTex"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Borderland/Effect/Noise.png")))))
		return E_FAIL;

#pragma region UI Texture
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_HpBar"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Borderland/UI/HP_Bar.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_HpIcon"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Borderland/UI/HP_Icon.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_HpBorder"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Borderland/UI/HP_Border.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_GunBar"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Borderland/UI/Gun_Bar.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_GunIcon"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Borderland/UI/SMG.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Sight"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Borderland/UI/Tex_Hyperion_Sight_SMG.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Zoom"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Borderland/UI/Zoom.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_ShieldBar"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Borderland/UI/Shield_Bar.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_ShieldBorder"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Borderland/UI/Shield_Border.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_ShieldIcon"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Borderland/UI/Shield_Icon.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_GunBorder"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Borderland/UI/Gun_Border.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Infinity"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Borderland/UI/Infinity.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_TextUI"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Borderland/UI/TextUI.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_MissionBox"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Borderland/UI/MissionBox.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_MissionClearBox"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Borderland/UI/MissionClearBox.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_EXPBar"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Borderland/UI/EXPBar.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_EXPBorder"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Borderland/UI/EXPBorder.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_MonsterHpBar"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Borderland/UI/MonsterHpBar.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Key_G"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Borderland/UI/Key_G.png")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_ShieldUI"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Borderland/UI/ShieldUI.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_AttackedUI"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Borderland/UI/AttackedUI.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_DamageFont"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Borderland/UI/Number%d.dds"), 10))))
		return E_FAIL;
#pragma endregion

	lstrcpy(m_szLoading, TEXT("모델을 로딩 중입니다."));

#pragma region Model
	/* For.Prototype_Component_VIBuffer_Terrain */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Borderland/Terrain/Flatland.bmp")))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Cube*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Cube"),
		CVIBuffer_Cube::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Rect_Instance*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Rect_Instance"),
		CVIBuffer_Rect_Instance::Create(m_pDevice, m_pContext, 300))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Point_Instance*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Point_Instance"),
		CVIBuffer_Point_Instance::Create(m_pDevice, m_pContext, 100))))
		return E_FAIL;

	_matrix		PivotMatrix = XMMatrixIdentity();

	/* For.Prototype_Component_Model_Player */
	PivotMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Player"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Borderland/Player/Player_Pistol.fbx", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Pistol */
	PivotMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f) * XMMatrixRotationY(XMConvertToRadians(82.f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Pistol"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Borderland/Weapon/SMG.fbx", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Thresher */
	PivotMatrix = XMMatrixScaling(0.06f, 0.06f, 0.06f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Thresher"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Borderland/Monster/Thresher.fbx", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_ThresherArm */
	PivotMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(255.0f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_ThresherArm"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Borderland/Monster/Thresher_Arm.fbx", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_ThresherArm2 */
	PivotMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(255.0f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_ThresherArm2"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Borderland/Monster/Thresher_Arm2.fbx", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_ThresherMelee */
	PivotMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(255.0f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_ThresherMelee"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Borderland/Monster/Thresher_Melee.fbx", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_SpiderAnt */
	PivotMatrix = XMMatrixScaling(0.05f, 0.05f, 0.05f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_SpiderAnt"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Borderland/Monster/SpiderAnt.fbx", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Stalker */
	PivotMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Stalker"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Borderland/Monster/Stalker.fbx", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Skag */
	PivotMatrix = XMMatrixScaling(0.07f, 0.07f, 0.07f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Skag"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Borderland/Monster/Skag.fbx", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Goliath */
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Goliath"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Borderland/Monster/Goliath.fbx", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Psycho */
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Psycho"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Borderland/Monster/Psycho.fbx", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Axe */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Axe"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Borderland/Monster/Axe.fbx"))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Psycho_Face */
	PivotMatrix = XMMatrixScaling(1.f, 1.f, 1.f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Psycho_Face"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Borderland/Monster/Psycho_Face.fbx", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Missile */
	/*PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) ;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Missile"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Borderland/Weapon/Missile.fbx", PivotMatrix))))
		return E_FAIL;*/
	/*PivotMatrix = XMMatrixScaling(0.13f, 0.13f, 0.13f);
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_GeoSphere"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Borderland/Monster/GeoSphere.fbx", PivotMatrix))))
		return E_FAIL;

	PivotMatrix = XMMatrixScaling(0.05f, 0.05f, 0.05f) * XMMatrixRotationX(XMConvertToRadians(90.0f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Horn"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Borderland/Monster/Horn.fbx", PivotMatrix))))
		return E_FAIL;*/

	PivotMatrix = XMMatrixScaling(0.007f, 0.008f, 0.008f) * XMMatrixRotationY(XMConvertToRadians(255.0f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Hammerlock"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Borderland/NPC/Hammerlock.fbx", PivotMatrix))))
		return E_FAIL;

	PivotMatrix = XMMatrixScaling(0.004f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_TreasureChest"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Borderland/Item/TreasureChest.fbx", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_HealthVial */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_HealthVial"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Borderland/Item/HealthVial.fbx"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_AmmoPickUp"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Borderland/Item/AmmoPickUp.fbx"))))
		return E_FAIL;

#pragma endregion

#pragma region StaicObject
	/*PivotMatrix = XMMatrixScaling(0.002f, 0.003f, 0.001f);
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_AridDistanceTerrain"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Borderland/Thresher_Static/AridDistanceTerrain.fbx", PivotMatrix))))
		return E_FAIL;*/
	/*
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_IceBlock01"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Borderland/Thresher_Static/IceBlock01.fbx", PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Large_Rock1"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Borderland/Thresher_Static/Large_Rock1.fbx", PivotMatrix))))
		return E_FAIL;*/
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_DamWall"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Borderland/Thresher_Static/obj_DamWall.fbx", PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Container"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Borderland/Thresher_Static/obj_Container.fbx", PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_TunnelDoor"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Borderland/Thresher_Static/obj_TunnelDoor.fbx", PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_House"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Borderland/Thresher_Static/obj_House.fbx", PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_CircularSupport"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Borderland/Thresher_Static/obj_CircularSupport.fbx", PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_CrateSet"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Borderland/Thresher_Static/obj_CrateSet.fbx", PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_MetalBeam"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Borderland/Thresher_Static/obj_MetalBeam.fbx", PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_MetalLadder"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Borderland/Thresher_Static/obj_MetalLadder.fbx", PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Pillar"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Borderland/Thresher_Static/obj_Pillar.fbx", PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_WaterTower"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Borderland/Thresher_Static/obj_WaterTower.fbx", PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_BarCounter"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Borderland/Thresher_Static/obj_BarCounter.fbx", PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_BedGarbage"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Borderland/Thresher_Static/obj_BedGarbage.fbx", PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Boat"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Borderland/Thresher_Static/obj_Boat.fbx", PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Dropship"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Borderland/Thresher_Static/obj_Dropship.fbx", PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_DropzoneWall"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Borderland/Thresher_Static/obj_DropzoneWall.fbx", PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_JakobsTank"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Borderland/Thresher_Static/obj_JakobsTank.fbx", PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_LynchwoodHotel"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Borderland/Thresher_Static/obj_LynchwoodHotel.fbx", PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Yurt"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Borderland/Thresher_Static/obj_Yurt.fbx", PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Yurt2"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Borderland/Thresher_Static/obj_Yurt2.fbx", PivotMatrix))))
		return E_FAIL;

#pragma endregion

	lstrcpy(m_szLoading, TEXT("셰이더를 로딩 중입니다."));

#pragma region Shader

	/* For.Prototype_Component_Shader_VtxNorTex */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxNorTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxMesh */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxMesh.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxAnimMesh */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnimMesh.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxCubeTex */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxCubeTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxCubeTex.hlsl"), VTXCUBETEX::Elements, VTXCUBETEX::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxRectInstance */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxRectInstance"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxRectInstance.hlsl"), VTXRECTINSTANCE::Elements, VTXRECTINSTANCE::iNumElements))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxPointInstance"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPointInstance.hlsl"), VTXPOINTINSTANCE::Elements, VTXPOINTINSTANCE::iNumElements))))
		return E_FAIL;

#pragma endregion

	lstrcpy(m_szLoading, TEXT("네비게이션 로딩 중입니다."));

	/* For.Prototype_Component_Navigation */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Map/NaviTest.txt"), 4))))
		return E_FAIL;

	lstrcpy(m_szLoading, TEXT("객체원형 로딩 중입니다."));

	/* For.Terrain */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain"),
		CTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Sky */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Sky"),
		CSky::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Camera_Free */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_Free"),
		CCamera_Free::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Camera_OnBone */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_OnBone"),
		CCamera_OnBone::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Camera_Pistol */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_Pistol"),
		CCamera_Pistol::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma region Object
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player"),
		CPlayer::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Pistol"),
		CPistol::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Thresher"),
		CThresher::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ThresherArm"),
		CThresher_Arm::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ThresherArm2"),
		CThresher_Arm2::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ThresherMelee"),
		CThresher_Melee::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_SpiderAnt"),
		CSpiderAnt::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Stalker"),
		CStalker::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Skag"),
		CSkag::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Goliath"),
		CGoliath::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Goliath2"),
		CGoliath2::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Goliath3"),
		CGoliath3::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Missile"),
		CMissile::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_HealthVial"),
		CHealthVial::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Psycho"),
		CPsycho::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Psycho2"),
		CPsycho2::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Psycho3"),
		CPsycho3::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Axe"),
		CAxe::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Psycho_Face"),
		CPsycho_Face::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Horn"),
		CHorn::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Hammerlock"),
		CHammerlock::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TreasureChest"),
		CTreasureChest::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_AmmoPickUp"),
		CAmmoPickUp::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion

#pragma region Static_Object
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_AridDistanceTerrain"),
		CAridDistanceTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_IceBlock01"),
		CIceBlock01::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Large_Rock1"),
		CLarge_Rock1::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_DamWall"),
		CDamWall::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Container"),
		CContainer::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TunnelDoor"),
		CTunnelDoor::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_CircularSupport"),
		CCircularSupport::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_House"),
		CHouse::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_CrateSet"),
		CCrateSet::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MetalBeam"),
		CMetalBeam::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MetalLadder"),
		CMetalLadder::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Pillar"),
		CPillar::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_WaterTower"),
		CWaterTower::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BarCounter"),
		CBarCounter::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BedGarbage"),
		CBedGarbage::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Boat"),
		CBoat::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Dropship"),
		CDropship::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_DropzoneWall"),
		CDropzoneWall::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_JakobsTank"),
		CJakobsTank::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_LynchwoodHotel"),
		CLynchwoodHotel::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Yurt"),
		CYurt::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Yurt2"),
		CYurt2::Create(m_pDevice, m_pContext))))
		return E_FAIL;


#pragma endregion

#pragma region Collider
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Thresher_Collider"),
		CThresher_Collider::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Thresher_TentacleCollider"),
		CThresher_TentacleCollider::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Thresher_ArmCollider"),
		CThresher_ArmCollider::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Thresher_Arm2Collider"),
		CThresher_Arm2Collider::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Thresher_MeleeCollider"),
		CThresher_MeleeCollider::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Thresher_ArmsTopCollider"),
		CThresher_ArmsTopCollider::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_SpiderAnt_Collider"),
		CSpiderAnt_Collider::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_SpiderAnt_LegCollider"),
		CSpiderAnt_LegCollider::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_SpiderAnt_HeadCollider"),
		CSpiderAnt_HeadCollider::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Stalker_Collider"),
		CStalker_Collider::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Stalker_HeaderCollider"),
		CStalker_HeaderCollider::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Skag_HeadCollider"),
		CSkag_HeadCollider::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Skag_Collider"),
		CSkag_Collider::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Skag_ToungueCollider"),
		CSkag_ToungueCollider::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Stalker_TailCollider"),
		CStalker_TailCollider::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Monster_Fire"),
		CMonster_Fire::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Goliath_HeadCollider"),
		CGoliath_HeadCollider::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Goliath_ArmCollider"),
		CGoliath_ArmCollider::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Goliath_BodyCollider"),
		CGoliath_BodyCollider::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Goliath_LegCollider"),
		CGoliath_LegCollider::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Psycho_BodyCollider"),
		CPsycho_BodyCollider::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Psycho_LegCollider"),
		CPsycho_LegCollider::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion

#pragma region UI
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_HpBar"),
		CHpBar::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_HpIcon"),
		CHpIcon::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_HpBorder"),
		CHpBorder::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_GunBar"),
		CGunBar::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_GunIcon"),
		CGunIcon::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Sight"),
		CSight::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Zoom"),
		CZoom::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UIFont"),
		CUIFont::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ShieldBar"),
		CShieldBar::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ShieldIcon"),
		CShieldIcon::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ShieldBorder"),
		CShieldBorder::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_GunBorder"),
		CGunBorder::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TextUI"),
		CTextUI::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MissionBox"),
		CMissionBox::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MissionFont"),
		CMissionFont::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EXPBar"),
		CEXPBar::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EXPBorder"),
		CEXPBorder::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MonsterHpBar"),
		CMonsterHpBar::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ThresherUI"),
		CThresherUI::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_InteractionFont"),
		CInteractionFont::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Key_G"),
		CKey_G::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TextFont"),
		CTextFont::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ShieldUI"),
		CShieldUI::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_AttackedUI"),
		CAttackedUI::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_DamageFont"),
		CDamageFont::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_AreaFont"),
		CAreaFont::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion

#pragma region Effect
	/* For.Prototype_GameObject_Muzzle */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Muzzle"),
		CMuzzle::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Snow_Blue"),
		CSnow_Blue::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Snowing"),
		CSnowing::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Rock"),
		CRock::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_AppearRock"),
		CAppearRock::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Bleeding*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Blood"),
		CBlood::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Blast"),
		CBlast::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Explosion"),
		CExplosion::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Smoke"),
		CSmoke::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Lightning"),
		CLightning::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Dust"),
		CDust::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EMPBlue"),
		CEMPBlue::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EMPRed"),
		CEMPRed::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Vapor"),
		CVapor::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Energy"),
		CEnergy::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion

	lstrcpy(m_szLoading, TEXT("로딩이 완료되었습니다."));

	Safe_Release(pGameInstance);

	m_isFinished = true;


	return S_OK;
}

HRESULT CLoader::Loading_ForEnding()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	lstrcpy(m_szLoading, TEXT("텍스쳐를 로딩 중입니다."));

	/* For.Prototype_Component_Texture_Terrain */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ENDING, TEXT("Prototype_Component_Texture_Terrain"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Borderland/Terrain/SnowDrift_01_Dif.dds")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Sky */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ENDING, TEXT("Prototype_Component_Texture_Sky"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Borderland/SkyBox/Sky_%d.dds"), 4))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Mask */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ENDING, TEXT("Prototype_Component_Texture_Mask"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Mask.dds")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Brush */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ENDING, TEXT("Prototype_Component_Texture_Brush"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Brush.png")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ENDING, TEXT("Prototype_Component_Texture_DissolveTex"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Borderland/Effect/Noise.png")))))
		return E_FAIL;

#pragma region Effect
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ENDING, TEXT("Prototype_Component_Texture_Snowing"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Borderland/Effect/Snowing.png")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ENDING, TEXT("Prototype_Component_Texture_Muzzle"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Borderland/Effect/Muzzle%d.png"), 4))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ENDING, TEXT("Prototype_Component_Texture_Catridge"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Borderland/Effect/Catridge.png")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ENDING, TEXT("Prototype_Component_Texture_Fog"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Borderland/Effect/Fog.png"), 1))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ENDING, TEXT("Prototype_Component_Texture_Blast"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Borderland/Effect/Blast%d.png"), 9))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ENDING, TEXT("Prototype_Component_Texture_Explosion"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Borderland/Effect/Explosion%d.png"), 16))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ENDING, TEXT("Prototype_Component_Texture_Smoke"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Borderland/Effect/Smoke%d.png"), 8))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ENDING, TEXT("Prototype_Component_Texture_Lightning"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Borderland/Effect/Lightning%d.png"), 4))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ENDING, TEXT("Prototype_Component_Texture_ExplosiveBurst"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Borderland/Effect/ExplosiveBurst%d.png"), 4))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ENDING, TEXT("Prototype_Component_Texture_EMPBlue"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Borderland/Effect/EMP%d.png"), 4))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ENDING, TEXT("Prototype_Component_Texture_EMPRed"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Borderland/Effect/EMPRed%d.png"), 4))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ENDING, TEXT("Prototype_Component_Texture_Energy"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Borderland/Effect/Energy%d.png"), 4))))
		return E_FAIL;


#pragma endregion

#pragma region UI Texture
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ENDING, TEXT("Prototype_Component_Texture_HpBar"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Borderland/UI/HP_Bar.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ENDING, TEXT("Prototype_Component_Texture_HpIcon"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Borderland/UI/HP_Icon.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ENDING, TEXT("Prototype_Component_Texture_HpBorder"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Borderland/UI/HP_Border.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ENDING, TEXT("Prototype_Component_Texture_GunBar"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Borderland/UI/Gun_Bar.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ENDING, TEXT("Prototype_Component_Texture_GunIcon"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Borderland/UI/SMG.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ENDING, TEXT("Prototype_Component_Texture_Sight"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Borderland/UI/Tex_Hyperion_Sight_SMG.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ENDING, TEXT("Prototype_Component_Texture_Zoom"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Borderland/UI/Zoom.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ENDING, TEXT("Prototype_Component_Texture_ShieldBar"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Borderland/UI/Shield_Bar.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ENDING, TEXT("Prototype_Component_Texture_ShieldBorder"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Borderland/UI/Shield_Border.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ENDING, TEXT("Prototype_Component_Texture_ShieldIcon"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Borderland/UI/Shield_Icon.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ENDING, TEXT("Prototype_Component_Texture_GunBorder"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Borderland/UI/Gun_Border.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ENDING, TEXT("Prototype_Component_Texture_Infinity"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Borderland/UI/Infinity.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ENDING, TEXT("Prototype_Component_Texture_TextUI"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Borderland/UI/TextUI.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ENDING, TEXT("Prototype_Component_Texture_MissionBox"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Borderland/UI/MissionBox.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ENDING, TEXT("Prototype_Component_Texture_MissionClearBox"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Borderland/UI/MissionClearBox.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ENDING, TEXT("Prototype_Component_Texture_EXPBorder"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Borderland/UI/EXPBorder.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ENDING, TEXT("Prototype_Component_Texture_EXPBar"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Borderland/UI/EXPBar.dds")))))
		return E_FAIL;


	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ENDING, TEXT("Prototype_Component_Texture_Key_G"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Borderland/UI/Key_G.png")))))
		return E_FAIL;


	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ENDING, TEXT("Prototype_Component_Texture_MonsterHpBar"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Borderland/UI/MonsterHpBar.dds")))))
		return E_FAIL;

#pragma endregion

	lstrcpy(m_szLoading, TEXT("모델을 로딩 중입니다."));

	/* For.Prototype_Component_VIBuffer_Terrain */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ENDING, TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Borderland/Terrain/Flatland.bmp")))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Cube*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ENDING, TEXT("Prototype_Component_VIBuffer_Cube"),
		CVIBuffer_Cube::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Rect_Instance*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ENDING, TEXT("Prototype_Component_VIBuffer_Rect_Instance"),
		CVIBuffer_Rect_Instance::Create(m_pDevice, m_pContext, 300))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Point_Instance*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ENDING, TEXT("Prototype_Component_VIBuffer_Point_Instance"),
		CVIBuffer_Point_Instance::Create(m_pDevice, m_pContext, 100))))
		return E_FAIL;

	_matrix		PivotMatrix = XMMatrixIdentity();

	/* For.Prototype_Component_Model_Player */
	PivotMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ENDING, TEXT("Prototype_Component_Model_TownPlayer"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Borderland/Player/Player_Pistol.fbx", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Pistol */
	PivotMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f) * XMMatrixRotationY(XMConvertToRadians(82.f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ENDING, TEXT("Prototype_Component_Model_Pistol"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Borderland/Weapon/SMG.fbx", PivotMatrix))))
		return E_FAIL;

	PivotMatrix = XMMatrixScaling(0.007f, 0.008f, 0.008f) * XMMatrixRotationY(XMConvertToRadians(255.0f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ENDING, TEXT("Prototype_Component_Model_Hammerlock"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Borderland/NPC/Hammerlock.fbx", PivotMatrix))))
		return E_FAIL;

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationZ(XMConvertToRadians(270.0f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ENDING, TEXT("Prototype_Component_Model_ClapTrap"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Borderland/NPC/ClapTrap.fbx", PivotMatrix))))
		return E_FAIL;

	PivotMatrix = XMMatrixScaling(0.004f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ENDING, TEXT("Prototype_Component_Model_TreasureChest"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Borderland/Item/TreasureChest.fbx", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_HealthVial */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ENDING, TEXT("Prototype_Component_Model_HealthVial"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Borderland/Item/HealthVial.fbx"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ENDING, TEXT("Prototype_Component_Model_AmmoPickUp"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Borderland/Item/AmmoPickUp.fbx"))))
		return E_FAIL;

	PivotMatrix = XMMatrixScaling(0.007f, 0.007f, 0.007f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ENDING, TEXT("Prototype_Component_Model_Roland"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Borderland/NPC/Roland.fbx", PivotMatrix))))
		return E_FAIL;

	PivotMatrix = XMMatrixScaling(0.007f, 0.007f, 0.007f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ENDING, TEXT("Prototype_Component_Model_Lilth"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Borderland/NPC/Lilth.fbx", PivotMatrix))))
		return E_FAIL;

	PivotMatrix = XMMatrixScaling(0.007f, 0.007f, 0.007f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ENDING, TEXT("Prototype_Component_Model_Mordecai"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Borderland/NPC/Mordecai.fbx", PivotMatrix))))
		return E_FAIL;

#pragma region StaicObject
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ENDING, TEXT("Prototype_Component_Model_DamWall"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Borderland/Thresher_Static/obj_DamWall.fbx", PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ENDING, TEXT("Prototype_Component_Model_MetalBeam"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Borderland/Thresher_Static/obj_MetalBeam.fbx", PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ENDING, TEXT("Prototype_Component_Model_MetalLadder"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Borderland/Thresher_Static/obj_MetalLadder.fbx", PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ENDING, TEXT("Prototype_Component_Model_WaterTower"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Borderland/Thresher_Static/obj_WaterTower.fbx", PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ENDING, TEXT("Prototype_Component_Model_Dropship"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Borderland/Thresher_Static/obj_Dropship.fbx", PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ENDING, TEXT("Prototype_Component_Model_Yurt"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Borderland/Thresher_Static/obj_Yurt.fbx", PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ENDING, TEXT("Prototype_Component_Model_Yurt2"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Borderland/Thresher_Static/obj_Yurt2.fbx", PivotMatrix))))
		return E_FAIL;

#pragma endregion

	lstrcpy(m_szLoading, TEXT("셰이더를 로딩 중입니다."));

	/* For.Prototype_Component_Shader_VtxNorTex */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ENDING, TEXT("Prototype_Component_Shader_VtxNorTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxMesh */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ENDING, TEXT("Prototype_Component_Shader_VtxMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxMesh.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxAnimMesh */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ENDING, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnimMesh.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxCubeTex */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ENDING, TEXT("Prototype_Component_Shader_VtxCubeTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxCubeTex.hlsl"), VTXCUBETEX::Elements, VTXCUBETEX::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxRectInstance */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ENDING, TEXT("Prototype_Component_Shader_VtxRectInstance"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxRectInstance.hlsl"), VTXRECTINSTANCE::Elements, VTXRECTINSTANCE::iNumElements))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ENDING, TEXT("Prototype_Component_Shader_VtxPointInstance"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPointInstance.hlsl"), VTXPOINTINSTANCE::Elements, VTXPOINTINSTANCE::iNumElements))))
		return E_FAIL;

	lstrcpy(m_szLoading, TEXT("네비게이션 로딩 중입니다."));

	/* For.Prototype_Component_Navigation */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ENDING, TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Map/NaviEnding.txt"), 5))))
		return E_FAIL;

	lstrcpy(m_szLoading, TEXT("객체원형 로딩 중입니다."));

	/* For.Terrain */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EndingTerrain"),
		CTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Sky */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EndingSky"),
		CSky::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Camera_Free */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EndingCamera_Free"),
		CCamera_Free::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Camera_OnBone */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EndingCamera_OnBone"),
		CCamera_OnBone::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma region Object

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EndingPlayer"),
		CTownPlayer::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EndingPistol"),
		CPistol::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EndingHammerlock"),
		CHammerlock::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EndingClapTrap"),
		CClapTrap::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EndingHealthVial"),
		CHealthVial::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EndingTreasureChest"),
		CTreasureChest::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EndingAmmoPickUp"),
		CAmmoPickUp::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EndingRoland"),
		CRoland::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EndingLilth"),
		CLilth::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EndingMordecai"),
		CMordecai::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma endregion

#pragma region Static_Object

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EndingDamWall"),
		CDamWall::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EndingMetalBeam"),
		CMetalBeam::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EndingWaterTower"),
		CWaterTower::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EndingDropship"),
		CDropship::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EndingYurt"),
		CYurt::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EndingYurt2"),
		CYurt2::Create(m_pDevice, m_pContext))))
		return E_FAIL;


#pragma endregion

#pragma region UI
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EndingHpBar"),
		CHpBar::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EndingHpIcon"),
		CHpIcon::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EndingHpBorder"),
		CHpBorder::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EndingGunBar"),
		CGunBar::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EndingGunIcon"),
		CGunIcon::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EndingSight"),
		CSight::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EndingZoom"),
		CZoom::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EndingUIFont"),
		CUIFont::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EndingShieldBar"),
		CShieldBar::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EndingShieldBorder"),
		CShieldBorder::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EndingShieldIcon"),
		CShieldIcon::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EndingGunBorder"),
		CGunBorder::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EndingMissionBox"),
		CMissionBox::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EndingMissionFont"),
		CMissionFont::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EndingEXPBar"),
		CEXPBar::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EndingEXPBorder"),
		CEXPBorder::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EndingMonsterHpBar"),
		CMonsterHpBar::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EndingInteractionFont"),
		CInteractionFont::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EndingKey_G"),
		CKey_G::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EndingTextFont"),
		CTextFont::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EndingAreaFont"),
		CAreaFont::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion

#pragma region Effect
		/* For.Prototype_GameObject_Muzzle */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EndingMuzzle"),
		CMuzzle::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EndingFog"),
		CFog::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EndingBlast"),
		CBlast::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EndingExplosion"),
		CExplosion::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EndingSmoke"),
		CSmoke::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EndingLightning"),
		CLightning::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EndingExplosiveBurst"),
		CDust::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EndingEMPBlue"),
		CEMPBlue::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EndingEnergy"),
		CEnergy::Create(m_pDevice, m_pContext))))
		return E_FAIL;


#pragma endregion

	lstrcpy(m_szLoading, TEXT("로딩이 완료되었습니다."));

	Safe_Release(pGameInstance);

	m_isFinished = true;

	return S_OK;
}

CLoader * CLoader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevel)
{
	CLoader*	pInstance = new CLoader(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevel)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Created : CLoader");
	}

	return pInstance;
}

void CLoader::Free()
{
	WaitForSingleObject(m_hThread, INFINITE);

	CloseHandle(m_hThread);

	DeleteObject(m_hThread);

	DeleteCriticalSection(&m_CriticalSection);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
