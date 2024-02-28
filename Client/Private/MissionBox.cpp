#include "stdafx.h"
#include "MissionBox.h"

#include "GameInstance.h"


bool g_bMissionClear;

CMissionBox::CMissionBox(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CMissionBox::CMissionBox(const CMissionBox & rhs)
	: CGameObject(rhs)
{
}

HRESULT CMissionBox::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMissionBox::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_fSizeX = 20;
	m_fSizeY = 20;
	m_fX = g_iWinSizeX - 50;
	m_fY = 250;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransformCom->Scaled(_float3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f, 1.f));

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(g_iWinSizeX, g_iWinSizeY, 0.f, 1.f));

	m_pTextureCom = m_pBoxTextureCom;

	return S_OK;
}

void CMissionBox::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);


	if (!g_bMissionClear)
		m_pTextureCom = m_pBoxTextureCom;
	else
		m_pTextureCom = m_pClearBoxTextureCom;

	Safe_Release(pGameInstance);
}

void CMissionBox::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CMissionBox::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;	

	m_pShaderCom->Begin(6);

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CMissionBox::Add_Components()
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

	if (FAILED(__super::Add_Component(g_CurLevel, TEXT("Prototype_Component_Texture_MissionBox"),
		TEXT("Com_BoxTexture"), (CComponent**)&m_pBoxTextureCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(g_CurLevel, TEXT("Prototype_Component_Texture_MissionClearBox"),
		TEXT("Com_ClearBoxTexture"), (CComponent**)&m_pClearBoxTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMissionBox::Bind_ShaderResources()
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

CMissionBox * CMissionBox::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMissionBox*	pInstance = new CMissionBox(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Created : CMissionBox");
	}

	return pInstance;
}

CGameObject * CMissionBox::Clone(void * pArg)
{
	CMissionBox*	pInstance = new CMissionBox(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Cloned : CMissionBox");
	}

	return pInstance;
}

void CMissionBox::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pBoxTextureCom);
	Safe_Release(m_pClearBoxTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
}
