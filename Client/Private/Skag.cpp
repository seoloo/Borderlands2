#include "stdafx.h"
#include "..\Public\Skag.h"

#include "GameInstance.h"
#include "SoundMgr.h"

#include "Skag_HeadCollider.h"
#include "Skag_Collider.h"
#include "Skag_ToungueCollider.h"

#include "Player.h"
#include "Stalker.h"

#include "Horn.h"

CSkag::CSkag(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CLandObject(pDevice, pContext)
{
}

CSkag::CSkag(const CSkag & rhs)
	: CLandObject(rhs)
{
}

HRESULT CSkag::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSkag::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(Add_Parts()))
		return E_FAIL;

	MONSTERDESC Desc = *(MONSTERDESC*)pArg;

	m_state = STATE_APPEAR;
	//m_state = STATE_DEAD;

	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(90.f, 0.f, 50.f, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, Desc.CreatePos);

	m_iMaxHp = 3000;
	m_iHp = m_iMaxHp;

	return S_OK;
}

void CSkag::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	m_iHp = Get_Hp();

	m_pPlayerTransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_Component(g_CurLevel, TEXT("Layer_Player"), TEXT("Com_Transform")));
	m_pObject = pGameInstance->Get_GameObject(g_CurLevel, TEXT("Layer_Skag"));
	m_pPlayer = pGameInstance->Get_GameObject(g_CurLevel, TEXT("Layer_Player"));

	XMStoreFloat4(&m_vSkagPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	if (m_state == STATE_IDLE)
		Idle_Mode(fTimeDelta);
	else if (m_state == STATE_APPEAR)
		Appear_Mode(fTimeDelta);
	else if (m_state == STATE_SHOUT)
		Shout_Mode(fTimeDelta);
	else if (m_state == STATE_WALK)
		Walk_Mode(fTimeDelta);
	else if (m_state == STATE_ATTACK)
		Attack_Mode(fTimeDelta);
	else if (m_state == STATE_BITE)
		Bite_Mode(fTimeDelta);
	else if (m_state == STATE_CLAW)
		Claw_Mode(fTimeDelta);
	else if (m_state == STATE_SHOOT)
		Shoot_Mode(fTimeDelta);
	else if (m_state == STATE_SUMMON)
		Summon_Mode(fTimeDelta);
	else if (m_state == STATE_ATTACKED)
		Attacked_Mode(fTimeDelta);
	else if (m_state == STATE_DEAD)
		Dead_Mode(fTimeDelta);

	if (Get_DeathAnim())
	{
		g_bMonsterCollidedMouse = false;
		g_strMonsterName == TEXT(" ");

		if (m_iCounter == 0)
		{
			CSoundMgr::Get_Instance()->PlaySoundW(L"Monster_12.mp3", 1.f);
			m_iCounter++;
		}
		m_fDissolveTimer += fTimeDelta;
		if (m_fDissolveTimer >= 0.1f)
		{
			m_fDissolveValue += 0.05f;
			m_fDissolveTimer = 0.f;
		}
		if (m_fDissolveValue >= 1.3f)
		{
			g_bSkagDead = true;
			g_bMonsterCollidedMouse = false;
			g_strMonsterName = TEXT(" ");
			Set_Dead();
		}
	}

	m_vPlayerPos.x = m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION).m128_f32[0];
	m_vPlayerPos.z = m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION).m128_f32[2];

	m_vPos.x = m_pTransformCom->Get_State(CTransform::STATE_POSITION).m128_f32[0];
	m_vPos.z = m_pTransformCom->Get_State(CTransform::STATE_POSITION).m128_f32[2];

	CTransform*				pTerrainTransform = (CTransform*)pGameInstance->Get_Component(g_CurLevel, TEXT("Layer_BackGround"), TEXT("Com_Transform"));
	CVIBuffer_Terrain*		pTerrainVIBuffer = (CVIBuffer_Terrain*)pGameInstance->Get_Component(g_CurLevel, TEXT("Layer_BackGround"), TEXT("Com_VIBuffer"));

	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		__super::SetUp_OnTerrain(m_pTransformCom->Get_State(CTransform::STATE_POSITION), pTerrainVIBuffer, pTerrainTransform));

	Safe_Release(pGameInstance);

	for (auto& Pair : m_Parts)
		Pair.second->Tick(fTimeDelta);

	

	for (size_t i = 0; i < COLLIDER_END; i++)
	{
		if (nullptr != m_pColliderCom[i])
			m_pColliderCom[i]->Tick(m_pTransformCom->Get_WorldMatrix_Float4x4());
	}
}

void CSkag::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	//Collision_Object();

	for (auto& Pair : m_Parts)
		Pair.second->Late_Tick(fTimeDelta);

	if (/* 화면에 그려진다면 */1)
	{
		for (auto& Pair : m_Parts)
		{
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, Pair.second);
		}

		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
	}
}

HRESULT CSkag::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;	

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_ShaderResourceView(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
			return E_FAIL;

		m_pShaderCom->Begin(1);

		m_pModelCom->Render(i);
	}

#ifdef _DEBUG

	for (size_t i = 0; i < COLLIDER_END; i++)
	{
		if (nullptr != m_pColliderCom[i])
			m_pColliderCom[i]->Render();
	}

	m_pNavigationCom->Render();

#endif
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	if (!g_bSkagScene)
	{
		if (m_pColliderCom[COLLIDER_SPHERE]->Collision_Mouse())
		{
			g_bMonsterCollidedMouse = true;
			g_strMonsterName = TEXT("Skag");
			g_iMonsterHp = m_iHp;

		}
		else
		{
			g_bMonsterCollidedMouse = false;
			g_strMonsterName = TEXT(" ");
		}
	}
	
	return S_OK;
}

void CSkag::Idle_Mode(_float fTimeDelta)
{
	m_pModelCom->Play_Animation(fTimeDelta);

	m_pModelCom->SetUp_Animation(35);

	if (20.f >= abs(m_vPlayerPos.x - m_vPos.x) && 20.f >= abs(m_vPlayerPos.z - m_vPos.z))
	{
		m_state = STATE_APPEAR;
	}
}

void CSkag::Appear_Mode(_float fTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	g_bSkagScene = true;

	m_pModelCom->Play_Animation(fTimeDelta);

	// 81, 82,83, 84

	m_pModelCom->SetUp_Animation(83);

	m_pTransformCom->LookAt(m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION));

	m_fAppearTimer += fTimeDelta;
	if (m_fAppearTimer >= 0.7f)
	{
		CSoundMgr::Get_Instance()->PlaySoundW(L"c2320_down2.mp3", 1.f);

		_ulong		dwByte = 0;
		HANDLE		hFile = CreateFile(TEXT("../../Client/Bin/Resources/Effect/DigRock.txt"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
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

			//_vector TempPos = XMVectorSet(RandomX(RandomNumber), 0.f, RandomZ(RandomNumber), 1.f);
			_vector TempPos = XMVectorSet(m_vPos.x, 0.5f, m_vPos.z, 1.f);

			XMStoreFloat4(&Desc.vPivotPos, TempPos);

			Desc.bDig = false;
			Desc.bFog = true;
			Desc.bGray = true;
			Desc.fMinSpeed = 35.f;
			Desc.fMaxSpeed = 60.f;

			if (FAILED(pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Layer_Effect_Explosion"),
				TEXT("Prototype_GameObject_Dust"), &Desc)))
				return;
		}
		CloseHandle(hFile);
		m_fAppearTimer = -1.f;
	}

	if (m_pModelCom->Get_Animation()[83]->Get_AnimEnd())
	{
		m_pModelCom->Get_Animation()[83]->Set_AnimEnd(false);
		m_state = STATE_SHOUT;

		m_fAppearTimer = 0.f;
	}

	Safe_Release(pGameInstance);
}

void CSkag::Shout_Mode(_float fTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	m_pModelCom->Play_Animation(fTimeDelta * 1.1f);

	// 81, 82,83, 84

	m_pModelCom->SetUp_Animation(81);
	m_pModelCom->Set_AnimEnd(false);

	m_pTransformCom->LookAt(m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION));

	if (m_iCounter ==0)
	{
		CSoundMgr::Get_Instance()->PlaySoundW(L"c4100_bomb.mp3", 1.f);

		m_iCounter++;
	}

	if (m_pModelCom->Get_Animation()[81]->Get_AnimEnd())
	{
		m_pModelCom->Get_Animation()[81]->Set_AnimEnd(false);
		m_iCounter = 0;
		g_bSkagScene = false;
		m_fSoundTimer = 0.f;
		m_state = STATE_WALK;
	}

	Safe_Release(pGameInstance);
}

void CSkag::Walk_Mode(_float fTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	m_pModelCom->Play_Animation(fTimeDelta * 2);

	m_fSoundTimer += fTimeDelta;
	if (m_fSoundTimer > 0.7f)
	{
		CSoundMgr::Get_Instance()->PlaySoundW(L"Baggi_Land.wav", 1.f);
		m_fSoundTimer = 0.f;
	}
	
	//6, 77
	m_pModelCom->SetUp_Animation(77);

	m_Look = m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	m_pTransformCom->LookAt(m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION));
	m_pTransformCom->Go_Look(m_Look, fTimeDelta, m_pNavigationCom);


	if (m_iHp <= 0)
	{
		m_fSoundTimer = 0.f;

		m_state = STATE_DEAD;
	}

	if (m_pColliderCom[COLLIDER_SPHERE]->Collision((CCollider*)pGameInstance->Get_Component(g_CurLevel, TEXT("Layer_Player"), TEXT("Com_OBB")))) {

		random_device rd;
		mt19937 gen(rd());
		uniform_int_distribution<int> Attack(1, 3);

		m_iRandomAttack = Attack(gen);

		switch (m_iRandomAttack)
		{
		case 1:
			m_fSoundTimer = 0.f;
			m_state = STATE_ATTACK;
			break;
		case 2:
			m_fSoundTimer = 0.f;
			m_state = STATE_BITE;
			break;
		default:
			m_state = STATE_CLAW;
			break;
		}
	}
	m_fTimer += fTimeDelta;

	if (m_fTimer >= 13.f)
	{
		m_fSoundTimer = 0.f;

		m_state = STATE_SUMMON;
		//m_bShootModeDone = false;
		m_fTimer = 0.f;
	}

	_float fPosX = abs(m_vPlayerPos.x - m_vPos.x);
	_float fPosZ = abs(m_vPlayerPos.z - m_vPos.z);

	if (30.f <= fPosX || 45.f <= fPosZ)
	{
		m_fSoundTimer = 0.f;

		m_state = STATE_SHOOT;
		//m_bShootModeDone = true;
	}

	if (Get_Attacked())
	{
		_int iAttack = 0;

		if (m_iCount == 0)
		{
			random_device rd;
			mt19937 gen(rd());
			uniform_int_distribution<int> Attack(1, 100);
			iAttack = Attack(gen);
			m_iCount++;
		}

		if (iAttack > 90)
		{
			m_state = STATE_ATTACKED;
			m_iCount = 0;
			m_bAttacked = false;
		}
		else
		{
			m_bAttacked = false;
			m_iCount = 0;
		}

	}

	Safe_Release(pGameInstance);
}

void CSkag::Attack_Mode(_float fTimeDelta)
{
	m_pModelCom->Play_Animation(fTimeDelta);

	if (m_iNum==0)
	{
		CSoundMgr::Get_Instance()->PlaySoundW(L"Monster_12.mp3", 1.f);
		m_iNum++;
	}

	// 3,4,10,11,21,22

	m_pModelCom->SetUp_Animation(21);

	m_pModelCom->Set_AnimEnd(false);
	if (m_pModelCom->Get_Animation()[21]->Get_AnimEnd())
	{
		m_pModelCom->Get_Animation()[21]->Set_AnimEnd(false);
		m_fShootTimer = 0.f;
		m_iNum = 0;
		m_state = STATE_WALK;
	}

}

void CSkag::Bite_Mode(_float fTimeDelta)
{
	m_pModelCom->Play_Animation(fTimeDelta);

	if (m_iNum==0)
	{
		CSoundMgr::Get_Instance()->PlaySoundW(L"Monster_12.mp3", 1.f);
		m_iNum++;
	}
	// 4,10,11,21,22

	m_pModelCom->SetUp_Animation(4);

	m_pModelCom->Set_AnimEnd(false);
	if (m_pModelCom->Get_Animation()[4]->Get_AnimEnd())
	{
		m_pModelCom->Get_Animation()[4]->Set_AnimEnd(false);
		m_fShootTimer = 0.f;
		m_iNum = 0;

		m_state = STATE_WALK;
	}
}

void CSkag::Claw_Mode(_float fTimeDelta)
{
	m_pModelCom->Play_Animation(fTimeDelta);

	if (m_iNum == 0)
	{
		CSoundMgr::Get_Instance()->PlaySoundW(L"Monster_12.mp3", 1.f);
		m_iNum++;
	}
	// 3,4,10,11,21,22


	m_pModelCom->SetUp_Animation(10);

	m_pModelCom->Set_AnimEnd(false);
	if (m_pModelCom->Get_Animation()[10]->Get_AnimEnd())
	{
		m_pModelCom->Get_Animation()[10]->Set_AnimEnd(false);
		m_fShootTimer = 0.f;
		m_iNum = 0;

		m_state = STATE_WALK;
	}
}

void CSkag::Shoot_Mode(_float fTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	m_pTransformCom->LookAt(m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION));

	m_pModelCom->Play_Animation(fTimeDelta * 2.f);

	if (m_iCounter == 0)
	{
		CSoundMgr::Get_Instance()->PlaySoundW(L"c4100_bomb.mp3", 1.f);
		m_iCounter++;
	}

	// 3,4,10,11,21,22

	m_pModelCom->SetUp_Animation(12);

	m_pModelCom->Set_AnimEnd(false);

	m_fShootTimer += fTimeDelta;

	if (m_fShootTimer > 0.3f)
	{
		_ulong		dwByte = 0;
		HANDLE		hFile = CreateFile(TEXT("../../Client/Bin/Resources/Effect/BleedingSnow.txt"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
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

			if (FAILED(pGameInstance->Add_GameObject(g_CurLevel, TEXT("Monster_Fire"),
				TEXT("Prototype_GameObject_Energy"), &Desc)))
				return;
		}
		CloseHandle(hFile);
		m_fShootTimer = 0.f;
	}

	if (m_pModelCom->Get_Animation()[12]->Get_AnimEnd())
	{
		m_pModelCom->Get_Animation()[12]->Set_AnimEnd(false);
		m_fSoundTimer = 0.f;

		m_state = STATE_WALK;
	}

	Safe_Release(pGameInstance);
}

void CSkag::Attacked_Mode(_float fTimeDelta)
{
	//5
	m_pModelCom->Play_Animation(fTimeDelta);

	m_pModelCom->SetUp_Animation(5);

	if (m_iCounter == 0)
	{
		CSoundMgr::Get_Instance()->PlaySoundW(L"Monster_6.mp3", 1.f);
		m_iCounter++;
	}

	m_pModelCom->Set_AnimEnd(false);
	if (m_pModelCom->Get_Animation()[5]->Get_AnimEnd())
	{
		m_pModelCom->Get_Animation()[5]->Set_AnimEnd(false);
		Set_Attacked(false);
		m_iCounter = 0;
		m_state = STATE_WALK;
	}
}

void CSkag::Summon_Mode(_float fTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);
	//12,13
	m_pModelCom->Play_Animation(fTimeDelta * 1.3f);

	m_pModelCom->SetUp_Animation(13);

	m_pModelCom->Set_AnimEnd(false);

	if (m_iNum == 0)
	{
		CSoundMgr::Get_Instance()->PlaySoundW(L"c4100_bomb.mp3", 1.f);
		m_iNum++;
	}

	g_bCamShake = true;

	if (m_pModelCom->Get_Animation()[13]->Get_AnimEnd())
	{
		g_bCamShake = false;

		m_pModelCom->Get_Animation()[13]->Set_AnimEnd(false);
		Set_Attacked(false);

		CStalker::MONSTERDESC Desc;
		//Desc.CreatePos = XMVectorSet(m_vSkagPos.x, m_vSkagPos.y, m_vSkagPos.z - 10.f, 1.f);
		Desc.CreatePos =  XMVectorSet(90.f, 0.f, 60.f, 1.f);
		if (FAILED(pGameInstance->Add_GameObject(g_CurLevel, TEXT("Layer_Stalker"),
			TEXT("Prototype_GameObject_Stalker"), &Desc)))
			return;

		m_state = STATE_WALK;
		m_iCounter = 0;
	}
	Safe_Release(pGameInstance);
}

void CSkag::Dead_Mode(_float fTimeDelta)
{
	g_bMonsterCollidedMouse = false;
	g_strMonsterName = TEXT(" ");

	m_pModelCom->Play_Animation(fTimeDelta * 0.5f);

	m_pModelCom->SetUp_Animation(62, true);

	Set_DeathAnim(true);
	if (m_pModelCom->Get_AnimEnd())
	{
		m_pModelCom->Set_AnimEnd(false);

		if (m_pModelCom->Get_Animation()[62]->Get_AnimEnd())
		{
		}
	}
}

HRESULT CSkag::Add_Components()
{
	/* Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;
	/* Com_Texture */
	if (FAILED(__super::Add_Component(g_CurLevel, TEXT("Prototype_Component_Texture_DissolveTex"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;
	/* Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof TransformDesc);

	TransformDesc.fSpeedPerSec = 11.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(__super::Add_Component(g_CurLevel, TEXT("Prototype_Component_Shader_VtxAnimMesh"), 
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom))) 
		return E_FAIL;

	/* Com_Model */
	if (FAILED(__super::Add_Component(g_CurLevel, TEXT("Prototype_Component_Model_Skag"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;	

	/* Com_AABB */
	CCollider::COLLIDERDESC		ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof ColliderDesc);

	ColliderDesc.vSize = _float3(0.7f, 1.3f, 0.7f);
	ColliderDesc.vPosition = _float3(0.f, ColliderDesc.vSize.y * 0.5f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_AABB"), (CComponent**)&m_pColliderCom[COLLIDER_AABB], &ColliderDesc)))
		return E_FAIL;

	/* Com_OBB */
	ZeroMemory(&ColliderDesc, sizeof ColliderDesc);

	ColliderDesc.vSize = _float3(1.f, 1.f, 1.f);
	ColliderDesc.vPosition = _float3(0.f, ColliderDesc.vSize.y * 0.5f, 0.f);
	ColliderDesc.vRotation = _float3(0.0f, XMConvertToRadians(45.0f), 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_OBB"), (CComponent**)&m_pColliderCom[COLLIDER_OBB], &ColliderDesc)))
		return E_FAIL;

	/* Com_SPHERE */
	ZeroMemory(&ColliderDesc, sizeof ColliderDesc);

	ColliderDesc.fRadius = 10.f;
	ColliderDesc.vPosition = _float3(0.f, ColliderDesc.fRadius - 10.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_SPHERE"), (CComponent**)&m_pColliderCom[COLLIDER_SPHERE], &ColliderDesc)))
		return E_FAIL;

	/* Com_Navigation */
	CNavigation::NAVIDESC		NaviDesc;

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CTransform* pTerrainTransform = (CTransform*)pGameInstance->Get_Component(g_CurLevel, TEXT("Layer_BackGround"), TEXT("Com_Transform"));

	NaviDesc.pWorldMatrix = pTerrainTransform->Get_WorldMatrixPtr();


	if (g_CurLevel == 3)
		NaviDesc.iCurrentIndex = 2;
	else if (g_CurLevel == 4)
		NaviDesc.iCurrentIndex = 73;

	Safe_Release(pGameInstance);
	if (FAILED(__super::Add_Component(g_CurLevel, TEXT("Prototype_Component_Navigation"),
		TEXT("Com_Navigation"), (CComponent**)&m_pNavigationCom, &NaviDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CSkag::Add_Parts()
{
	CGameObject* pPart = nullptr;
	CGameObject* pPart2 = nullptr;
	CGameObject* pPart3 = nullptr;
	CGameObject* pPart4 = nullptr;
	CGameObject* pPart5 = nullptr;
	CGameObject* pPart6 = nullptr;
	CGameObject* pPart7 = nullptr;
	CGameObject* pPart8 = nullptr;

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CSkag_HeadCollider::COLLIDERDESC			Desc;
	Desc.pBoneMatrix = m_pModelCom->Get_BoneCombinedMatrix("Head");
	Desc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	Desc.PivotMatrix = m_pModelCom->Get_PivotMatrix();

	CSkag_Collider::COLLIDERDESC			Desc2;
	Desc2.pBoneMatrix = m_pModelCom->Get_BoneCombinedMatrix("Tounge1");
	Desc2.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	Desc2.PivotMatrix = m_pModelCom->Get_PivotMatrix();

	CSkag_Collider::COLLIDERDESC			Desc3;
	Desc3.pBoneMatrix = m_pModelCom->Get_BoneCombinedMatrix("Tounge2");
	Desc3.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	Desc3.PivotMatrix = m_pModelCom->Get_PivotMatrix();

	CSkag_Collider::COLLIDERDESC			Desc4;
	Desc4.pBoneMatrix = m_pModelCom->Get_BoneCombinedMatrix("Tounge3");
	Desc4.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	Desc4.PivotMatrix = m_pModelCom->Get_PivotMatrix();

	CSkag_Collider::COLLIDERDESC			Desc5;
	Desc5.pBoneMatrix = m_pModelCom->Get_BoneCombinedMatrix("Tounge4");
	Desc5.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	Desc5.PivotMatrix = m_pModelCom->Get_PivotMatrix();

	CSkag_Collider::COLLIDERDESC			Desc6;
	Desc6.pBoneMatrix = m_pModelCom->Get_BoneCombinedMatrix("Jaw");
	Desc6.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	Desc6.PivotMatrix = m_pModelCom->Get_PivotMatrix();

	CSkag_Collider::COLLIDERDESC			Desc7;
	Desc7.pBoneMatrix = m_pModelCom->Get_BoneCombinedMatrix("RHand");
	Desc7.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	Desc7.PivotMatrix = m_pModelCom->Get_PivotMatrix();

	CSkag_Collider::COLLIDERDESC			Desc8;
	Desc8.pBoneMatrix = m_pModelCom->Get_BoneCombinedMatrix("LHand");
	Desc8.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	Desc8.PivotMatrix = m_pModelCom->Get_PivotMatrix();

	pPart = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Skag_HeadCollider"), &Desc);
	pPart2 = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Skag_ToungueCollider"), &Desc2);
	pPart3 = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Skag_ToungueCollider"), &Desc3);
	pPart4 = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Skag_ToungueCollider"), &Desc4);
	pPart5 = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Skag_ToungueCollider"), &Desc5);
	pPart6 = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Skag_Collider"), &Desc6);
	pPart7 = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Skag_Collider"), &Desc7);
	pPart8 = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Skag_Collider"), &Desc8);

	if (nullptr == pPart)
		return E_FAIL;
	if (nullptr == pPart2)
		return E_FAIL;
	if (nullptr == pPart3)
		return E_FAIL;
	if (nullptr == pPart4)
		return E_FAIL;
	if (nullptr == pPart5)
		return E_FAIL;
	if (nullptr == pPart6)
		return E_FAIL;
	if (nullptr == pPart7)
		return E_FAIL;
	if (nullptr == pPart8)
		return E_FAIL;

	m_Parts.emplace(TEXT("Part_1"), pPart);
	m_Parts.emplace(TEXT("Part_2"), pPart2);
	m_Parts.emplace(TEXT("Part_3"), pPart3);
	m_Parts.emplace(TEXT("Part_4"), pPart4);
	m_Parts.emplace(TEXT("Part_5"), pPart5);
	m_Parts.emplace(TEXT("Part_6"), pPart6);
	m_Parts.emplace(TEXT("Part_7"), pPart7);
	m_Parts.emplace(TEXT("Part_8"), pPart8);

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CSkag::Bind_ShaderResources()
{	
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_ShaderResourceView(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_AddDissolve", &m_fDissolveValue, sizeof(_float))))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CSkag::Collision_Object()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (m_pColliderCom[COLLIDER_SPHERE]->Collision((CCollider*)pGameInstance->Get_Component(g_CurLevel, TEXT("Layer_Stalker"), TEXT("Com_SPHERE")))) {
		m_bCollision = true;
	}
	else
		m_bCollision = false;

	Safe_Release(pGameInstance);

	return S_OK;
}

CSkag * CSkag::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CSkag*	pInstance = new CSkag(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Created : CSkag");
	}

	return pInstance;
}

CGameObject * CSkag::Clone(void * pArg)
{
	CSkag*	pInstance = new CSkag(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Cloned : CSkag");
	}

	return pInstance;
}

void CSkag::Free()
{
	__super::Free();

	m_pPlayerTransformCom = { nullptr };
	m_pPlayer = { nullptr };

	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pPlayerTransformCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pTextureCom);

	for (auto& Pair : m_Parts)
		Safe_Release(Pair.second);

	m_Parts.clear();

	for (size_t i = 0; i < COLLIDER_END; i++)
		Safe_Release(m_pColliderCom[i]);
}
