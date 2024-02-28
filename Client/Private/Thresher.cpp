#include "stdafx.h"
#include "..\Public\Thresher.h"

#include "GameInstance.h"
#include "SoundMgr.h"

#include "Thresher_Collider.h"
#include "Thresher_TentacleCollider.h"

#include "Thresher_State.h"
#include "Thresher_Dig.h"
#include "Thresher_Idle.h"
#include "Thresher_Appear.h"
#include "Thresher_Attack.h"
#include "Thresher_FirstAppear.h"
#include "Thresher_Summon.h"
#include "Thresher_Dead.h"
#include "Thresher_SecondAppear.h"

CThresher::CThresher(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CLandObject(pDevice, pContext)
{
}

CThresher::CThresher(const CThresher & rhs)
	: CLandObject(rhs)
{
}

HRESULT CThresher::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CThresher::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(Add_Parts()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(90.f, 0.f, 100.f, 1.f));

	m_pThresher_State = new CThresher_Idle;
	m_pThresher_State->Enter(this);

	m_iMaxHp = 3000;
	m_iHp = m_iMaxHp;
	
	return S_OK;
}

void CThresher::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (!m_pThresher_State)
		return;

	m_iHp = Get_Hp();
	Set_Hp(m_iHp);

	m_pThresher_State->Tick(fTimeDelta);

	if (Get_DeathAnim())
	{
		g_bThirdMissionClear = true;
		g_bMonsterCollidedMouse = false;
		CSoundMgr::Get_Instance()->StopAll();
		Set_Dead();
	}

	//m_pTransformCom->LookAt(m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION));

	

	CTransform*				pTerrainTransform = (CTransform*)pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_BackGround"), TEXT("Com_Transform"));
	CVIBuffer_Terrain*		pTerrainVIBuffer = (CVIBuffer_Terrain*)pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_BackGround"), TEXT("Com_VIBuffer"));

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

void CThresher::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

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

HRESULT CThresher::Render()
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

		m_pShaderCom->Begin(0);

		m_pModelCom->Render(i);
	}

//#ifdef _DEBUG
//
//	for (size_t i = 0; i < COLLIDER_END; i++)
//	{
//		if (nullptr != m_pColliderCom[i])
//			m_pColliderCom[i]->Render();
//	}
//
//#endif
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	
	if (m_States != STATE_SUMMON || m_States != STATE_DEAD)
	{
		if (m_pColliderCom[COLLIDER_SPHERE]->Collision_Mouse())
		{
			g_bMonsterCollidedMouse = true;
			g_strMonsterName = TEXT("Thresher");
			g_iMonsterHp = m_iHp;
		}
		else
		{
			g_bMonsterCollidedMouse = false;
		}
	}
	return S_OK;
}

void CThresher::ChangeState(STATES states)
{
	m_pThresher_State->Exit(this);
	Safe_Delete(m_pThresher_State);

	switch (states)
	{
	case STATE_IDLE:
		m_pThresher_State = new CThresher_Idle;
		break;
	case STATE_FIRSTAPPEAR:
		m_pThresher_State = new CThresher_FirstAppear;
		break;
	case STATE_SECONDAPPEAR:
		m_pThresher_State = new CThresher_SecondAppear;
		break;
	case STATE_APPEAR:
		m_pThresher_State = new CThresher_Appear;
		break;
	case STATE_DIG:
		m_pThresher_State = new CThresher_Dig;
		break;
	case STATE_ATTACK:
		m_pThresher_State = new CThresher_Attack;
		break;
	case STATE_SUMMON:
		m_pThresher_State = new CThresher_Summon;
		break;
	case STATE_DEAD:
		m_pThresher_State = new CThresher_Dead;
		break;
	default:
		break;
	}

	m_pThresher_State->Enter(this);
}

HRESULT CThresher::Add_Components()
{
	/* Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;
	
	/* Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimMesh"), 
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom))) 
		return E_FAIL;

	/* Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Thresher"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;	

	/* Com_AABB */
	CCollider::COLLIDERDESC		ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof ColliderDesc);

	ColliderDesc.vSize = _float3(0.7f, 1.f, 0.7f);
	ColliderDesc.vPosition = _float3(0.f, ColliderDesc.vSize.y * 0.5f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_AABB"), (CComponent**)&m_pColliderCom[COLLIDER_AABB], &ColliderDesc)))
		return E_FAIL;

	/* Com_OBB */
	ZeroMemory(&ColliderDesc, sizeof ColliderDesc);

	ColliderDesc.vSize = _float3(5.f, 5.f, 5.f);
	ColliderDesc.vPosition = _float3(0.f, ColliderDesc.vSize.y * 0.5f, 0.f);
	ColliderDesc.vRotation = _float3(0.0f, XMConvertToRadians(45.0f), 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_OBB"), (CComponent**)&m_pColliderCom[COLLIDER_OBB], &ColliderDesc)))
		return E_FAIL;

	/* Com_SPHERE */
	ZeroMemory(&ColliderDesc, sizeof ColliderDesc);

	ColliderDesc.fRadius = 30.f;
	ColliderDesc.vPosition = _float3(0.f, ColliderDesc.fRadius, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_SPHERE"), (CComponent**)&m_pColliderCom[COLLIDER_SPHERE], &ColliderDesc)))
		return E_FAIL;

	/* Com_Navigation */
	CNavigation::NAVIDESC		NaviDesc;

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CTransform* pTerrainTransform = (CTransform*)pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_BackGround"), TEXT("Com_Transform"));

	NaviDesc.pWorldMatrix = pTerrainTransform->Get_WorldMatrixPtr();


	NaviDesc.iCurrentIndex = 73;

	Safe_Release(pGameInstance);
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"),
		TEXT("Com_Navigation"), (CComponent**)&m_pNavigationCom, &NaviDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CThresher::Add_Parts()
{
	CGameObject* pPartEyeL1 = nullptr;
	CGameObject* pPartEyeL2 = nullptr;
	CGameObject* pPartEyeL3 = nullptr;
	CGameObject* pPartEyeR1 = nullptr;
	CGameObject* pPartEyeR2 = nullptr;
	CGameObject* pPartEyeR3 = nullptr;

	CGameObject* pPartArmL1 = nullptr;
	CGameObject* pPartArmL2 = nullptr;
	CGameObject* pPartArmL3 = nullptr;
	CGameObject* pPartArmL4 = nullptr;
	CGameObject* pPartArmL5 = nullptr;

	CGameObject* pPartArmR1 = nullptr;
	CGameObject* pPartArmR2 = nullptr;
	CGameObject* pPartArmR3 = nullptr;
	CGameObject* pPartArmR4 = nullptr;
	CGameObject* pPartArmR5 = nullptr;

	CThresher_Collider::COLLIDERDESC			DescL1;
	ZeroMemory(&DescL1, sizeof DescL1);
	DescL1.pBoneMatrix = m_pModelCom->Get_BoneCombinedMatrix("L_Eye01");
	DescL1.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	DescL1.PivotMatrix = m_pModelCom->Get_PivotMatrix();

	CThresher_Collider::COLLIDERDESC			DescL2;
	ZeroMemory(&DescL2, sizeof DescL2);
	DescL2.pBoneMatrix = m_pModelCom->Get_BoneCombinedMatrix("L_Eye02");
	DescL2.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	DescL2.PivotMatrix = m_pModelCom->Get_PivotMatrix();

	CThresher_Collider::COLLIDERDESC			DescL3;
	ZeroMemory(&DescL3, sizeof DescL3);
	DescL3.pBoneMatrix = m_pModelCom->Get_BoneCombinedMatrix("L_Eye03");
	DescL3.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	DescL3.PivotMatrix = m_pModelCom->Get_PivotMatrix();

	CThresher_Collider::COLLIDERDESC			DescR1;
	ZeroMemory(&DescR1, sizeof DescR1);
	DescR1.pBoneMatrix = m_pModelCom->Get_BoneCombinedMatrix("R_Eye01");
	DescR1.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	DescR1.PivotMatrix = m_pModelCom->Get_PivotMatrix();

	CThresher_Collider::COLLIDERDESC			DescR2;
	ZeroMemory(&DescR2, sizeof DescR2);
	DescR2.pBoneMatrix = m_pModelCom->Get_BoneCombinedMatrix("R_Eye02");
	DescR2.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	DescR2.PivotMatrix = m_pModelCom->Get_PivotMatrix();

	CThresher_Collider::COLLIDERDESC			DescR3;
	ZeroMemory(&DescR3, sizeof DescR3);
	DescR3.pBoneMatrix = m_pModelCom->Get_BoneCombinedMatrix("R_Eye03");
	DescR3.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	DescR3.PivotMatrix = m_pModelCom->Get_PivotMatrix();

	CThresher_TentacleCollider::COLLIDERDESC			DescArmL1;
	ZeroMemory(&DescArmL1, sizeof DescArmL1);
	DescArmL1.pBoneMatrix = m_pModelCom->Get_BoneCombinedMatrix("L_Front_Tentacle04");
	DescArmL1.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	DescArmL1.PivotMatrix = m_pModelCom->Get_PivotMatrix();

	CThresher_TentacleCollider::COLLIDERDESC			DescArmL2;
	ZeroMemory(&DescArmL2, sizeof DescArmL2);
	DescArmL2.pBoneMatrix = m_pModelCom->Get_BoneCombinedMatrix("L_Front_Tentacle05");
	DescArmL2.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	DescArmL2.PivotMatrix = m_pModelCom->Get_PivotMatrix();

	CThresher_TentacleCollider::COLLIDERDESC			DescArmL3;
	ZeroMemory(&DescArmL3, sizeof DescArmL3);
	DescArmL3.pBoneMatrix = m_pModelCom->Get_BoneCombinedMatrix("L_Front_Tentacle06");
	DescArmL3.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	DescArmL3.PivotMatrix = m_pModelCom->Get_PivotMatrix();

	CThresher_TentacleCollider::COLLIDERDESC			DescArmL4;
	ZeroMemory(&DescArmL4, sizeof DescArmL4);
	DescArmL4.pBoneMatrix = m_pModelCom->Get_BoneCombinedMatrix("L_Front_Tentacle07");
	DescArmL4.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	DescArmL4.PivotMatrix = m_pModelCom->Get_PivotMatrix();

	CThresher_TentacleCollider::COLLIDERDESC			DescArmL5;
	ZeroMemory(&DescArmL5, sizeof DescArmL5);
	DescArmL5.pBoneMatrix = m_pModelCom->Get_BoneCombinedMatrix("L_Front_Tentacle08");
	DescArmL5.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	DescArmL5.PivotMatrix = m_pModelCom->Get_PivotMatrix();

	CThresher_TentacleCollider::COLLIDERDESC			DescArmR1;
	ZeroMemory(&DescArmR1, sizeof DescArmR1);
	DescArmR1.pBoneMatrix = m_pModelCom->Get_BoneCombinedMatrix("R_Front_Tentacle04");
	DescArmR1.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	DescArmR1.PivotMatrix = m_pModelCom->Get_PivotMatrix();

	CThresher_TentacleCollider::COLLIDERDESC			DescArmR2;
	ZeroMemory(&DescArmR2, sizeof DescArmR2);
	DescArmR2.pBoneMatrix = m_pModelCom->Get_BoneCombinedMatrix("R_Front_Tentacle05");
	DescArmR2.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	DescArmR2.PivotMatrix = m_pModelCom->Get_PivotMatrix();

	CThresher_TentacleCollider::COLLIDERDESC			DescArmR3;
	ZeroMemory(&DescArmR3, sizeof DescArmR3);
	DescArmR3.pBoneMatrix = m_pModelCom->Get_BoneCombinedMatrix("R_Front_Tentacle06");
	DescArmR3.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	DescArmR3.PivotMatrix = m_pModelCom->Get_PivotMatrix();

	CThresher_TentacleCollider::COLLIDERDESC			DescArmR4;
	ZeroMemory(&DescArmR4, sizeof DescArmR4);
	DescArmR4.pBoneMatrix = m_pModelCom->Get_BoneCombinedMatrix("R_Front_Tentacle07");
	DescArmR4.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	DescArmR4.PivotMatrix = m_pModelCom->Get_PivotMatrix();

	CThresher_TentacleCollider::COLLIDERDESC			DescArmR5;
	ZeroMemory(&DescArmR5, sizeof DescArmR5);
	DescArmR5.pBoneMatrix = m_pModelCom->Get_BoneCombinedMatrix("R_Front_Tentacle08");
	DescArmR5.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	DescArmR5.PivotMatrix = m_pModelCom->Get_PivotMatrix();



	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	pPartEyeL1 = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Thresher_Collider"), &DescL1);
	pPartEyeL2 = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Thresher_Collider"), &DescL2);
	pPartEyeL3 = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Thresher_Collider"), &DescL3);
	pPartEyeR1 = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Thresher_Collider"), &DescR1);
	pPartEyeR2 = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Thresher_Collider"), &DescR2);
	pPartEyeR3 = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Thresher_Collider"), &DescR3);

	pPartArmL1 = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Thresher_Collider"), &DescArmL1);
	pPartArmL2 = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Thresher_TentacleCollider"), &DescArmL2);
	pPartArmL3 = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Thresher_TentacleCollider"), &DescArmL3);
	pPartArmL4 = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Thresher_TentacleCollider"), &DescArmL4);
	pPartArmL5 = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Thresher_TentacleCollider"), &DescArmL5);
	
	pPartArmR1 = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Thresher_Collider"), &DescArmR1);
	pPartArmR2 = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Thresher_TentacleCollider"), &DescArmR2);
	pPartArmR3 = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Thresher_TentacleCollider"), &DescArmR3);
	pPartArmR4 = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Thresher_TentacleCollider"), &DescArmR4);
	pPartArmR5 = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Thresher_TentacleCollider"), &DescArmR5);

	if (nullptr == pPartEyeL1)
		return E_FAIL;
	if (nullptr == pPartEyeL2)
		return E_FAIL;
	if (nullptr == pPartEyeL3)
		return E_FAIL;
	if (nullptr == pPartEyeR1)
		return E_FAIL;
	if (nullptr == pPartEyeR2)
		return E_FAIL;
	if (nullptr == pPartEyeR3)
		return E_FAIL;
	if (nullptr == pPartArmL1)
		return E_FAIL;
	if (nullptr == pPartArmL2)
		return E_FAIL;
	if (nullptr == pPartArmL3)
		return E_FAIL;
	if (nullptr == pPartArmL4)
		return E_FAIL;
	if (nullptr == pPartArmL5)
		return E_FAIL;
	if (nullptr == pPartArmR1)
		return E_FAIL;
	if (nullptr == pPartArmR2)
		return E_FAIL;
	if (nullptr == pPartArmR3)
		return E_FAIL;
	if (nullptr == pPartArmR4)
		return E_FAIL;
	if (nullptr == pPartArmR5)
		return E_FAIL;

	/* 중복태그에 대한 검사를 해^^ */
	m_Parts.emplace(TEXT("Part_EyeL1"), pPartEyeL1);
	m_Parts.emplace(TEXT("Part_EyeL2"), pPartEyeL2);
	m_Parts.emplace(TEXT("Part_EyeL3"), pPartEyeL3);
	m_Parts.emplace(TEXT("Part_EyeR1"), pPartEyeR1);
	m_Parts.emplace(TEXT("Part_EyeR2"), pPartEyeR2);
	m_Parts.emplace(TEXT("Part_EyeR3"), pPartEyeR3);

	m_Parts.emplace(TEXT("Part_ArmL1"), pPartArmL1);
	m_Parts.emplace(TEXT("Part_ArmL2"), pPartArmL2);
	m_Parts.emplace(TEXT("Part_ArmL3"), pPartArmL3);
	m_Parts.emplace(TEXT("Part_ArmL4"), pPartArmL4);
	m_Parts.emplace(TEXT("Part_ArmL5"), pPartArmL5);
	
	m_Parts.emplace(TEXT("Part_ArmR1"), pPartArmR1);
	m_Parts.emplace(TEXT("Part_ArmR2"), pPartArmR2);
	m_Parts.emplace(TEXT("Part_ArmR3"), pPartArmR3);
	m_Parts.emplace(TEXT("Part_ArmR4"), pPartArmR4);
	m_Parts.emplace(TEXT("Part_ArmR5"), pPartArmR5);

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CThresher::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;


	Safe_Release(pGameInstance);

	return S_OK;
}

CThresher * CThresher::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CThresher*	pInstance = new CThresher(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Created : CThresher");
	}

	return pInstance;
}

CGameObject * CThresher::Clone(void * pArg)
{
	CThresher*	pInstance = new CThresher(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Cloned : CThresher");
	}

	return pInstance;
}

void CThresher::Free()
{
	__super::Free();
	m_pPlayerTransformCom = { nullptr };
	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pPlayerTransformCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);

	if (m_pThresher_State)
	{
		m_pThresher_State->Exit(this);
		Safe_Delete(m_pThresher_State);
	}

	for (auto& Pair : m_Parts)
		Safe_Release(Pair.second);

	m_Parts.clear();

	for (size_t i = 0; i < COLLIDER_END; i++)
		Safe_Release(m_pColliderCom[i]);
}
