#include "stdafx.h"
#include "..\Public\Axe.h"

#include "GameInstance.h"
#include "SoundMgr.h"

#include "Psycho.h"
#include "Psycho2.h"
#include "Psycho3.h"
#include "Player.h"

CAxe::CAxe(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CAxe::CAxe(const CAxe & rhs)
	: CGameObject(rhs)
{
}

HRESULT CAxe::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CAxe::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	WEAPONDESC* pWeaponDesc = (WEAPONDESC*)pArg;

	m_pBoneMatrix = pWeaponDesc->pBoneMatrix;
	m_pParentMatrix = pWeaponDesc->pParentMatrix;
	m_PivotMatrix = pWeaponDesc->PivotMatrix;
	m_iNumber = pWeaponDesc->iNumber;
	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransformCom->Rotation(XMVectorSet(1.f, 0.f, 0.f, 1.f), XMConvertToRadians(270.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));
	
	return S_OK;
}

void CAxe::Tick(_float fTimeDelta)
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


	m_pPlayer = pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Player"));

	if (m_bCollision)
	{
		m_fTimer += fTimeDelta;
		if (m_fTimer > 0.2f)
		{
			dynamic_cast<CPlayer*>(m_pPlayer)->Set_Attacked(true);

			CSoundMgr::Get_Instance()->PlaySoundW(L"Monster_Whoosh.mp3", 1.f);
			random_device rd;
			mt19937 gen(rd());
			uniform_int_distribution<int> Demage(10, 15);

			if (dynamic_cast<CPlayer*>(m_pPlayer)->Get_Shield() > 0)
			{
				dynamic_cast<CPlayer*>(m_pPlayer)->Decrease_Shield(Demage(gen));
				dynamic_cast<CPlayer*>(m_pPlayer)->Set_MonsterAtt(50);
			}
			else
				dynamic_cast<CPlayer*>(m_pPlayer)->Decrease_Hp(Demage(gen));

			m_fTimer = 0.f;
		}
	}
	else
		dynamic_cast<CPlayer*>(m_pPlayer)->Set_Attacked(false);

	if (m_iNumber == 1)
	{
		m_pObject = pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Psycho"));
		if (dynamic_cast<CPsycho*>(m_pObject)->Get_DeathAnim())
		{
			m_fDissolveTimer += fTimeDelta;
			if (m_fDissolveTimer >= 0.1f)
			{
				m_fDissolveValue += 0.1f;
				m_fDissolveTimer = 0.f;
			}
		}
	}
	else if (m_iNumber == 2)
	{
		m_pObject = pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Psycho2"));
		if (dynamic_cast<CPsycho2*>(m_pObject)->Get_DeathAnim())
		{
			m_fDissolveTimer += fTimeDelta;
			if (m_fDissolveTimer >= 0.1f)
			{
				m_fDissolveValue += 0.1f;
				m_fDissolveTimer = 0.f;
			}
		}
	}
	else if (m_iNumber == 3)
	{
		m_pObject = pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Psycho3"));
		if (dynamic_cast<CPsycho3*>(m_pObject)->Get_DeathAnim())
		{
			m_fDissolveTimer += fTimeDelta;
			if (m_fDissolveTimer >= 0.1f)
			{
				m_fDissolveValue += 0.1f;
				m_fDissolveTimer = 0.f;
			}
		}
	}

	for (size_t i = 0; i < COLLIDER_END; i++)
	{
		if (nullptr != m_pColliderCom[i])
			m_pColliderCom[i]->Tick(XMLoadFloat4x4(&m_WorldMatrix));
	}

	Safe_Release(pGameInstance);

}

void CAxe::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
	Collision_Object();
}

HRESULT CAxe::Render()
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
	return S_OK;
}

HRESULT CAxe::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMesh"), 
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom))) 
		return E_FAIL;

	/* Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Axe"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;	

	/* Com_AABB */
	CCollider::COLLIDERDESC		ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof ColliderDesc);

	ColliderDesc.vSize = _float3(2.f, 3.0f, 5.0f);
	ColliderDesc.vPosition = _float3(0.f, ColliderDesc.vSize.y * 0.5f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_AABB"), (CComponent**)&m_pColliderCom[COLLIDER_AABB], &ColliderDesc)))
		return E_FAIL;

	/* Com_OBB */
	ZeroMemory(&ColliderDesc, sizeof ColliderDesc);

	ColliderDesc.vSize = _float3(20.0f, 70.0f, 20.0f);
	ColliderDesc.vPosition = _float3(0.f, ColliderDesc.vSize.y * 0.5f, 0.f);
	ColliderDesc.vRotation = _float3(0.0f, XMConvertToRadians(45.0f), 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_OBB"), (CComponent**)&m_pColliderCom[COLLIDER_OBB], &ColliderDesc)))
		return E_FAIL;

	/* Com_SPHERE */
	ZeroMemory(&ColliderDesc, sizeof ColliderDesc);

	ColliderDesc.fRadius = 0.5f;
	ColliderDesc.vPosition = _float3(0.f, ColliderDesc.fRadius, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_SPHERE"), (CComponent**)&m_pColliderCom[COLLIDER_SPHERE], &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CAxe::Bind_ShaderResources()
{
	/*if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;*/

	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;

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

HRESULT CAxe::Collision_Object()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (m_pColliderCom[COLLIDER_OBB]->Collision((CCollider*)pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_OBB")))) {
		m_bCollision = true;
	}
	else
		m_bCollision = false;


	Safe_Release(pGameInstance);

	return S_OK;
}

CAxe * CAxe::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CAxe*	pInstance = new CAxe(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Created : CAxe");
	}

	return pInstance;
}

CGameObject * CAxe::Clone(void * pArg)
{
	CAxe*	pInstance = new CAxe(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Cloned : CAxe");
	}

	return pInstance;
}

void CAxe::Free()
{
	__super::Free();

	m_pPlayer = { nullptr };
	m_pObject = { nullptr };
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pTextureCom);

	for (size_t i = 0; i < COLLIDER_END; i++)
		Safe_Release(m_pColliderCom[i]);
}
