#include "stdafx.h"
#include "TreasureChest.h"

#include "GameInstance.h"
#include "SoundMgr.h"
#include "TownPlayer.h"
#include "Player.h"

#include "HealthVial.h"
#include "AmmoPickUp.h"

CTreasureChest::CTreasureChest(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CLandObject(pDevice, pContext)
{
}

CTreasureChest::CTreasureChest(const CTreasureChest & rhs)
	: CLandObject(rhs)
{
}

HRESULT CTreasureChest::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTreasureChest::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(Add_Parts()))
		return E_FAIL;

	m_pModelCom->SetUp_Animation(3);

	m_State = STATES_CLOSE;

	if (g_CurLevel == 3)
	{
		random_device		RandomDevice;
		mt19937_64			RandomNumber;
		uniform_real_distribution<float>		RandomX;
		uniform_real_distribution<float>		RandomZ;

		RandomNumber = mt19937_64(RandomDevice());

		RandomX = uniform_real_distribution<float>(10.f, 20.f);
		RandomZ = uniform_real_distribution<float>(10.f, 20.f);

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(RandomX(RandomNumber), 10.f, RandomZ(RandomNumber), 1.f));
		//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(5.f, 0.f, 10.f, 1.f));
	}
	else if (g_CurLevel == 4)
	{
		random_device		RandomDevice;
		mt19937_64			RandomNumber;
		uniform_real_distribution<float>		RandomX;
		uniform_real_distribution<float>		RandomZ;

		RandomNumber = mt19937_64(RandomDevice());

		RandomX = uniform_real_distribution<float>(70.f, 115.f);
		RandomZ = uniform_real_distribution<float>(20.f, 80.f);

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(RandomX(RandomNumber), 10.f, RandomZ(RandomNumber), 1.f));

	}

	m_vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);


	Safe_Release(pGameInstance);

	return S_OK;
}

void CTreasureChest::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (m_vPos.m128_f32[1] > 0.f)
	{
		_float DecreasePos = 0.05f;
		m_vPos.m128_f32[1] -= DecreasePos;
	}
	
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vPos);

	if (m_State == STATES_CLOSE)
		Close(fTimeDelta);
	else if (m_State == STATES_OPENING)
		Opening(fTimeDelta);
	else if (m_State == STATES_OPENED)
		Opened(fTimeDelta);

	Collision_Object();

	if (m_bCollision)
	{
		g_bInteractionItem = true;
		g_InteractionCount = 0;
	}
	else
	{
		g_bInteractionItem = false;
		g_InteractionCount = 0;
	}

	if (m_bCollision && m_iCount == 0)
	{
		m_State = STATES_OPENING;
		m_iCount++;
	}

	if (m_bCollision && KEYDOWN('G'))
	{
		m_pObject = pGameInstance->Get_GameObject(g_CurLevel, L"Layer_Player");

		if (g_CurLevel == 3)
		{
			dynamic_cast<CTownPlayer*>(m_pObject)->Increase_Hp(100);
			g_AmountOfBullet += 50;
		}
		else if (g_CurLevel == 4)
		{
			dynamic_cast<CPlayer*>(m_pObject)->Increase_Hp(100);
			dynamic_cast<CPlayer*>(m_pObject)->Increase_Shield(50);
			g_AmountOfBullet += 50;
		}

		g_bInteractionDestroy = true;
	}

	if (g_bInteractionDestroy)
	{
		m_fDissolveTimer += fTimeDelta;

		if (m_fDissolveTimer > 0.1f)
		{
			m_fDissolveValue += 0.05f;
			m_fDissolveTimer = 0.f;
		}
		if (m_fDissolveValue > 1.5f)
		{
			g_bInteractionItem = false;
			g_InteractionCount = 0;
			m_fDissolveValue = 0.f;
			m_fDissolveTimer = 0.f;
			g_bInteractionDestroy = false;
			CSoundMgr::Get_Instance()->PlaySoundW(L"Bonus.wav", 1.f);

			Set_Dead();
		}
	}


	Safe_Release(pGameInstance);

	for (size_t i = 0; i < COLLIDER_END; i++)
	{
		if (nullptr != m_pColliderCom[i])
			m_pColliderCom[i]->Tick(m_pTransformCom->Get_WorldMatrix_Float4x4());
	}

	for (auto& Pair : m_Parts)
		Pair.second->Tick(fTimeDelta);
}

void CTreasureChest::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);


	for (auto& Pair : m_Parts)
		Pair.second->Late_Tick(fTimeDelta);

	if (/* 화면에 그려진다면 */1)
	{
		for (auto& Pair : m_Parts)
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, Pair.second);

		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);

	#ifdef _DEBUG
			for (size_t i = 0; i < COLLIDER_END; i++)
				m_pRendererCom->Add_DebugComponent(m_pColliderCom[i]);
	#endif
	}
}

HRESULT CTreasureChest::Render()
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

	return S_OK;
}

void CTreasureChest::Close(_float fTimeDelta)
{
	m_pModelCom->Play_Animation(fTimeDelta);

	m_pModelCom->SetUp_Animation(0);
}

void CTreasureChest::Opening(_float fTimeDelta)
{
	m_pModelCom->Play_Animation(fTimeDelta * 2.f);

	m_pModelCom->SetUp_Animation(1);

	m_pModelCom->Set_AnimEnd(false);

	if (m_pModelCom->Get_Animation()[1]->Get_AnimEnd())
	{
		m_pModelCom->Get_Animation()[1]->Set_AnimEnd(false);
		m_State = STATES_OPENED;
	}
}

void CTreasureChest::Opened(_float fTimeDelta)
{
	m_pModelCom->Play_Animation(fTimeDelta);

	m_pModelCom->SetUp_Animation(2);
}

HRESULT CTreasureChest::Add_Components()
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
	if (FAILED(__super::Add_Component(g_CurLevel, TEXT("Prototype_Component_Model_TreasureChest"),
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

	ColliderDesc.fRadius = 1.f;
	ColliderDesc.vPosition = _float3(0.f, ColliderDesc.fRadius - 1.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_SPHERE"), (CComponent**)&m_pColliderCom[COLLIDER_SPHERE], &ColliderDesc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CTreasureChest::Bind_ShaderResources()
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

HRESULT CTreasureChest::Collision_Object()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (m_pColliderCom[COLLIDER_SPHERE]->Collision((CCollider*)pGameInstance->Get_Component(g_CurLevel, TEXT("Layer_Player"), TEXT("Com_SPHERE")))) {
		m_bCollision = true;
		m_bCheck = true;
	}
	else
	{
		m_bCollision = false;
		m_bCheck = false;

	}

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CTreasureChest::Add_Parts()
{
	CGameObject* pPart = nullptr;
	CGameObject* pPart2 = nullptr;
	CGameObject* pPart3 = nullptr;

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CHealthVial::ITEMDESC			Desc;
	Desc.pBoneMatrix = m_pModelCom->Get_BoneCombinedMatrix("Bone_Bottom_Shelf");
	Desc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	Desc.PivotMatrix = m_pModelCom->Get_PivotMatrix();

	CAmmoPickUp::ITEMDESC			Desc2;
	Desc2.pBoneMatrix = m_pModelCom->Get_BoneCombinedMatrix("Bone_Top_Shelf_L");
	Desc2.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	Desc2.PivotMatrix = m_pModelCom->Get_PivotMatrix();
	Desc2.iNumber = 1;

	CAmmoPickUp::ITEMDESC			Desc3;
	Desc3.pBoneMatrix = m_pModelCom->Get_BoneCombinedMatrix("Bone_Top_Shelf_R");
	Desc3.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	Desc3.PivotMatrix = m_pModelCom->Get_PivotMatrix();
	Desc3.iNumber = 2;

	if (g_CurLevel == 3)
	{
		pPart = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_TownHealthVial"), &Desc);
		pPart2 = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_TownAmmoPickUp"), &Desc2);
		pPart3 = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_TownAmmoPickUp"), &Desc3);

	}
	else if (g_CurLevel == 4)
	{
		pPart = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_HealthVial"), &Desc);
		pPart2 = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_AmmoPickUp"), &Desc2);
		pPart3 = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_AmmoPickUp"), &Desc3);

	}

	if (nullptr == pPart)
		return E_FAIL;
	if (nullptr == pPart2)
		return E_FAIL;
	if (nullptr == pPart3)
		return E_FAIL;

	m_Parts.emplace(TEXT("Part_1"), pPart);
	m_Parts.emplace(TEXT("Part_2"), pPart2);
	m_Parts.emplace(TEXT("Part_3"), pPart3);

	Safe_Release(pGameInstance);

	return S_OK;
}

CTreasureChest * CTreasureChest::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CTreasureChest*	pInstance = new CTreasureChest(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Created : CTreasureChest");
	}

	return pInstance;
}

CGameObject * CTreasureChest::Clone(void * pArg)
{
	CTreasureChest*	pInstance = new CTreasureChest(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Cloned : CTreasureChest");
	}

	return pInstance;
}

void CTreasureChest::Free()
{
	__super::Free();

	m_pPlayerTransformCom = { nullptr };
	m_pObject = { nullptr };

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pPlayerTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pTextureCom);

	for (auto& Pair : m_Parts)
		Safe_Release(Pair.second);

	for (size_t i = 0; i < COLLIDER_END; i++)
		Safe_Release(m_pColliderCom[i]);
}
