#include "Effect_Terrain.h"

#include "GameInstance.h"

CEffect_Terrain::CEffect_Terrain(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CEffect_Terrain::CEffect_Terrain(const CEffect_Terrain & rhs)
	: CGameObject(rhs)
{
}

HRESULT CEffect_Terrain::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_Terrain::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;
	
	return S_OK;
}

void CEffect_Terrain::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CEffect_Terrain::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CEffect_Terrain::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(0);

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CEffect_Terrain::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxNorTex"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Terrain"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_SnowTerrain"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;


	return S_OK;
}

HRESULT CEffect_Terrain::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	_float4x4 viewMatrix = pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW);
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &viewMatrix)))
		return E_FAIL;

	_float4x4 projMatrix = pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ);
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &projMatrix)))
		return E_FAIL;

	_float4 camPosition = pGameInstance->Get_CamPosition();
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", &camPosition, sizeof(_float4))))
		return E_FAIL;

	const LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(0);
	if (nullptr == pLightDesc)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDir", &pLightDesc->vDirection, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4))))
		return E_FAIL;

	Safe_Release(pGameInstance);

	if (FAILED(m_pTextureCom[TYPE_DIFFUSE]->Bind_ShaderResourceViews(m_pShaderCom, "g_DiffuseTexture")))
		return E_FAIL;

	return S_OK;
}

CEffect_Terrain * CEffect_Terrain::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CEffect_Terrain*	pInstance = new CEffect_Terrain(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Created : CEffect_Terrain");
	}

	return pInstance;
}

CGameObject * CEffect_Terrain::Clone(void * pArg)
{
	CEffect_Terrain*	pInstance = new CEffect_Terrain(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Cloned : CEffect_Terrain");
	}

	return pInstance;
}

void CEffect_Terrain::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);

	for (size_t i = 0; i < TYPE_END; i++)
	{
		Safe_Release(m_pTextureCom[i]);
	}

	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
}
