#include "stdafx.h"
#include "..\Public\ShieldBar.h"

#include "GameInstance.h"
#include "Player.h"
#include "TownPlayer.h"

CShieldBar::CShieldBar(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CShieldBar::CShieldBar(const CShieldBar & rhs)
	: CGameObject(rhs)
{
}

HRESULT CShieldBar::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CShieldBar::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();


	m_fSizeX = 300;
	m_fSizeY = 50;
	m_fX = 80;
	m_fY = g_iWinSizeY - 100;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransformCom->Scaled(_float3(m_fSizeX, m_fSizeY, 1.f));

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f, 1.f));

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(g_iWinSizeX, g_iWinSizeY, 0.f, 1.f));

	return S_OK;
}

void CShieldBar::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	m_pObject = pGameInstance->Get_GameObject(g_CurLevel, L"Layer_Player");

	if (g_CurLevel == 3 || g_CurLevel == 5)
	{
		m_iPlayerShield = dynamic_cast<CTownPlayer*>(m_pObject)->Get_Shield();
	}
	else if (g_CurLevel == 4)
	{

		m_iPlayerShield = dynamic_cast<CPlayer*>(m_pObject)->Get_Shield();
	}

	m_fSizeX = m_iPlayerShield * 2 + 2.f;

	if (m_fSizeX <= 0.f)
		m_fSizeX = 0.1f;

	m_pTransformCom->Scaled(_float3(m_fSizeX , m_fSizeY, 1.f));

	_vector taf = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f, 1.f));

	Safe_Release(pGameInstance);
}

void CShieldBar::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CShieldBar::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;	

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	m_pShaderCom->Begin(5);

	m_pVIBufferCom->Render();

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CShieldBar::Add_Components()
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
	if (FAILED(__super::Add_Component(g_CurLevel, TEXT("Prototype_Component_Texture_ShieldBar"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CShieldBar::Bind_ShaderResources()
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

CShieldBar * CShieldBar::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CShieldBar*	pInstance = new CShieldBar(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Created : CShieldBar");
	}

	return pInstance;
}

CGameObject * CShieldBar::Clone(void * pArg)
{
	CShieldBar*	pInstance = new CShieldBar(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Cloned : CShieldBar");
	}

	return pInstance;
}

void CShieldBar::Free()
{
	__super::Free();

	m_pObject = { nullptr };

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
}
