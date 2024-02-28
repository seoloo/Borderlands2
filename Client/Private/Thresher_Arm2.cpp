#include "stdafx.h"
#include "..\Public\Thresher_Arm2.h"

#include "GameInstance.h"
#include "Thresher_Arm2Collider.h"
#include "Thresher.h"
#include "Thresher_ArmsTopCollider.h"

CThresher_Arm2::CThresher_Arm2(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CLandObject(pDevice, pContext)
{
}

CThresher_Arm2::CThresher_Arm2(const CThresher_Arm2 & rhs)
	: CLandObject(rhs)
{
}

HRESULT CThresher_Arm2::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CThresher_Arm2::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	ARMDESC Desc = *(ARMDESC*)pArg;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, Desc.CreatePos);

	m_iNumbering = Desc.Numbering;

	if (FAILED(Add_Parts()))
		return E_FAIL;

	m_iMaxHp = 1;
	m_iHp = m_iMaxHp;
	
	m_state = STATE_APPEAR;

	random_device rd;
	mt19937 gen(rd());
	uniform_int_distribution<int> Bury(27, 41);
	uniform_int_distribution<int> Attack(12, 20);

	m_iRandomAttack = Attack(gen);
	m_iRandomBury = Bury(gen);

	return S_OK;
}

void CThresher_Arm2::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	m_iHp = Get_Hp();
	Set_Hp(m_iHp);

	m_pPlayerTransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_Transform")));
	m_pTransformCom->LookAt(m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION));


	m_pModelCom->Play_Animation(fTimeDelta);

	if (m_state == STATE_APPEAR)
		State_Appear(fTimeDelta);
	else if (m_state == STATE_ATTACK)
		State_Attack(fTimeDelta);
	else if (m_state == STATE_BURY)
		State_Bury(fTimeDelta);

	CTransform*				pTerrainTransform = (CTransform*)pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_BackGround"), TEXT("Com_Transform"));
	CVIBuffer_Terrain*		pTerrainVIBuffer = (CVIBuffer_Terrain*)pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_BackGround"), TEXT("Com_VIBuffer"));

	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		__super::SetUp_OnTerrain(m_pTransformCom->Get_State(CTransform::STATE_POSITION), pTerrainVIBuffer, pTerrainTransform));

	Safe_Release(pGameInstance);

	for (auto& Pair : m_Parts)
		Pair.second->Tick(fTimeDelta);
}

void CThresher_Arm2::Late_Tick(_float fTimeDelta)
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

HRESULT CThresher_Arm2::Render()
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
	return S_OK;
}

void CThresher_Arm2::State_Appear(_float fTimeDelta)
{
	// 42 ~ 47
	m_pModelCom->SetUp_Animation(42, true);

	if (m_pModelCom->Get_AnimEnd())
	{
		m_pModelCom->Set_AnimEnd(false);
		m_pModelCom->Stop_Animation(fTimeDelta, true);

		if (m_pModelCom->Get_Animation()[42]->Get_AnimEnd())
		{
				
			m_state = STATE_ATTACK;
		}
	}

}

void CThresher_Arm2::State_Attack(_float fTimeDelta)
{
	// 공격 : 12, 13,14, 15,16, 17, 18, 19,20

	m_pModelCom->SetUp_Animation(m_iRandomAttack, true);

	if (m_iHp <= 0)
	{
		if (m_pModelCom->Get_AnimEnd())
		{
			m_pModelCom->Set_AnimEnd(false);

			if (m_pModelCom->Get_Animation()[m_iRandomAttack]->Get_AnimEnd())
			{
				m_state = STATE_BURY;
			}
		}
	}
}

void CThresher_Arm2::State_Bury(_float fTimeDelta)
{
	// 들어감 27 ~ 41
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);
	m_pModelCom->SetUp_Animation(m_iRandomBury, true);

	if (m_pModelCom->Get_AnimEnd())
	{
		m_pModelCom->Set_AnimEnd(false);

		if (m_pModelCom->Get_Animation()[m_iRandomBury]->Get_AnimEnd())
		{
			m_pObject = pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Thresher"));

			if (m_iNumbering == 1)
			{
				if (m_bArmsDead)
				{
					dynamic_cast<CThresher*>(m_pObject)->Set_Arm2Dead(true);
				}
			}
			g_Arm2Number = 0;
				
			Set_Dead();
		}
	}

	Safe_Release(pGameInstance);
}

HRESULT CThresher_Arm2::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_ThresherArm2"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;	

	return S_OK;
}

HRESULT CThresher_Arm2::Add_Parts()
{
	CGameObject* pPartArmL1 = nullptr;
	CGameObject* pPartArmL2 = nullptr;
	CGameObject* pPartArmL3 = nullptr;
	CGameObject* pPartArmL4 = nullptr;
	CGameObject* pPartArmL5 = nullptr;
	CGameObject* pPartArmL6 = nullptr;
	CGameObject* pPartArmL7 = nullptr;
	CGameObject* pPartArmL8 = nullptr;
	CGameObject* pPartArmL9 = nullptr;
	CGameObject* pPartArmL10 = nullptr;
	CGameObject* pPartArmL11 = nullptr;
	CGameObject* pPartArmL12 = nullptr;
	CGameObject* pPartArmL13 = nullptr;
	CGameObject* pPartArmL14 = nullptr;
	CGameObject* pPartArmL15 = nullptr;
	CGameObject* pPartArmL16 = nullptr;

	CThresher_Arm2Collider::COLLIDERDESC			DescArmL1;
	ZeroMemory(&DescArmL1, sizeof DescArmL1);
	DescArmL1.pBoneMatrix = m_pModelCom->Get_BoneCombinedMatrix("ArmBone_15");
	DescArmL1.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	DescArmL1.PivotMatrix = m_pModelCom->Get_PivotMatrix();
	DescArmL1.Numbering = m_iNumbering;

	CThresher_Arm2Collider::COLLIDERDESC			DescArmL2;
	ZeroMemory(&DescArmL2, sizeof DescArmL2);
	DescArmL2.pBoneMatrix = m_pModelCom->Get_BoneCombinedMatrix("ArmBone_14");
	DescArmL2.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	DescArmL2.PivotMatrix = m_pModelCom->Get_PivotMatrix();
	DescArmL2.Numbering = m_iNumbering;

	CThresher_Arm2Collider::COLLIDERDESC			DescArmL3;
	ZeroMemory(&DescArmL3, sizeof DescArmL3);
	DescArmL3.pBoneMatrix = m_pModelCom->Get_BoneCombinedMatrix("ArmBone_13");
	DescArmL3.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	DescArmL3.PivotMatrix = m_pModelCom->Get_PivotMatrix();
	DescArmL3.Numbering = m_iNumbering;

	CThresher_Arm2Collider::COLLIDERDESC			DescArmL4;
	ZeroMemory(&DescArmL4, sizeof DescArmL4);
	DescArmL4.pBoneMatrix = m_pModelCom->Get_BoneCombinedMatrix("ArmBone_12");
	DescArmL4.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	DescArmL4.PivotMatrix = m_pModelCom->Get_PivotMatrix();
	DescArmL4.Numbering = m_iNumbering;

	CThresher_Arm2Collider::COLLIDERDESC			DescArmL5;
	ZeroMemory(&DescArmL5, sizeof DescArmL5);
	DescArmL5.pBoneMatrix = m_pModelCom->Get_BoneCombinedMatrix("ArmBone_11");
	DescArmL5.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	DescArmL5.PivotMatrix = m_pModelCom->Get_PivotMatrix();
	DescArmL5.Numbering = m_iNumbering;

	CThresher_Arm2Collider::COLLIDERDESC			DescArmL6;
	ZeroMemory(&DescArmL6, sizeof DescArmL6);
	DescArmL6.pBoneMatrix = m_pModelCom->Get_BoneCombinedMatrix("Tentacle_End");
	DescArmL6.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	DescArmL6.PivotMatrix = m_pModelCom->Get_PivotMatrix();
	DescArmL6.Numbering = m_iNumbering;

	CThresher_Arm2Collider::COLLIDERDESC			DescArmL7;
	ZeroMemory(&DescArmL7, sizeof DescArmL7);
	DescArmL7.pBoneMatrix = m_pModelCom->Get_BoneCombinedMatrix("Projecitle");
	DescArmL7.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	DescArmL7.PivotMatrix = m_pModelCom->Get_PivotMatrix();
	DescArmL7.Numbering = m_iNumbering;

	CThresher_Arm2Collider::COLLIDERDESC			DescArmL8;
	ZeroMemory(&DescArmL8, sizeof DescArmL8);
	DescArmL8.pBoneMatrix = m_pModelCom->Get_BoneCombinedMatrix("Needle");
	DescArmL8.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	DescArmL8.PivotMatrix = m_pModelCom->Get_PivotMatrix();
	DescArmL8.Numbering = m_iNumbering;

	CThresher_Arm2Collider::COLLIDERDESC			DescArmL9;
	ZeroMemory(&DescArmL9, sizeof DescArmL9);
	DescArmL9.pBoneMatrix = m_pModelCom->Get_BoneCombinedMatrix("ArmBone_10");
	DescArmL9.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	DescArmL9.PivotMatrix = m_pModelCom->Get_PivotMatrix();
	DescArmL9.Numbering = m_iNumbering;

	CThresher_Arm2Collider::COLLIDERDESC			DescArmL10;
	ZeroMemory(&DescArmL10, sizeof DescArmL10);
	DescArmL10.pBoneMatrix = m_pModelCom->Get_BoneCombinedMatrix("ArmBone_9");
	DescArmL10.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	DescArmL10.PivotMatrix = m_pModelCom->Get_PivotMatrix();
	DescArmL10.Numbering = m_iNumbering;

	CThresher_Arm2Collider::COLLIDERDESC			DescArmL11;
	ZeroMemory(&DescArmL11, sizeof DescArmL11);
	DescArmL11.pBoneMatrix = m_pModelCom->Get_BoneCombinedMatrix("ArmBone_8");
	DescArmL11.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	DescArmL11.PivotMatrix = m_pModelCom->Get_PivotMatrix();
	DescArmL11.Numbering = m_iNumbering;

	CThresher_Arm2Collider::COLLIDERDESC			DescArmL12;
	ZeroMemory(&DescArmL12, sizeof DescArmL12);
	DescArmL12.pBoneMatrix = m_pModelCom->Get_BoneCombinedMatrix("ArmBone_7");
	DescArmL12.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	DescArmL12.PivotMatrix = m_pModelCom->Get_PivotMatrix();
	DescArmL12.Numbering = m_iNumbering;

	CThresher_Arm2Collider::COLLIDERDESC			DescArmL13;
	ZeroMemory(&DescArmL13, sizeof DescArmL13);
	DescArmL13.pBoneMatrix = m_pModelCom->Get_BoneCombinedMatrix("ArmBone_6");
	DescArmL13.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	DescArmL13.PivotMatrix = m_pModelCom->Get_PivotMatrix();
	DescArmL13.Numbering = m_iNumbering;

	CThresher_Arm2Collider::COLLIDERDESC			DescArmL14;
	ZeroMemory(&DescArmL14, sizeof DescArmL14);
	DescArmL14.pBoneMatrix = m_pModelCom->Get_BoneCombinedMatrix("ArmBone_5");
	DescArmL14.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	DescArmL14.PivotMatrix = m_pModelCom->Get_PivotMatrix();
	DescArmL14.Numbering = m_iNumbering;

	CThresher_Arm2Collider::COLLIDERDESC			DescArmL15;
	ZeroMemory(&DescArmL15, sizeof DescArmL15);
	DescArmL15.pBoneMatrix = m_pModelCom->Get_BoneCombinedMatrix("ArmBone_4");
	DescArmL15.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	DescArmL15.PivotMatrix = m_pModelCom->Get_PivotMatrix();
	DescArmL15.Numbering = m_iNumbering;

	CThresher_ArmsTopCollider::COLLIDERDESC			DescArmL16;
	ZeroMemory(&DescArmL16, sizeof DescArmL16);
	DescArmL16.pBoneMatrix = m_pModelCom->Get_BoneCombinedMatrix("ArmBone_3");
	DescArmL16.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	DescArmL16.PivotMatrix = m_pModelCom->Get_PivotMatrix();
	DescArmL16.Numbering = m_iNumbering;



	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	pPartArmL1 = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Thresher_Arm2Collider"), &DescArmL1);
	pPartArmL2 = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Thresher_Arm2Collider"), &DescArmL2);
	pPartArmL3 = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Thresher_Arm2Collider"), &DescArmL3);
	pPartArmL4 = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Thresher_Arm2Collider"), &DescArmL4);
	pPartArmL5 = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Thresher_Arm2Collider"), &DescArmL5);
	pPartArmL6 = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Thresher_Arm2Collider"), &DescArmL6);
	pPartArmL7 = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Thresher_Arm2Collider"), &DescArmL7);
	pPartArmL8 = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Thresher_Arm2Collider"), &DescArmL8);
	pPartArmL9 = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Thresher_Arm2Collider"), &DescArmL9);
	pPartArmL10 = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Thresher_Arm2Collider"), &DescArmL10);
	pPartArmL11 = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Thresher_Arm2Collider"), &DescArmL11);
	pPartArmL12 = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Thresher_Arm2Collider"), &DescArmL12);
	pPartArmL13 = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Thresher_Arm2Collider"), &DescArmL13);
	pPartArmL14 = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Thresher_Arm2Collider"), &DescArmL14);
	pPartArmL15 = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Thresher_Arm2Collider"), &DescArmL15);
	pPartArmL16 = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Thresher_Arm2Collider"), &DescArmL16);

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
	if (nullptr == pPartArmL6)
		return E_FAIL;
	if (nullptr == pPartArmL7)
		return E_FAIL;
	if (nullptr == pPartArmL8)
		return E_FAIL;
	if (nullptr == pPartArmL9)
		return E_FAIL;
	if (nullptr == pPartArmL10)
		return E_FAIL;
	if (nullptr == pPartArmL11)
		return E_FAIL;
	if (nullptr == pPartArmL12)
		return E_FAIL;
	if (nullptr == pPartArmL13)
		return E_FAIL;
	if (nullptr == pPartArmL14)
		return E_FAIL;
	if (nullptr == pPartArmL15)
		return E_FAIL;
	if (nullptr == pPartArmL16)
		return E_FAIL;


	/* 중복태그에 대한 검사를 해^^ */
	m_Parts.emplace(TEXT("Part_ArmL1"), pPartArmL1);
	m_Parts.emplace(TEXT("Part_ArmL2"), pPartArmL2);
	m_Parts.emplace(TEXT("Part_ArmL3"), pPartArmL3);
	m_Parts.emplace(TEXT("Part_ArmL4"), pPartArmL4);
	m_Parts.emplace(TEXT("Part_ArmL5"), pPartArmL5);
	m_Parts.emplace(TEXT("Part_ArmL6"), pPartArmL6);
	m_Parts.emplace(TEXT("Part_ArmL7"), pPartArmL7);
	m_Parts.emplace(TEXT("Part_ArmL8"), pPartArmL8);
	m_Parts.emplace(TEXT("Part_ArmL9"), pPartArmL9);
	m_Parts.emplace(TEXT("Part_ArmL10"), pPartArmL10);
	m_Parts.emplace(TEXT("Part_ArmL11"), pPartArmL11);
	m_Parts.emplace(TEXT("Part_ArmL12"), pPartArmL12);
	m_Parts.emplace(TEXT("Part_ArmL13"), pPartArmL13);
	m_Parts.emplace(TEXT("Part_ArmL14"), pPartArmL14);
	m_Parts.emplace(TEXT("Part_ArmL15"), pPartArmL15);
	m_Parts.emplace(TEXT("Part_ArmL16"), pPartArmL16);

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CThresher_Arm2::Bind_ShaderResources()
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

CThresher_Arm2 * CThresher_Arm2::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CThresher_Arm2*	pInstance = new CThresher_Arm2(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Created : CThresher_Arm2");
	}

	return pInstance;
}

CGameObject * CThresher_Arm2::Clone(void * pArg)
{
	CThresher_Arm2*	pInstance = new CThresher_Arm2(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Cloned : CThresher_Arm2");
	}

	return pInstance;
}

void CThresher_Arm2::Free()
{
	__super::Free();

	m_pPlayerTransformCom = { nullptr };
	Safe_Release(m_pPlayerTransformCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);

	for (auto& Pair : m_Parts)
		Safe_Release(Pair.second);

	m_Parts.clear();
}
