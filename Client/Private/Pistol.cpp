#include "stdafx.h"
#include "..\Public\Pistol.h"

#include "GameInstance.h"
#include "Muzzle.h"
#include "Player.h"
#include "TownPlayer.h"

CPistol::CPistol(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CPistol::CPistol(const CPistol & rhs)
	: CGameObject(rhs)
{
}

HRESULT CPistol::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPistol::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	WEAPONDESC* pWeaponDesc = (WEAPONDESC*)pArg;

	m_pBoneMatrix = pWeaponDesc->pBoneMatrix;
	m_pParentMatrix = pWeaponDesc->pParentMatrix;
	m_PivotMatrix = pWeaponDesc->PivotMatrix;

	if (FAILED(Add_Components()))
		return E_FAIL;

	/*if (FAILED(Add_Parts()))
		return E_FAIL;*/

	m_pTransformCom->Rotation(XMVectorSet(1.f, 0.f, 0.f, 1.f), XMConvertToRadians(270.f));

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-5.f, 3.f, 2.f, 1.f));

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	LIGHTDESC		LightDesc;
	ZeroMemory(&LightDesc, sizeof LightDesc);

	LightDesc.eType = LIGHTDESC::TYPE_POINT;
	LightDesc.vPosition = _float4(0.f, 0.f, 0.f, 0.f);
	LightDesc.fRange = 5.f;

	LightDesc.vDiffuse = _float4(1.f, 0.8f, 0.f, 1.f);
	LightDesc.vAmbient = _float4(0.4f, 0.2f, 0.f, 1.f);
	LightDesc.vSpecular = LightDesc.vDiffuse;

	/*if (g_CurLevel == 4)
	{*/
		if (FAILED(pGameInstance->Add_Lights(m_pDevice, m_pContext, LightDesc)))
			return E_FAIL;
	//}

	pGameInstance->Lights_TurnOff(true);

	Safe_Release(pGameInstance);

	return S_OK;
}

void CPistol::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	m_pPlayerTransform = dynamic_cast<CTransform*>(pGameInstance->Get_Component(g_CurLevel, TEXT("Layer_Player"), TEXT("Com_Transform")));
	XMStoreFloat4(&m_vPlayerPos, m_pPlayerTransform->Get_State(CTransform::STATE_POSITION));

	_matrix		BoneMatrix = XMLoadFloat4x4(m_pBoneMatrix);

	BoneMatrix.r[0] = XMVector3Normalize(BoneMatrix.r[0]);
	BoneMatrix.r[1] = XMVector3Normalize(BoneMatrix.r[1]);
	BoneMatrix.r[2] = XMVector3Normalize(BoneMatrix.r[2]);

	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix_Float4x4() * BoneMatrix *
		XMLoadFloat4x4(&m_PivotMatrix) *
		XMLoadFloat4x4(m_pParentMatrix));

	m_pModelCom->Play_Animation(fTimeDelta);

	if (pGameInstance->Get_DIMouseState(CInput_Device::DIMK_LBUTTON) & 0x8000)
	{
		if (g_BulletCount > 0)
		{
			m_pModelCom->SetUp_Animation(1);

			if (m_pModelCom->Get_AnimEnd())
			{
				m_pModelCom->Set_AnimEnd(false);

				if (g_CurLevel == 3)
				{
					if (FAILED(pGameInstance->Add_GameObject(g_CurLevel, TEXT("Layer_Muzzle"),
						TEXT("Prototype_GameObject_TownMuzzle"))))
						return;
				}

				if (g_CurLevel == 4)
				{
					if (FAILED(pGameInstance->Add_GameObject(g_CurLevel, TEXT("Layer_Muzzle"),
						TEXT("Prototype_GameObject_Muzzle"))))
						return;
				}
				/*if (FAILED(pGameInstance->Add_GameObject(g_CurLevel, TEXT("Layer_Effect_Blood"),
					TEXT("Prototype_GameObject_Catridge"))))
					return;*/

				if (m_pModelCom->Get_Animation()[1]->Get_AnimEnd())
				{
					m_pModelCom->Get_Animation()[1]->Set_AnimEnd(false);
					m_pModelCom->SetUp_Animation(4);

				}

				m_bFire = true;
			}
		}
	}
	else
		m_pModelCom->SetUp_Animation(4);

	if (m_bFire)
	{
		m_fTimer += fTimeDelta;
		pGameInstance->Update_LightsPos(m_vPlayerPos);
		pGameInstance->Lights_TurnOff(false);

		if (m_fTimer >= 0.2f)
		{
			m_bFire = false;
			m_fTimer = 0.f;
			pGameInstance->Update_LightsPos(_float4(0.f, 0.f, 0.f, 0.f));
		}
	}
	else
	{
		pGameInstance->Remove_PointLight();
		pGameInstance->Lights_TurnOff(true);
	}

	_vector pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	_float4x4 mat = m_WorldMatrix;

	for (auto& Pair : m_Parts)
		Pair.second->Tick(fTimeDelta);
	Safe_Release(pGameInstance);

}

void CPistol::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	//m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
	if (/* 화면에 그려진다면 */1)
	{
		for (auto& Pair : m_Parts)
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, Pair.second);

		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);

	}
}

HRESULT CPistol::Render()
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

HRESULT CPistol::Add_Components()
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
	if (FAILED(__super::Add_Component(g_CurLevel, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom))) 
		return E_FAIL;

	/* Com_Model */
	if (FAILED(__super::Add_Component(g_CurLevel, TEXT("Prototype_Component_Model_Pistol"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;	

	return S_OK;
}

HRESULT CPistol::Add_Parts()
{
	CGameObject* pPartObject = nullptr;

	/*무기*/
	CMuzzle::EFFECTDESC			Desc;
	Desc.pBoneMatrix = m_pModelCom->Get_BoneCombinedMatrix("Trigger");
	Desc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	Desc.PivotMatrix = m_pModelCom->Get_PivotMatrix();

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	pPartObject = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Muzzle"), &Desc);

	if (nullptr == pPartObject)
		return E_FAIL;

	/* 중복태그에 대한 검사를 해^^ */
	m_Parts.emplace(TEXT("Part_Muzzle"), pPartObject);

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CPistol::Bind_ShaderResources()
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

	Safe_Release(pGameInstance);

	return S_OK;
}

CPistol * CPistol::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPistol*	pInstance = new CPistol(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Created : CPistol");
	}

	return pInstance;
}

CGameObject * CPistol::Clone(void * pArg)
{
	CPistol*	pInstance = new CPistol(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Cloned : CPistol");
	}

	return pInstance;
}

void CPistol::Free()
{
	__super::Free();

	m_pPlayerTransform = { nullptr };
	m_pPlayer = { nullptr };

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);

	for (auto& Pair : m_Parts)
		Safe_Release(Pair.second);

	m_Parts.clear();

}
