#include "stdafx.h"
#include "..\Public\Goliath_ArmCollider.h"

#include "GameInstance.h"
#include "Player.h"

CGoliath_ArmCollider::CGoliath_ArmCollider(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CGoliath_ArmCollider::CGoliath_ArmCollider(const CGoliath_ArmCollider & rhs)
	: CGameObject(rhs)
{
}

HRESULT CGoliath_ArmCollider::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CGoliath_ArmCollider::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	COLLIDERDESC* pColliderDesc = (COLLIDERDESC*)pArg;

	m_pBoneMatrix = pColliderDesc->pBoneMatrix;
	m_pParentMatrix = pColliderDesc->pParentMatrix;
	m_PivotMatrix = pColliderDesc->PivotMatrix;

	if (FAILED(Add_Components()))
		return E_FAIL;


	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	Safe_Release(pGameInstance);
	
	return S_OK;
}

void CGoliath_ArmCollider::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	_matrix		BoneMatrix = XMLoadFloat4x4(m_pBoneMatrix);

	BoneMatrix.r[0] = XMVector3Normalize(BoneMatrix.r[0]);
	BoneMatrix.r[1] = XMVector3Normalize(BoneMatrix.r[1]);
	BoneMatrix.r[2] = XMVector3Normalize(BoneMatrix.r[2]);

	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix_Float4x4() *
		BoneMatrix *
		XMLoadFloat4x4(&m_PivotMatrix) *
		XMLoadFloat4x4(m_pParentMatrix));

	m_pObject = pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Player"));

	if (m_bCollision)
	{
		m_fTimer += fTimeDelta;
		if (m_fTimer > 0.2f)
		{
			dynamic_cast<CPlayer*>(m_pObject)->Set_Attacked(true);

			random_device rd;
			mt19937 gen(rd());
			uniform_int_distribution<int> Demage(10, 15);

			if (dynamic_cast<CPlayer*>(m_pObject)->Get_Shield() > 0)
			{
				dynamic_cast<CPlayer*>(m_pObject)->Decrease_Shield(Demage(gen));
				dynamic_cast<CPlayer*>(m_pObject)->Set_MonsterAtt(50);
			}
			else
				dynamic_cast<CPlayer*>(m_pObject)->Decrease_Hp(Demage(gen));

			m_fTimer = 0.f;
		}
	}
	else
		dynamic_cast<CPlayer*>(m_pObject)->Set_Attacked(false);

	Safe_Release(pGameInstance);

	for (size_t i = 0; i < COLLIDER_END; i++)
	{
		if (nullptr != m_pColliderCom[i])
			m_pColliderCom[i]->Tick(XMLoadFloat4x4(&m_WorldMatrix));
	}
}

void CGoliath_ArmCollider::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	Collision_Object();
}

HRESULT CGoliath_ArmCollider::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;	

	m_pShaderCom->Begin(0);

#ifdef _DEBUG

	for (size_t i = 0; i < COLLIDER_END; i++)
	{
		if (nullptr != m_pColliderCom[i])
			m_pColliderCom[i]->Render();
	}
#endif

	return S_OK;
}

HRESULT CGoliath_ArmCollider::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMesh"), 
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom))) 
		return E_FAIL;

	/* Com_AABB */
	CCollider::COLLIDERDESC		ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof ColliderDesc);

	ColliderDesc.vSize = _float3(55.0f, 75.0f, 55.0f);
	ColliderDesc.vPosition = _float3(0.f, ColliderDesc.vSize.y * 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_AABB"), (CComponent**)&m_pColliderCom[COLLIDER_AABB], &ColliderDesc)))
		return E_FAIL;

	/* Com_OBB */
	ZeroMemory(&ColliderDesc, sizeof ColliderDesc);

	ColliderDesc.vSize = _float3(10.0f, 10.0f, 10.0f);
	ColliderDesc.vPosition = _float3(0.f, ColliderDesc.vSize.y * 0.5f, 0.f);
	ColliderDesc.vRotation = _float3(0.0f, XMConvertToRadians(45.0f), 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_OBB"), (CComponent**)&m_pColliderCom[COLLIDER_OBB], &ColliderDesc)))
		return E_FAIL;

	/* Com_SPHERE */
	ZeroMemory(&ColliderDesc, sizeof ColliderDesc);

	ColliderDesc.fRadius = 30.f;
	ColliderDesc.vPosition = _float3(0.f, ColliderDesc.fRadius -3.f, -1.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_SPHERE"), (CComponent**)&m_pColliderCom[COLLIDER_SPHERE], &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CGoliath_ArmCollider::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CGoliath_ArmCollider::Collision_Object()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (m_pColliderCom[COLLIDER_AABB]->Collision((CCollider*)pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_OBB")))||
		m_pColliderCom[COLLIDER_OBB]->Collision((CCollider*)pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_OBB")))) {
		m_bCollision = true;
	}
	else
		m_bCollision = false;


	Safe_Release(pGameInstance);

	return S_OK;
}

CGoliath_ArmCollider * CGoliath_ArmCollider::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CGoliath_ArmCollider*	pInstance = new CGoliath_ArmCollider(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Created : CGoliath_ArmCollider");
	}

	return pInstance;
}

CGameObject * CGoliath_ArmCollider::Clone(void * pArg)
{
	CGoliath_ArmCollider*	pInstance = new CGoliath_ArmCollider(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Cloned : CGoliath_ArmCollider");
	}

	return pInstance;
}

void CGoliath_ArmCollider::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);

	for (size_t i = 0; i < COLLIDER_END; i++)
		Safe_Release(m_pColliderCom[i]);
}
