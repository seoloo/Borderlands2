#include "stdafx.h"
#include "..\Public\AreaFont.h"

#include "GameInstance.h"
#include "Player.h"
#include "TownPlayer.h"

CAreaFont::CAreaFont(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CAreaFont::CAreaFont(const CAreaFont & rhs)
	: CGameObject(rhs)
{
}

HRESULT CAreaFont::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CAreaFont::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

void CAreaFont::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	m_pObject = pGameInstance->Get_GameObject(g_CurLevel, L"Layer_Player");

	if (g_CurLevel == 3 || g_CurLevel == 5)
	{
		m_strLevel = TEXT(" - 임시 거처 - ");
	}
	else if (g_CurLevel == 4)
	{
		m_strLevel = TEXT(" - 파괴된 마을 - ");
	}

	m_fTimer += fTimeDelta;

	if (m_fTimer > 0.2f)
	{
		m_fAlpha -= 0.05f;
		m_fTimer = 0.f;
	}
	if (m_fAlpha <= 0.f)
	{
		m_fAlpha = 1.f;
		Set_Dead();
	}

	Safe_Release(pGameInstance);
}

void CAreaFont::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CAreaFont::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (g_CurLevel == 3)
	{
		if (FAILED(pGameInstance->Render_Font(TEXT("Font_136"), m_strLevel, _float2(g_iWinSizeX * 0.5f - 130.f, 100.f), XMVectorSet(0.f, 0.f, 0.f, m_fAlpha), 1.f)))
			return E_FAIL;
	}
	if (g_CurLevel == 4)
	{
		if (FAILED(pGameInstance->Render_Font(TEXT("Font_136"), m_strLevel, _float2(600.f, 100.f), XMVectorSet(1.f, 1.f, 1.f, m_fAlpha), 1.f)))
			return E_FAIL;
	}

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CAreaFont::Add_Components()
{
	/* Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	return S_OK;
}

CAreaFont * CAreaFont::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CAreaFont*	pInstance = new CAreaFont(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Created : CAreaFont");
	}

	return pInstance;
}

CGameObject * CAreaFont::Clone(void * pArg)
{
	CAreaFont*	pInstance = new CAreaFont(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Cloned : CAreaFont");
	}

	return pInstance;
}

void CAreaFont::Free()
{
	__super::Free();

	m_pObject = { nullptr };

	Safe_Release(m_pRendererCom);
}
