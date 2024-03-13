#include "stdafx.h"
#include "Hammerlock.h"

#include "GameInstance.h"

#include "TownPlayer.h"
#include "Player.h"

#include "SoundMgr.h"

#include "DamageFont.h"

CHammerlock::CHammerlock(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CLandObject(pDevice, pContext)
{
}

CHammerlock::CHammerlock(const CHammerlock& rhs)
	: CLandObject(rhs)
{
}
HRESULT CHammerlock::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CHammerlock::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pModelCom->SetUp_Animation(3);

	if(g_CurLevel == 3 || g_CurLevel == 5)
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(10.f, 0.f, 10.f, 1.f));
	else if(g_CurLevel == 4)
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(5.f, 0.f, 117.f, 1.f));
		//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(15.f, 0.f, 15.f, 1.f));

	m_iAtt = 2;

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	Safe_Release(pGameInstance);

	return S_OK;
}

void CHammerlock::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	m_pModelCom->Play_Animation(fTimeDelta);
	m_pModelCom->SetUp_Animation(4);

	m_pModelCom->Stop_Animation(fTimeDelta, true);

	if (m_pModelCom->Get_Animation()[4]->Get_AnimEnd())
	{
		m_pModelCom->Get_Animation()[4]->Set_AnimEnd(false);
	}

	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	m_pPlayerTransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_Component(g_CurLevel, TEXT("Layer_Player"), TEXT("Com_Transform")));

	m_pTransformCom->LookAt(m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION));

	m_pObject = pGameInstance->Get_GameObject(g_CurLevel, L"Layer_Player");

	Collision_Object();

	if (m_bCollision)
	{
		g_bInteractionNPC = true;
	}
	else
	{
		g_bInteractionNPC = false;
		g_InteractionCount = 0;
	}

	if (m_bCollision)
	{
		/*dynamic_cast<CTownPlayer*>(m_pObject)->Set_Attacked(false);

		m_fTimer += fTimeDelta;
		if (m_fTimer > 0.5f)
		{
			if (g_CurLevel == 3)
			{
				dynamic_cast<CTownPlayer*>(m_pObject)->Set_Attacked(true);
				if (dynamic_cast<CTownPlayer*>(m_pObject)->Get_Shield() > 0)
				{
					dynamic_cast<CTownPlayer*>(m_pObject)->Decrease_Shield(10);
					dynamic_cast<CTownPlayer*>(m_pObject)->Set_MonsterAtt(50);
				}
				else
				{
					dynamic_cast<CTownPlayer*>(m_pObject)->Decrease_Hp(10);
				}

			}
			else if (g_CurLevel == 4)
			{
			}

			m_fTimer = 0.f;
		}*/
	}
	/*else
	{
		if (g_CurLevel == 3)
		{
			dynamic_cast<CTownPlayer*>(m_pObject)->Set_Attacked(false);
		}
		else if (g_CurLevel == 4)
		{
		}
	}*/


	/*m_fTimer += fTimeDelta;
	if (m_fTimer > 0.1f)
	{
		m_fValue += 0.05f;
		m_fTimer = 0.f;
	}
	if (m_fValue >= 1.5f)
		m_fValue = 0.f;*/

	/*m_fTimer += fTimeDelta;
	if (m_fTimer >= 1.f)
	{
		CVIBuffer_Point_Instance::POINTINSTANCEDESC		Desc;
		_vector TempPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		XMStoreFloat4(&Desc.vPivotPos, TempPos);
		Desc.iNum = 0;
		Desc.iDamage = 37;
		if (FAILED(pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Layer_Effect_Blast"),
			TEXT("Prototype_GameObject_TownDamageFont"), &Desc)))
			return;

		m_fTimer = 0.f;
	}*/

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
}

void CHammerlock::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOWDEPTH, this);
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);

#ifdef _DEBUG
	//if (/* 화면에 그려진다면 */1)
	//{

	//	for (size_t i = 0; i < COLLIDER_END; i++)
	//	{
	//		m_pRendererCom->Add_DebugComponent(m_pColliderCom[i]);
	//	}

	//	m_pRendererCom->Add_DebugComponent(m_pNavigationCom);
	//}
#endif
}

HRESULT CHammerlock::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;	

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_ShaderResourceView(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
			return E_FAIL;

		m_pShaderCom->Begin(1);

		m_pModelCom->Render(i);
	}

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CHammerlock::Render_ShadowDepth()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CHammerlock::Add_Components()
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

	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(__super::Add_Component(g_CurLevel, TEXT("Prototype_Component_Shader_VtxAnimMesh"), 
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom))) 
		return E_FAIL;

	/* Com_Model */
	if (FAILED(__super::Add_Component(g_CurLevel, TEXT("Prototype_Component_Model_Hammerlock"),
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

	ColliderDesc.vSize = _float3(1.0f, 1.0f, 1.0f);
	ColliderDesc.vPosition = _float3(0.f, ColliderDesc.vSize.y * 0.5f, 0.f);
	ColliderDesc.vRotation = _float3(0.0f, XMConvertToRadians(45.0f), 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_OBB"), (CComponent**)&m_pColliderCom[COLLIDER_OBB], &ColliderDesc)))
		return E_FAIL;

	/* Com_SPHERE */
	ZeroMemory(&ColliderDesc, sizeof ColliderDesc);

	ColliderDesc.fRadius = 3.f;
	ColliderDesc.vPosition = _float3(0.f, ColliderDesc.fRadius - 3.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_SPHERE"), (CComponent**)&m_pColliderCom[COLLIDER_SPHERE], &ColliderDesc)))
		return E_FAIL;


	/* Com_Navigation */
	CNavigation::NAVIDESC		NaviDesc;

	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CTransform*				pTerrainTransform = (CTransform*)pGameInstance->Get_Component(g_CurLevel, TEXT("Layer_BackGround"), TEXT("Com_Transform"));

	NaviDesc.pWorldMatrix = pTerrainTransform->Get_WorldMatrixPtr();

	Safe_Release(pGameInstance);	

	NaviDesc.iCurrentIndex = 2;

	if (FAILED(__super::Add_Component(g_CurLevel, TEXT("Prototype_Component_Navigation"),
		TEXT("Com_Navigation"), (CComponent**)&m_pNavigationCom, &NaviDesc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CHammerlock::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	_float4x4 view = pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW);
	_float4x4 proj = pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ);

	if (FAILED(m_pTextureCom->Bind_ShaderResourceView(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_AddDissolve", &m_fValue, sizeof(_float))))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CHammerlock::Collision_Object()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	/*if (m_pColliderCom[COLLIDER_SPHERE]->Collision((CCollider*)pGameInstance->Get_Component(g_CurLevel, TEXT("Layer_Player"), TEXT("Com_SPHERE")))) {
		m_bCollision = true;
	}
	else
		m_bCollision = false;*/

	Safe_Release(pGameInstance);

	return S_OK;
}

CHammerlock * CHammerlock::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CHammerlock*	pInstance = new CHammerlock(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Created : CHammerlock");
	}

	return pInstance;
}

CGameObject * CHammerlock::Clone(void * pArg)
{
	CHammerlock*	pInstance = new CHammerlock(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Cloned : CHammerlock");
	}

	return pInstance;
}

void CHammerlock::Free()
{
	__super::Free();

	m_pPlayerTransformCom = { nullptr };
	m_pObject = { nullptr };

	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pPlayerTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pTextureCom);

	for (size_t i = 0; i < COLLIDER_END; i++)
		Safe_Release(m_pColliderCom[i]);
	
}
