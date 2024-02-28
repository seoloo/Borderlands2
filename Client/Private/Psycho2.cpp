#include "stdafx.h"
#include "..\Public\Psycho2.h"

#include "GameInstance.h"
#include "SoundMgr.h"

#include "Psycho_Face.h"
#include "Axe.h"
#include "Psycho_BodyCollider.h"
#include "Psycho_LegCollider.h"
#include "Player.h"

CPsycho2::CPsycho2(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CLandObject(pDevice, pContext)
{
}

CPsycho2::CPsycho2(const CPsycho2 & rhs)
	: CLandObject(rhs)
{
}

HRESULT CPsycho2::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPsycho2::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;


	MONSTERDESC* Desc = (MONSTERDESC*)pArg;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(45.f, 0.f, 90.f, 1.f));
	m_iNumber = 2;

	if (FAILED(Add_Parts()))
		return E_FAIL;

	m_state = STATE_APPEAR;

	//m_iNumber = Desc->iNumber;

	m_iMaxHp = 300;
	m_iHp = m_iMaxHp;

	return S_OK;
}

void CPsycho2::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	m_iHp = Get_Hp();

	m_pPlayerTransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_Transform")));

	//m_pModelCom->SetUp_Animation(30, true); /*맞는 모션*/
	m_vPlayerPos.x = m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION).m128_f32[0];
	m_vPlayerPos.z = m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION).m128_f32[2];

	m_vPos.x = m_pTransformCom->Get_State(CTransform::STATE_POSITION).m128_f32[0];
	m_vPos.z = m_pTransformCom->Get_State(CTransform::STATE_POSITION).m128_f32[2];

	if (m_state == STATE_APPEAR)
		Appear_Mode(fTimeDelta);
	else if (m_state == STATE_IDLE)
		Idle_Mode(fTimeDelta);
	else if (m_state == STATE_WALK)
	{
		m_MoveState = MOVE_FORWARD;
		Walk_Mode(fTimeDelta);
	}
	else if (m_state == STATE_ATTACK)
		Attack_Mode(fTimeDelta);
	else if (m_state == STATE_ATTACKED)
		Attacked_Mode(fTimeDelta);
	else if (m_state == STATE_DEAD)
		Dead_Mode(fTimeDelta);

	if (Get_DeathAnim())
	{
		g_bPsychoCollidedMouse2 = false;
		if (m_iCounter == 0)
		{
			CSoundMgr::Get_Instance()->PlaySoundW(L"Monster_Scream.mp3", 1.f);
			m_iCounter++;
		}
		m_fDissolveTimer += fTimeDelta;
		if (m_fDissolveTimer >= 0.1f)
		{
			m_fDissolveValue += 0.1f;
			m_fDissolveTimer = 0.f;
		}
		if (m_fDissolveValue >= 1.5f)
		{
			g_bPsychoCollidedMouse2 = false;
			g_PlayerEXP += 50;
			Set_Dead();
		}
	}

	

	CTransform*				pTerrainTransform = (CTransform*)pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_BackGround"), TEXT("Com_Transform"));
	CVIBuffer_Terrain*		pTerrainVIBuffer = (CVIBuffer_Terrain*)pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_BackGround"), TEXT("Com_VIBuffer"));

	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		__super::SetUp_OnTerrain(m_pTransformCom->Get_State(CTransform::STATE_POSITION), pTerrainVIBuffer, pTerrainTransform));

	Safe_Release(pGameInstance);

	for (size_t i = 0; i < COLLIDER_END; i++)
	{
		if (nullptr != m_pColliderCom[i])
			m_pColliderCom[i]->Tick(m_pTransformCom->Get_WorldMatrix_Float4x4());
	}

	for (auto& Pair : m_Parts)
		Pair.second->Tick(fTimeDelta);
}

void CPsycho2::Late_Tick(_float fTimeDelta)
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

HRESULT CPsycho2::Render()
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

	/*for (size_t i = 0; i < COLLIDER_END; i++)
	{
		if (nullptr != m_pColliderCom[i])
			m_pColliderCom[i]->Render();
	}*/

	//m_pNavigationCom->Render();

#endif
	if (m_pColliderCom[COLLIDER_AABB]->Collision_Mouse())
	{
		g_bPsychoCollidedMouse2 = true;
		g_strMonsterName = TEXT("Psycho");
		g_iMonsterHp = m_iHp;
	}
	else
	{
		g_bPsychoCollidedMouse2 = false;
	}
	return S_OK;
}

void CPsycho2::Idle_Mode(_float fTimeDelta)
{
	m_pModelCom->Play_Animation(fTimeDelta);

	m_pModelCom->SetUp_Animation(22);

	if (20.f >= abs(m_vPlayerPos.x - m_vPos.x) && 20.f >= abs(m_vPlayerPos.z - m_vPos.z))
	{
		CSoundMgr::Get_Instance()->PlaySoundW(L"Monster_Appear.mp3", 1.f);

		m_state = STATE_WALK;
	}
	
}

void CPsycho2::Appear_Mode(_float fTimeDelta)
{
	m_pModelCom->Play_Animation(fTimeDelta);
	m_pModelCom->SetUp_Animation(9);

	if (m_pModelCom->Get_Animation()[9]->Get_AnimEnd())
	{
		m_pModelCom->Get_Animation()[9]->Set_AnimEnd(false);
		m_state = STATE_IDLE;
	}
}

void CPsycho2::Walk_Mode(_float fTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);
	m_pModelCom->Play_Animation(fTimeDelta);

	if (m_iHp <= 0)
		m_state = STATE_DEAD;

	//66
	m_pModelCom->SetUp_Animation(29);

	m_Look = m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	m_pTransformCom->LookAt(m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION));

	m_vPreviousPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	m_pTransformCom->Go_Look(m_Look, fTimeDelta, m_pNavigationCom);

	if (m_iHp <= 0)
		m_state = STATE_DEAD;

	if (m_pColliderCom[COLLIDER_SPHERE]->Collision((CCollider*)pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_OBB")))) {
		m_state = STATE_ATTACK;
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

void CPsycho2::Attack_Mode(_float fTimeDelta)
{
	m_pModelCom->Play_Animation(fTimeDelta);

	m_pTransformCom->Go_Look(m_Look, fTimeDelta *0.f, m_pNavigationCom);

	if (m_iCounter == 0)
	{
		CSoundMgr::Get_Instance()->PlaySoundW(L"swing-sword-charge.mp3", 1.f);
		m_iCounter++;
	}


	// 57, 58, 59, 60
	m_pModelCom->SetUp_Animation(2);

	m_pModelCom->Set_AnimEnd(false);
	if (m_pModelCom->Get_Animation()[2]->Get_AnimEnd())
	{
		m_pModelCom->Get_Animation()[2]->Set_AnimEnd(false);
		m_iCounter = 0;
		m_state = STATE_WALK;
	}
	
}

void CPsycho2::Attacked_Mode(_float fTimeDelta)
{
	m_pModelCom->Play_Animation(fTimeDelta);

	m_pModelCom->SetUp_Animation(13);

	if (m_iHp <= 0)
		m_state = STATE_DEAD;

	if (m_pModelCom->Get_Animation()[13]->Get_AnimEnd())
	{
		m_pModelCom->Get_Animation()[13]->Set_AnimEnd(false);
		m_iCount = 0;
		m_state = STATE_WALK;
	}
}

void CPsycho2::Dead_Mode(_float fTimeDelta)
{
	m_pModelCom->Play_Animation(fTimeDelta);

	m_pModelCom->SetUp_Animation(4, true);

	if (m_pModelCom->Get_AnimEnd())
	{
		Set_DeathAnim(true);
		m_pModelCom->Set_AnimEnd(false);

		if (m_pModelCom->Get_Animation()[4]->Get_AnimEnd())
		{
		}
	}
}

HRESULT CPsycho2::Add_Components()
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

	/* Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimMesh"), 
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom))) 
		return E_FAIL;

	/* Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Psycho"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;	

	/* Com_Navigation */
	CNavigation::NAVIDESC		NaviDesc;

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CTransform* pTerrainTransform = (CTransform*)pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_BackGround"), TEXT("Com_Transform"));

	NaviDesc.pWorldMatrix = pTerrainTransform->Get_WorldMatrixPtr();

	Safe_Release(pGameInstance);

	NaviDesc.iCurrentIndex = 29;
	//NaviDesc.iCurrentIndex = 4;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"),
		TEXT("Com_Navigation"), (CComponent**)&m_pNavigationCom, &NaviDesc)))
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

	ColliderDesc.vSize = _float3(1.f, 1.6f, 1.f);
	ColliderDesc.vPosition = _float3(0.f, ColliderDesc.vSize.y * 0.5f, 0.f);
	ColliderDesc.vRotation = _float3(0.0f, XMConvertToRadians(45.0f), 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_OBB"), (CComponent**)&m_pColliderCom[COLLIDER_OBB], &ColliderDesc)))
		return E_FAIL;

	/* Com_SPHERE */
	ZeroMemory(&ColliderDesc, sizeof ColliderDesc);

	ColliderDesc.fRadius = 1.5f;
	ColliderDesc.vPosition = _float3(0.f, ColliderDesc.fRadius - 1.5f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_SPHERE"), (CComponent**)&m_pColliderCom[COLLIDER_SPHERE], &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPsycho2::Bind_ShaderResources()
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

HRESULT CPsycho2::Collision_Object()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (m_pColliderCom[COLLIDER_AABB]->Collision((CCollider*)pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Missile"), TEXT("Com_OBB")))) {
		m_bCollision = true;
	}
	else
		m_bCollision = false;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CPsycho2::Add_Parts()
{
	CGameObject* pPart = nullptr;
	CGameObject* pPart2 = nullptr;
	CGameObject* pPart3 = nullptr;
	CGameObject* pPart4 = nullptr;
	CGameObject* pPart5 = nullptr;

	CPsycho_Face::WEAPONDESC			Desc;
	Desc.pBoneMatrix = m_pModelCom->Get_BoneCombinedMatrix("Head");
	Desc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	Desc.PivotMatrix = m_pModelCom->Get_PivotMatrix();
	Desc.iNumber = m_iNumber;

	CAxe::WEAPONDESC			Desc2;
	Desc2.pBoneMatrix = m_pModelCom->Get_BoneCombinedMatrix("R_Weapon_Bone");
	Desc2.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	Desc2.PivotMatrix = m_pModelCom->Get_PivotMatrix();
	Desc2.iNumber = m_iNumber;

	CPsycho_BodyCollider::COLLIDERDESC			Desc3;
	Desc3.pBoneMatrix = m_pModelCom->Get_BoneCombinedMatrix("Spine3");
	Desc3.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	Desc3.PivotMatrix = m_pModelCom->Get_PivotMatrix();
	Desc3.iNumber = m_iNumber;

	CPsycho_LegCollider::COLLIDERDESC			Desc4;
	Desc4.pBoneMatrix = m_pModelCom->Get_BoneCombinedMatrix("R_Toe");
	Desc4.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	Desc4.PivotMatrix = m_pModelCom->Get_PivotMatrix();
	Desc3.iNumber = m_iNumber;

	CPsycho_LegCollider::COLLIDERDESC			Desc5;
	Desc5.pBoneMatrix = m_pModelCom->Get_BoneCombinedMatrix("L_Toe");
	Desc5.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	Desc5.PivotMatrix = m_pModelCom->Get_PivotMatrix();
	Desc3.iNumber = m_iNumber;

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	pPart = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Psycho_Face"), &Desc);
	pPart2 = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Axe"), &Desc2);
	pPart3 = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Psycho_BodyCollider"), &Desc3);
	pPart4 = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Psycho_LegCollider"), &Desc4);
	pPart5 = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Psycho_LegCollider"), &Desc5);

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

	m_Parts.emplace(TEXT("Part_1"), pPart);
	m_Parts.emplace(TEXT("Part_2"), pPart2);
	m_Parts.emplace(TEXT("Part_3"), pPart3);
	m_Parts.emplace(TEXT("Part_4"), pPart4);
	m_Parts.emplace(TEXT("Part_5"), pPart5);

	Safe_Release(pGameInstance);

	return S_OK;
}

CPsycho2 * CPsycho2::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPsycho2*	pInstance = new CPsycho2(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Created : CPsycho2");
	}

	return pInstance;
}

CGameObject * CPsycho2::Clone(void * pArg)
{
	CPsycho2*	pInstance = new CPsycho2(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Cloned : CPsycho2");
	}

	return pInstance;
}

void CPsycho2::Free()
{
	__super::Free();

	m_pPlayerTransformCom = { nullptr };
	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pPlayerTransformCom);
	Safe_Release(m_pTextureCom);

	for (size_t i = 0; i < COLLIDER_END; i++)
		Safe_Release(m_pColliderCom[i]);

	for (auto& Pair : m_Parts)
		Safe_Release(Pair.second);

	m_Parts.clear();
}
