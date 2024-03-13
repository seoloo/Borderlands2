#include "stdafx.h"
#include "Snowing.h"

#include "GameInstance.h"

CSnowing::CSnowing(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CSnowing::CSnowing(const CSnowing & rhs)
	: CGameObject(rhs)
{
}

HRESULT CSnowing::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSnowing::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;
		
	return S_OK;
}

void CSnowing::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);	

	m_pVIBufferCom->Tick(fTimeDelta);	
}

void CSnowing::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_BLEND, this);
}

HRESULT CSnowing::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;	

	m_pShaderCom->Begin(5);

	m_pVIBufferCom->Render();	

	return S_OK;
}

HRESULT CSnowing::Add_Components()
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
	if (FAILED(__super::Add_Component(g_CurLevel, TEXT("Prototype_Component_Shader_VtxPointInstance"), 
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom))) 
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Component(g_CurLevel, TEXT("Prototype_Component_Texture_Snowing"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* Com_VIBuffer */
	CVIBuffer_Point_Instance::POINTINSTANCEDESC		PointInstanceDesc;
	PointInstanceDesc.iNumInstance = 2000;
	PointInstanceDesc.vPivotPos = _float4(0.f, 30.f, 0.f, 1.f);
	PointInstanceDesc.vRange = _float3(250.f, 5.f, 250.f);
	PointInstanceDesc.fMinSpeed = 3.0f;
	PointInstanceDesc.fMaxSpeed = 15.0f;
	PointInstanceDesc.bFalling = true;

	if (FAILED(__super::Add_Component(g_CurLevel, 
		TEXT("Prototype_Component_VIBuffer_Point_Instance"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom, &PointInstanceDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CSnowing::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;	

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", &pGameInstance->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;
	
	Safe_Release(pGameInstance);

	if (FAILED(m_pTextureCom->Bind_ShaderResourceView(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;

	return S_OK;
}

CSnowing * CSnowing::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CSnowing*	pInstance = new CSnowing(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Created : CSnowing");
	}

	return pInstance;
}

CGameObject * CSnowing::Clone(void * pArg)
{
	CSnowing*	pInstance = new CSnowing(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Cloned : CSnowing");
	}

	return pInstance;
}

void CSnowing::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
