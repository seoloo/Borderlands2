#include "stdafx.h"
#include "..\Public\LoadingUI.h"

#include "GameInstance.h"

CLoadingUI::CLoadingUI(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CLoadingUI::CLoadingUI(const CLoadingUI& rhs)
	: CGameObject(rhs)
{
}

HRESULT CLoadingUI::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoadingUI::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_fSizeX = g_iWinSizeX;
	m_fSizeY = g_iWinSizeY;
	m_fX = g_iWinSizeX * 0.5f;
	m_fY = g_iWinSizeY * 0.5f;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransformCom->Scaled(_float3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f, 1.f));

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(g_iWinSizeX, g_iWinSizeY, 0.f, 1.f));

	if (g_bTownEnd == false)
		m_pTextureCom = m_pTextureTownCom;
	else if (g_bTownEnd == true && g_bGamePlayEnd == false)
		m_pTextureCom = m_pTextureGamePlayCom;
	else if (g_bGamePlayEnd == true && g_bTownEnd == true)
		m_pTextureCom = m_pTextureEndingCom;;

	return S_OK;
}

void CLoadingUI::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CLoadingUI::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CLoadingUI::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(0);

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CLoadingUI::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_LOADING, TEXT("Prototype_Component_Texture_LoadingUI"),
		TEXT("Com_TextureGamePlay"), (CComponent**)&m_pTextureGamePlayCom)))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_LOADING, TEXT("Prototype_Component_Texture_LoadingTown"),
		TEXT("Com_TextureTown"), (CComponent**)&m_pTextureTownCom)))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_LOADING, TEXT("Prototype_Component_Texture_LoadingEnding"),
		TEXT("Com_TextureEnding"), (CComponent**)&m_pTextureEndingCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoadingUI::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResourceView(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;

	return S_OK;
}

CLoadingUI* CLoadingUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CLoadingUI*	pInstance = new CLoadingUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Created : CLoadingUI");
	}

	return pInstance;
}

CGameObject * CLoadingUI::Clone(void * pArg)
{
	CLoadingUI*	pInstance = new CLoadingUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Cloned : CLoadingUI");
	}

	return pInstance;
}

void CLoadingUI::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTextureEndingCom);
	Safe_Release(m_pTextureGamePlayCom);
	Safe_Release(m_pTextureTownCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
}
