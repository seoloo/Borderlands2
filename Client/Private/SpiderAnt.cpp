#include "stdafx.h"
#include "..\Public\SpiderAnt.h"

#include "GameInstance.h"
#include "SoundMgr.h"

#include "SpiderAnt_Collider.h"
#include "SpiderAnt_LegCollider.h"
#include "SpiderAnt_State.h"

#include "SpiderAnt_Idle.h"
#include "SpiderAnt_Attack.h"
#include "SpiderAnt_Walk.h"
#include "SpiderAnt_Shoot.h"
#include "SpiderAnt_HeadCollider.h"
#include "SpiderAnt_Appear.h"
#include "SpiderAnt_Dig.h"
#include "SpiderAnt_Attacked.h"
#include "SpiderAnt_Shoot2.h"
#include "SpiderAnt_Dead.h"
#include "SpiderAnt_Descend.h"

#include "Player.h"

CSpiderAnt::CSpiderAnt(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CLandObject(pDevice, pContext)
{
}

CSpiderAnt::CSpiderAnt(const CSpiderAnt & rhs)
	: CLandObject(rhs)
{
}

HRESULT CSpiderAnt::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSpiderAnt::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(Add_Parts()))
		return E_FAIL;

	m_pSpiderAnt_State = new CSpiderAnt_Descend;

	m_pSpiderAnt_State->Enter(this);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(90.f, 0.f, 90.f, 1.f));

	m_iMaxHp = 3000;
	//m_iMaxHp = 13;
	m_iHp = m_iMaxHp;


	// 땅에 파고 들기 19,
	// 땅에서 나오기 20, 21

	// 회피 32

	// 피격 36
	
	return S_OK;
}

void CSpiderAnt::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	m_iHp = Get_Hp();
	Set_Hp(m_iHp);

	if (!m_pSpiderAnt_State)
		return;

	m_pSpiderAnt_State->Tick(fTimeDelta);

	if (m_isDissolve)
	{
		if (m_iCounter == 0)
		{
			CSoundMgr::Get_Instance()->PlaySoundW(L"Monster_12.mp3", 1.f);
			m_iCounter++;
		}
		g_bMonsterCollidedMouse = false;
		g_strMonsterName = TEXT(" ");

		m_fDissolveTimer += fTimeDelta;
		if (m_fDissolveTimer >= 0.1f)
		{
			m_fDissolveValue += 0.05f;
			m_fDissolveTimer = 0.f;
		}
		if (m_fDissolveValue >= 1.3f)
		{
			g_bSpiderAntDead = true;
			g_strMonsterName == TEXT(" ");
			g_PlayerEXP += 50;
			Set_Dead();
		}
	}

	

	CTransform*				pTerrainTransform = (CTransform*)pGameInstance->Get_Component(g_CurLevel, TEXT("Layer_BackGround"), TEXT("Com_Transform"));
	CVIBuffer_Terrain*		pTerrainVIBuffer = (CVIBuffer_Terrain*)pGameInstance->Get_Component(g_CurLevel, TEXT("Layer_BackGround"), TEXT("Com_VIBuffer"));

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

void CSpiderAnt::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	Collision_Object();

	for (auto& Pair : m_Parts)
		Pair.second->Late_Tick(fTimeDelta);

	if (!Get_DeathAnim())
	{
		for (auto& Pair : m_Parts)
		{
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, Pair.second);
		}

		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
	}
}

HRESULT CSpiderAnt::Render()
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
#endif
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	if (!g_bSpiderAntScene && m_iHp > 0)
	{
		if (m_pColliderCom[COLLIDER_SPHERE]->Collision_Mouse())
		{
			g_bMonsterCollidedMouse = true;
			g_strMonsterName = TEXT("SpiderAnt");
			m_pObject = pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_SpiderAnt"));

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

void CSpiderAnt::Change_State(STATE state)
{
	m_pSpiderAnt_State->Exit(this);
	Safe_Delete(m_pSpiderAnt_State);

	switch (state) {
	case STATE_DESCEND:
		m_pSpiderAnt_State = new CSpiderAnt_Descend;
		break;
	case STATE_IDLE:
		m_pSpiderAnt_State = new CSpiderAnt_Idle;
		break;
	case STATE_WALK:
		m_pSpiderAnt_State = new CSpiderAnt_Walk;
		break;
	case STATE_ATTACK:
		m_pSpiderAnt_State = new CSpiderAnt_Attack;
		break;
	case STATE_SHOOT:
		m_pSpiderAnt_State = new CSpiderAnt_Shoot;
		break;
	case STATE_SHOOT2:
		m_pSpiderAnt_State = new CSpiderAnt_Shoot2;
		break;
	case STATE_DIG:
		m_pSpiderAnt_State = new CSpiderAnt_Dig;
		break;
	case STATE_APPEAR:
		m_pSpiderAnt_State = new CSpiderAnt_Appear;
		break;
	case STATE_ATTACKED:
		m_pSpiderAnt_State = new CSpiderAnt_Attacked;
		break;
	case STATE_DEAD:
		m_pSpiderAnt_State = new CSpiderAnt_Dead;
		break;
	default:
		break;
	}

	m_pSpiderAnt_State->Enter(this);
}

HRESULT CSpiderAnt::Add_Components()
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

	TransformDesc.fSpeedPerSec = 0.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(__super::Add_Component(g_CurLevel, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom))) 
		return E_FAIL;

	/* Com_Model */
	if (FAILED(__super::Add_Component(g_CurLevel, TEXT("Prototype_Component_Model_SpiderAnt"),
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

	ColliderDesc.fRadius = 6.f;
	ColliderDesc.vPosition = _float3(0.f, ColliderDesc.fRadius - 6.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_SPHERE"), (CComponent**)&m_pColliderCom[COLLIDER_SPHERE], &ColliderDesc)))
		return E_FAIL;

	/* Com_Navigation */
	CNavigation::NAVIDESC		NaviDesc;

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CTransform* pTerrainTransform = (CTransform*)pGameInstance->Get_Component(g_CurLevel, TEXT("Layer_BackGround"), TEXT("Com_Transform"));

	NaviDesc.pWorldMatrix = pTerrainTransform->Get_WorldMatrixPtr();


	NaviDesc.iCurrentIndex = 73;

	Safe_Release(pGameInstance);
	if (FAILED(__super::Add_Component(g_CurLevel, TEXT("Prototype_Component_Navigation"),
		TEXT("Com_Navigation"), (CComponent**)&m_pNavigationCom, &NaviDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CSpiderAnt::Add_Parts()
{
	CGameObject* pPart = nullptr;
	CGameObject* pPart2 = nullptr;
	CGameObject* pPart3 = nullptr;
	CGameObject* pPart4 = nullptr;

	CSpiderAnt_Collider::COLLIDERDESC			Desc;
	Desc.pBoneMatrix = m_pModelCom->Get_BoneCombinedMatrix("Tail3");
	Desc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	Desc.PivotMatrix = m_pModelCom->Get_PivotMatrix();

	CSpiderAnt_LegCollider::COLLIDERDESC			Desc2;
	Desc2.pBoneMatrix = m_pModelCom->Get_BoneCombinedMatrix("LHand");
	Desc2.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	Desc2.PivotMatrix = m_pModelCom->Get_PivotMatrix();

	CSpiderAnt_LegCollider::COLLIDERDESC			Desc3;
	Desc3.pBoneMatrix = m_pModelCom->Get_BoneCombinedMatrix("RHand");
	Desc3.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	Desc3.PivotMatrix = m_pModelCom->Get_PivotMatrix();

	CSpiderAnt_HeadCollider::COLLIDERDESC			Desc4;
	Desc4.pBoneMatrix = m_pModelCom->Get_BoneCombinedMatrix("Head");
	Desc4.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	Desc4.PivotMatrix = m_pModelCom->Get_PivotMatrix();

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	pPart = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_SpiderAnt_Collider"), &Desc);
	pPart2 = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_SpiderAnt_LegCollider"), &Desc2);
	pPart3 = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_SpiderAnt_LegCollider"), &Desc3);
	pPart4 = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_SpiderAnt_HeadCollider"), &Desc4);

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

HRESULT CSpiderAnt::Collision_Object()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (m_pColliderCom[COLLIDER_SPHERE]->Collision((CCollider*)pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_OBB")))) {
		m_bCollided = true;
	}
	else
		m_bCollided = false;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CSpiderAnt::Bind_ShaderResources()
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

CSpiderAnt * CSpiderAnt::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CSpiderAnt*	pInstance = new CSpiderAnt(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Created : CSpiderAnt");
	}

	return pInstance;
}

CGameObject * CSpiderAnt::Clone(void * pArg)
{
	CSpiderAnt*	pInstance = new CSpiderAnt(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Cloned : CSpiderAnt");
	}

	return pInstance;
}

void CSpiderAnt::Free()
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

	if (m_pSpiderAnt_State)
	{
		m_pSpiderAnt_State->Exit(this);
		Safe_Delete(m_pSpiderAnt_State);

	}

	for (size_t i = 0; i < COLLIDER_END; i++)
		Safe_Release(m_pColliderCom[i]);

	for (auto& Pair : m_Parts)
		Safe_Release(Pair.second);

	m_Parts.clear();

}
