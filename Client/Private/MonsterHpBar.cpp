#include "stdafx.h"
#include "..\Public\MonsterHpBar.h"

#include "GameInstance.h"
#include "Player.h"
#include "TownPlayer.h"

CMonsterHpBar::CMonsterHpBar(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CMonsterHpBar::CMonsterHpBar(const CMonsterHpBar & rhs)
	: CGameObject(rhs)
{
}

HRESULT CMonsterHpBar::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonsterHpBar::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	m_fSizeX = 300;
	m_fSizeY = 15;
	m_fX = g_iWinSizeY * 0.5f + 210.f;
	m_fY = 200.f;

	if (FAILED(Add_Components()))
		return E_FAIL;

	//m_pTransformCom->Scaled(_float3(m_fSizeX, m_fSizeY, 1.f));

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f, 1.f));

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(g_iWinSizeX, g_iWinSizeY, 0.f, 1.f));

	return S_OK;
}

void CMonsterHpBar::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	

	Safe_Release(pGameInstance);
}

void CMonsterHpBar::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CMonsterHpBar::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;	

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (!g_bMonsterCollidedMouse || !g_bPsychoCollidedMouse || !g_bPsychoCollidedMouse2 || !g_bPsychoCollidedMouse3 ||
		!g_bGoliathCollidedMouse || !g_bGoliathCollidedMouse2 || !g_bGoliathCollidedMouse3)
	{
		g_bDestroyedName = true;
		g_strMonsterName == TEXT(" ");
		Set_Dead();
	}

	if (g_bMonsterCollidedMouse || g_bPsychoCollidedMouse || g_bPsychoCollidedMouse2 || g_bPsychoCollidedMouse3 ||
		g_bGoliathCollidedMouse || g_bGoliathCollidedMouse2 || g_bGoliathCollidedMouse3)
	{
		if (g_strMonsterName == TEXT("Psycho") ||
			g_strMonsterName == TEXT("Stalker"))
			m_fSizeX = g_iMonsterHp * 1.f + 0.1f;

		if (g_strMonsterName == TEXT("Goliath"))
			m_fSizeX = g_iMonsterHp * 0.5f + 0.1f;

		if (g_strMonsterName == TEXT("SpiderAnt") ||
			g_strMonsterName == TEXT("Skag") ||
			g_strMonsterName == TEXT("Thresher"))
			m_fSizeX = g_iMonsterHp * 0.1f + 0.1f;
		m_pTransformCom->Scaled(_float3(m_fSizeX, m_fSizeY, 1.f));
	}
	
	m_pShaderCom->Begin(9);

	m_pVIBufferCom->Render();

	if (g_strMonsterName == TEXT("Hammerlock"))
	{
		if (FAILED(pGameInstance->Render_Font(TEXT("Font_136"), g_strMonsterName, _float2(g_iWinSizeX * 0.5f - 65.f, 210.f), XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.5f)))
			return E_FAIL;
	}
	if (g_strMonsterName == TEXT("Psycho") || g_strMonsterName == TEXT("Goliath") || g_strMonsterName == TEXT("Stalker"))
	{
		if (FAILED(pGameInstance->Render_Font(TEXT("Font_136"), g_strMonsterName, _float2(g_iWinSizeX * 0.5f - 35.f, 210.f), XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.5f)))
			return E_FAIL;
	}
	if (g_strMonsterName == TEXT("SpiderAnt") || g_strMonsterName == TEXT("Thresher"))
	{
		if (FAILED(pGameInstance->Render_Font(TEXT("Font_136"), g_strMonsterName, _float2(g_iWinSizeX * 0.5f - 45.f, 210.f), XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.5f)))
			return E_FAIL;
	}

	if (g_strMonsterName == TEXT("Skag"))
	{
		if (FAILED(pGameInstance->Render_Font(TEXT("Font_136"), g_strMonsterName, _float2(g_iWinSizeX * 0.5f - 25.f, 210.f), XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.5f)))
			return E_FAIL;
	}

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CMonsterHpBar::Add_Components()
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
	if (FAILED(__super::Add_Component(g_CurLevel, TEXT("Prototype_Component_Texture_MonsterHpBar"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonsterHpBar::Bind_ShaderResources()
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

CMonsterHpBar * CMonsterHpBar::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMonsterHpBar*	pInstance = new CMonsterHpBar(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Created : CMonsterHpBar");
	}

	return pInstance;
}

CGameObject * CMonsterHpBar::Clone(void * pArg)
{
	CMonsterHpBar*	pInstance = new CMonsterHpBar(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Cloned : CMonsterHpBar");
	}

	return pInstance;
}

void CMonsterHpBar::Free()
{
	__super::Free();

	m_pObject = { nullptr };

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
}
