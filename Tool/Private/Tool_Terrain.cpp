#include "Tool_Terrain.h"

#include "GameInstance.h"
#include "Imgui_Manager.h"
#include "Input_Device.h"

CTool_Terrain::CTool_Terrain(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CTool_Terrain::CTool_Terrain(const CTool_Terrain & rhs)
	: CGameObject(rhs)
{
}

HRESULT CTool_Terrain::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTool_Terrain::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;
	
	m_pTextureCom_Origin[TYPE_DIFFUSE] = m_pTextureCom_Ground[TYPE_DIFFUSE];
	m_pTextureCom_Origin[TYPE_MASK] = m_pTextureCom_Ground[TYPE_MASK];
	m_pTextureCom_Origin[TYPE_BRUSH] = m_pTextureCom_Ground[TYPE_BRUSH];
	m_pTextureCom_Origin[TYPE_END] = m_pTextureCom_Ground[TYPE_END];

	return S_OK;
}

void CTool_Terrain::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	Change_Terrain();

	_float3 result = {};
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (pGameInstance->Get_DIMouseState(CInput_Device::DIMK_RBUTTON) & 0x8000)
	{
		if (m_pVIBufferCom->Intersect_Polygons(m_pTransformCom->Get_WorldMatrix(), &result))
		{
			m_pImgui_Manager->Get_Instance()->Set_PickingPos(result);
			m_pImgui_Manager->Get_Instance()->Set_PickTerrain(true);
		}
	}

	Safe_Release(pGameInstance);
}

void CTool_Terrain::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CTool_Terrain::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(0);

	m_pVIBufferCom->Render();

#ifdef _DEBUG
	m_pNavigationCom->Render();
#endif

	return S_OK;
}

void CTool_Terrain::Change_Terrain()
{
	if (m_pImgui_Manager->Get_Instance()->Get_TerrainName() == TEXT("SlateRock3_Dif") &&
		m_pImgui_Manager->Get_Instance()->Get_Apply() == true)
	{
		m_pTextureCom_Origin[TYPE_DIFFUSE] = m_pTextureCom_Rock[TYPE_DIFFUSE];
		m_pTextureCom_Origin[TYPE_MASK] = m_pTextureCom_Rock[TYPE_MASK];
		m_pTextureCom_Origin[TYPE_BRUSH] = m_pTextureCom_Rock[TYPE_BRUSH];
		m_pTextureCom_Origin[TYPE_END] = m_pTextureCom_Rock[TYPE_END];

		m_pImgui_Manager->Get_Instance()->Set_Apply(false);
	}

	if (m_pImgui_Manager->Get_Instance()->Get_TerrainName() == TEXT("Bright_Lava_Tile2_Dif") &&
		m_pImgui_Manager->Get_Instance()->Get_Apply() == true)
	{
		m_pTextureCom_Origin[TYPE_DIFFUSE] = m_pTextureCom_Lava[TYPE_DIFFUSE];
		m_pTextureCom_Origin[TYPE_MASK] = m_pTextureCom_Lava[TYPE_MASK];
		m_pTextureCom_Origin[TYPE_BRUSH] = m_pTextureCom_Lava[TYPE_BRUSH];
		m_pTextureCom_Origin[TYPE_END] = m_pTextureCom_Lava[TYPE_END];

		m_pImgui_Manager->Get_Instance()->Set_Apply(false);
	}

	if (m_pImgui_Manager->Get_Instance()->Get_TerrainName() == TEXT("SnowDrift_01_Dif") &&
		m_pImgui_Manager->Get_Instance()->Get_Apply() == true)
	{
		m_pTextureCom_Origin[TYPE_DIFFUSE] = m_pTextureCom_Snow[TYPE_DIFFUSE];
		m_pTextureCom_Origin[TYPE_MASK] = m_pTextureCom_Snow[TYPE_MASK];
		m_pTextureCom_Origin[TYPE_BRUSH] = m_pTextureCom_Snow[TYPE_BRUSH];
		m_pTextureCom_Origin[TYPE_END] = m_pTextureCom_Snow[TYPE_END];

		m_pImgui_Manager->Get_Instance()->Set_Apply(false);
	}

	if (m_pImgui_Manager->Get_Instance()->Get_TerrainName() == TEXT("Snow") &&
		m_pImgui_Manager->Get_Instance()->Get_Apply() == true)
	{
		m_pTextureCom_Origin[TYPE_DIFFUSE] = m_pTextureCom_Snow2[TYPE_DIFFUSE];
		m_pTextureCom_Origin[TYPE_MASK] = m_pTextureCom_Snow2[TYPE_MASK];
		m_pTextureCom_Origin[TYPE_BRUSH] = m_pTextureCom_Snow2[TYPE_BRUSH];
		m_pTextureCom_Origin[TYPE_END] = m_pTextureCom_Snow2[TYPE_END];

		m_pImgui_Manager->Get_Instance()->Set_Apply(false);
	}

	if (m_pImgui_Manager->Get_Instance()->Get_TerrainName() == TEXT("FrozenLake") &&
		m_pImgui_Manager->Get_Instance()->Get_Apply() == true)
	{
		m_pTextureCom_Origin[TYPE_DIFFUSE] = m_pTextureCom_FrozenLake[TYPE_DIFFUSE];
		m_pTextureCom_Origin[TYPE_MASK] = m_pTextureCom_FrozenLake[TYPE_MASK];
		m_pTextureCom_Origin[TYPE_BRUSH] = m_pTextureCom_FrozenLake[TYPE_BRUSH];
		m_pTextureCom_Origin[TYPE_END] = m_pTextureCom_FrozenLake[TYPE_END];

		m_pImgui_Manager->Get_Instance()->Set_Apply(false);
	}

	if (m_pImgui_Manager->Get_Instance()->Get_TerrainName() == TEXT("Grassland") &&
		m_pImgui_Manager->Get_Instance()->Get_Apply() == true)
	{
		m_pTextureCom_Origin[TYPE_DIFFUSE] = m_pTextureCom_Grassland[TYPE_DIFFUSE];
		m_pTextureCom_Origin[TYPE_MASK] = m_pTextureCom_Grassland[TYPE_MASK];
		m_pTextureCom_Origin[TYPE_BRUSH] = m_pTextureCom_Grassland[TYPE_BRUSH];
		m_pTextureCom_Origin[TYPE_END] = m_pTextureCom_Grassland[TYPE_END];

		m_pImgui_Manager->Get_Instance()->Set_Apply(false);
	}
}

HRESULT CTool_Terrain::Add_Components()
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

	/* Com_Navigation */
	CNavigation::NAVIDESC		NaviDesc;
	NaviDesc.pWorldMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	NaviDesc.iCurrentIndex = -1;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Navigation"),
		TEXT("Com_Navigation"), (CComponent**)&m_pNavigationCom, &NaviDesc)))
		return E_FAIL;

#pragma region Textures
	/* Com_Texture_Snow */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_SnowDrift_01_Dif"),
		TEXT("Com_Texture_Snow"), (CComponent**)&m_pTextureCom_Snow)))
		return E_FAIL;

	/* Com_Texture_Snow */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Snow"),
		TEXT("Com_Texture_Snow2"), (CComponent**)&m_pTextureCom_Snow2)))
		return E_FAIL;

	/* Com_Texture_Rock */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_SlateRock3_Dif"),
		TEXT("Com_Texture_Rock"), (CComponent**)&m_pTextureCom_Rock)))
		return E_FAIL;

	/* Com_Texture_Lava */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Bright_Lava_Tile2_Dif"),
		TEXT("Com_Texture_Lava"), (CComponent**)&m_pTextureCom_Lava)))
		return E_FAIL;

	/* Com_Texture_Ground */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Tile0"),
		TEXT("Com_Texture_Ground"), (CComponent**)&m_pTextureCom_Ground)))
		return E_FAIL;

	/* Com_Texture_Grassland */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Grassland"),
		TEXT("Com_Texture_Grassland"), (CComponent**)&m_pTextureCom_Grassland)))
		return E_FAIL;

	/* Com_Texture_FrozenLake */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_FrozenLake"),
		TEXT("Com_Texture_FrozenLake"), (CComponent**)&m_pTextureCom_FrozenLake)))
		return E_FAIL;
#pragma endregion

	return S_OK;
}

HRESULT CTool_Terrain::Bind_ShaderResources()
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

	Safe_Release(pGameInstance);

	if (FAILED(m_pTextureCom_Origin[TYPE_DIFFUSE]->Bind_ShaderResourceViews(m_pShaderCom, "g_DiffuseTexture")))
		return E_FAIL;

	/*if (FAILED(m_pTextureCom[TYPE_MASK]->Bind_ShaderResourceView(m_pShaderCom, "g_MaskTexture")))
		return E_FAIL;*/

	return S_OK;
}

CTool_Terrain * CTool_Terrain::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CTool_Terrain*	pInstance = new CTool_Terrain(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Created : CTool_Terrain");
	}

	return pInstance;
}

CGameObject * CTool_Terrain::Clone(void * pArg)
{
	CTool_Terrain*	pInstance = new CTool_Terrain(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Cloned : CTool_Terrain");
	}

	return pInstance;
}

void CTool_Terrain::Free()
{
	__super::Free();

	CImgui_Manager::Destroy_Instance();
	Safe_Release(m_pImgui_Manager);

	 Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);

	for (size_t i = 0; i < TYPE_END; i++)
	{
		Safe_Release(m_pTextureCom_Origin[i]);
		Safe_Release(m_pTextureCom_Snow2[i]);
		Safe_Release(m_pTextureCom_Snow[i]);
		Safe_Release(m_pTextureCom_Rock[i]);
		Safe_Release(m_pTextureCom_Lava[i]);
		Safe_Release(m_pTextureCom_Ground[i]);
		Safe_Release(m_pTextureCom_FrozenLake[i]);
		Safe_Release(m_pTextureCom_Grassland[i]);
	}
	Safe_Release(m_pNavigationCom);

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
}
