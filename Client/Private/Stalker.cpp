#include "stdafx.h"
#include "..\Public\Stalker.h"

#include "GameInstance.h"
#include "SoundMgr.h"

#include "Stalker_Collider.h"
#include "Stalker_TailCollider.h"
#include "Stalker_HeaderCollider.h"
#include "Player.h"

CStalker::CStalker(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CLandObject(pDevice, pContext)
{
}

CStalker::CStalker(const CStalker & rhs)
	: CLandObject(rhs)
{
}

HRESULT CStalker::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CStalker::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(Add_Parts()))
		return E_FAIL;

	MONSTERDESC Desc = *(MONSTERDESC*)pArg;

	m_state = STATE_APPEAR;

	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(90.f, 0.f, 50.f, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, Desc.CreatePos);

	m_iMaxHp = 300;
	m_iHp = m_iMaxHp;

	return S_OK;
}

void CStalker::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	m_iHp = Get_Hp();

	m_pPlayerTransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_Component(g_CurLevel, TEXT("Layer_Player"), TEXT("Com_Transform")));

	if (m_state == STATE_IDLE)
		Idle_Mode(fTimeDelta);
	else if (m_state == STATE_APPEAR)
		Appear_Mode(fTimeDelta);
	else if (m_state == STATE_WALK)
		Walk_Mode(fTimeDelta);
	else if (m_state == STATE_ATTACK)
		Attack_Mode(fTimeDelta);
	else if (m_state == STATE_CLAWATTACK)
		ClawAttack_Mode(fTimeDelta);
	else if (m_state == STATE_TAILATTACK)
		TailAttack_Mode(fTimeDelta);
	else if (m_state == STATE_ATTACKED)
		Attacked_Mode(fTimeDelta);
	else if (m_state == STATE_DEAD)
		Dead_Mode(fTimeDelta);

	if (Get_DeathAnim())
	{
		g_bPsychoCollidedMouse = false;

		if (m_iCounter == 0)
		{
			CSoundMgr::Get_Instance()->PlaySoundW(L"kerberos_attack_icebreath_04_voice.ogg", 1.f);
			m_iCounter++;
		}
		m_fDissolveTimer += fTimeDelta;
		if (m_fDissolveTimer > 0.1f)
		{
			m_fDissolveValue += 0.06f;
			m_fDissolveTimer = 0.f;
		}
		if (m_fDissolveValue >= 1.3f)
		{
			g_PlayerEXP += 50;
			g_bPsychoCollidedMouse = false;
			m_iCounter = 0.f;
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

void CStalker::Late_Tick(_float fTimeDelta)
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

HRESULT CStalker::Render()
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
	if (m_pColliderCom[COLLIDER_SPHERE]->Collision_Mouse())
	{
		g_bPsychoCollidedMouse = true;
		g_strMonsterName = TEXT("Stalker");
		g_iMonsterHp = m_iHp;
	}
	else {
		g_bPsychoCollidedMouse = false;
		g_strMonsterName = TEXT(" ");
	}

	return S_OK;
}

void CStalker::Idle_Mode(_float fTimeDelta)
{
	m_pModelCom->Play_Animation(fTimeDelta);

	m_pModelCom->SetUp_Animation(33);

	if (20.f >= abs(m_vPlayerPos.x - m_vPos.x) && 20.f >= abs(m_vPlayerPos.z - m_vPos.z))
	{

		CSoundMgr::Get_Instance()->PlaySoundW(L"Monster_12.mp3", 1.f);
		m_state = STATE_WALK;
	}
}

void CStalker::Appear_Mode(_float fTimeDelta)
{
	m_pModelCom->Play_Animation(fTimeDelta);

	m_pTransformCom->LookAt(m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION));
	m_pModelCom->SetUp_Animation(56);

	m_pModelCom->Set_AnimEnd(false);
	if (m_pModelCom->Get_Animation()[56]->Get_AnimEnd())
	{
		m_pModelCom->Get_Animation()[56]->Set_AnimEnd(false);
		CSoundMgr::Get_Instance()->PlaySoundW(L"kerberos_attack_icebreath_04_voice.ogg", 1.f);

		m_state = STATE_WALK;
	}
}

void CStalker::Walk_Mode(_float fTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	m_pModelCom->Play_Animation(fTimeDelta);
	//25

	m_pModelCom->SetUp_Animation(68);

	m_Look = m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	m_pTransformCom->LookAt(m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION));
	m_pTransformCom->Go_Look(m_Look, fTimeDelta, m_pNavigationCom);

	if (m_iHp <= 0)
		m_state = STATE_DEAD;

	if (m_pColliderCom[COLLIDER_SPHERE]->Collision((CCollider*)pGameInstance->Get_Component(g_CurLevel, TEXT("Layer_Player"), TEXT("Com_OBB")))) {
		random_device rd;
		mt19937 gen(rd());
		uniform_int_distribution<int> Attack(1, 2);

		m_iRandomAttack = Attack(gen);

		// 5, 72, 73,74
		switch (m_iRandomAttack)
		{
		case 1:
			m_state = STATE_TAILATTACK;
			break;
		case 2:
			m_state = STATE_CLAWATTACK;
			break;
		default:
			break;
		}
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

void CStalker::Attack_Mode(_float fTimeDelta)
{
	m_pModelCom->Play_Animation(fTimeDelta);

	random_device rd;
	mt19937 gen(rd());
	uniform_int_distribution<int> Attack(1,2);

	m_iRandomAttack = Attack(gen);

	m_state = STATE_CLAWATTACK;
	// 5, 72, 73,74
	/*switch (m_iRandomAttack)
	{
	case 1:
		m_state = STATE_TAILATTACK;
		break;
	case 2:
		break;
	default:
		break;
	}*/

	/*m_pModelCom->SetUp_Animation(m_iAttackAnim);

	m_pModelCom->Set_AnimEnd(false);
	if (m_pModelCom->Get_Animation()[m_iAttackAnim]->Get_AnimEnd())
	{
		m_pModelCom->Get_Animation()[m_iAttackAnim]->Set_AnimEnd(false);
		m_state = STATE_WALK;
	}*/

}

void CStalker::ClawAttack_Mode(_float fTimeDelta)
{
	// 5, 72, 73,74
	m_pModelCom->Play_Animation(fTimeDelta);

	m_pModelCom->SetUp_Animation(5);

	m_pModelCom->Set_AnimEnd(false);

	if (m_iCounter ==0)
	{
		CSoundMgr::Get_Instance()->PlaySoundW(L"WolfB_Sound_1.ogg", 1.f);

		m_iCounter++;
	}
	if (m_pModelCom->Get_Animation()[5]->Get_AnimEnd())
	{
		m_pModelCom->Get_Animation()[5]->Set_AnimEnd(false);
		m_iCounter = 0;

		m_state = STATE_WALK;
	}
}

void CStalker::TailAttack_Mode(_float fTimeDelta)
{
	// 5, 72, 73,74
	m_pModelCom->Play_Animation(fTimeDelta);

	m_pModelCom->SetUp_Animation(74);

	if (m_iCounter ==0)
	{
		CSoundMgr::Get_Instance()->PlaySoundW(L"WolfB_Sound_1.ogg", 1.f);
		m_iCounter++;
	}

	m_pModelCom->Set_AnimEnd(false);
	if (m_pModelCom->Get_Animation()[74]->Get_AnimEnd())
	{
		m_pModelCom->Get_Animation()[74]->Set_AnimEnd(false);
		m_iCounter = 0;
		m_state = STATE_WALK;
	}
}

void CStalker::Attacked_Mode(_float fTimeDelta)
{
	//22
	m_pModelCom->Play_Animation(fTimeDelta);

	m_pModelCom->SetUp_Animation(22);

	if (m_iCounter == 0)
	{
		CSoundMgr::Get_Instance()->PlaySoundW(L"WolfB_Sound_1.ogg", 1.f);
		m_iCounter++;
	}

	m_pModelCom->Set_AnimEnd(false);
	if (m_pModelCom->Get_Animation()[22]->Get_AnimEnd())
	{
		m_pModelCom->Get_Animation()[22]->Set_AnimEnd(false);
		Set_Attacked(false);
		m_iCounter = 0;
		m_state = STATE_WALK;
	}
}

void CStalker::Dead_Mode(_float fTimeDelta)
{
	m_pModelCom->Play_Animation(fTimeDelta);

	m_pModelCom->SetUp_Animation(16, true);

	Set_DeathAnim(true);
	if (m_pModelCom->Get_AnimEnd())
	{
		m_pModelCom->Set_AnimEnd(false);

		if (m_pModelCom->Get_Animation()[16]->Get_AnimEnd())
		{
		}
	}
}

HRESULT CStalker::Add_Components()
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

	TransformDesc.fSpeedPerSec = 8.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(__super::Add_Component(g_CurLevel, TEXT("Prototype_Component_Shader_VtxAnimMesh"), 
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom))) 
		return E_FAIL;

	/* Com_Model */
	if (FAILED(__super::Add_Component(g_CurLevel, TEXT("Prototype_Component_Model_Stalker"),
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

	ColliderDesc.fRadius = 5.f;
	ColliderDesc.vPosition = _float3(0.f, ColliderDesc.fRadius - 5.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_SPHERE"), (CComponent**)&m_pColliderCom[COLLIDER_SPHERE], &ColliderDesc)))
		return E_FAIL;

	/* Com_Navigation */
	CNavigation::NAVIDESC		NaviDesc;

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CTransform* pTerrainTransform = (CTransform*)pGameInstance->Get_Component(g_CurLevel, TEXT("Layer_BackGround"), TEXT("Com_Transform"));

	NaviDesc.pWorldMatrix = pTerrainTransform->Get_WorldMatrixPtr();

	if(g_CurLevel == 3)
		NaviDesc.iCurrentIndex = 2;
	else if(g_CurLevel == 4)
		NaviDesc.iCurrentIndex = 73;

	Safe_Release(pGameInstance);
	if (FAILED(__super::Add_Component(g_CurLevel, TEXT("Prototype_Component_Navigation"),
		TEXT("Com_Navigation"), (CComponent**)&m_pNavigationCom, &NaviDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CStalker::Add_Parts()
{
	CGameObject* pPart = nullptr;
	CGameObject* pPart2 = nullptr;
	CGameObject* pPart3 = nullptr;
	CGameObject* pPart4 = nullptr;

	CStalker_HeaderCollider::COLLIDERDESC			Desc;
	Desc.pBoneMatrix = m_pModelCom->Get_BoneCombinedMatrix("Head");
	Desc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	Desc.PivotMatrix = m_pModelCom->Get_PivotMatrix();

	CStalker_Collider::COLLIDERDESC			Desc2;
	Desc2.pBoneMatrix = m_pModelCom->Get_BoneCombinedMatrix("RKnee");
	Desc2.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	Desc2.PivotMatrix = m_pModelCom->Get_PivotMatrix();

	CStalker_Collider::COLLIDERDESC			Desc3;
	Desc3.pBoneMatrix = m_pModelCom->Get_BoneCombinedMatrix("LKnee");
	Desc3.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	Desc3.PivotMatrix = m_pModelCom->Get_PivotMatrix();

	CStalker_Collider::COLLIDERDESC			Desc4;
	Desc4.pBoneMatrix = m_pModelCom->Get_BoneCombinedMatrix("tailSpike");
	Desc4.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	Desc4.PivotMatrix = m_pModelCom->Get_PivotMatrix();
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	pPart = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Stalker_HeaderCollider"), &Desc);
	pPart2 = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Stalker_Collider"), &Desc2);
	pPart3 = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Stalker_Collider"), &Desc3);
	pPart4 = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Stalker_TailCollider"), &Desc4);

	if (nullptr == pPart)
		return E_FAIL;

	if (nullptr == pPart2)
		return E_FAIL;

	if (nullptr == pPart3)
		return E_FAIL;

	if (nullptr == pPart4)
		return E_FAIL;

	m_Parts.emplace(TEXT("Part_1"), pPart);
	m_Parts.emplace(TEXT("Part_2"), pPart2);
	m_Parts.emplace(TEXT("Part_3"), pPart3);
	m_Parts.emplace(TEXT("Part_4"), pPart4);

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CStalker::Bind_ShaderResources()
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

HRESULT CStalker::Collision_Object()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (m_pColliderCom[COLLIDER_SPHERE]->Collision((CCollider*)pGameInstance->Get_Component(g_CurLevel, TEXT("Layer_Stalker"), TEXT("Com_SPHERE")))) {
		m_bCollision = true;
	}

	Safe_Release(pGameInstance);

	return S_OK;
}

CStalker * CStalker::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CStalker*	pInstance = new CStalker(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Created : CStalker");
	}

	return pInstance;
}

CGameObject * CStalker::Clone(void * pArg)
{
	CStalker*	pInstance = new CStalker(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Cloned : CStalker");
	}

	return pInstance;
}

void CStalker::Free()
{
	__super::Free();

	m_pPlayerTransformCom = { nullptr };
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
