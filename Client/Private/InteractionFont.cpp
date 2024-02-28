#include "stdafx.h"
#include "InteractionFont.h"

#include "GameInstance.h"
#include "Player.h"
#include "TownPlayer.h"

CInteractionFont::CInteractionFont(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CInteractionFont::CInteractionFont(const CInteractionFont & rhs)
	: CGameObject(rhs)
{
}

HRESULT CInteractionFont::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CInteractionFont::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

void CInteractionFont::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (!g_bInteractionItem && !g_bInteractionNPC)
	{
		Set_Dead();
	}
	if (g_DialogueCount >= 3)
		Set_Dead();

	Safe_Release(pGameInstance);
}

void CInteractionFont::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CInteractionFont::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (g_bInteractionNPC)
	{
		if (g_CurLevel == 3)
		{
			if (FAILED(Town_NPCFont()))
				return E_FAIL;
		}
		else if (g_CurLevel == 4)
		{
			if (FAILED(GamePlay_NPCFont()))
				return E_FAIL;
		}
		else if (g_CurLevel == 5)
		{
			if (FAILED(Town_NPCFont()))
				return E_FAIL;
		}

	}

	if (g_bInteractionItem)
	{
		if (g_CurLevel == 3)
		{
			if (FAILED(Town_ItemFont()))
				return E_FAIL;
		}
		else if (g_CurLevel == 4)
		{
			if (FAILED(GamePlay_ItemFont()))
				return E_FAIL;
		}
	}
	
	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CInteractionFont::Town_NPCFont()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);
	m_strNPC = TEXT("´ëÈ­");

	if (FAILED(pGameInstance->Render_Font(TEXT("Font_136"), m_strNPC, _float2(g_iWinSizeX * 0.5f + 10.f, g_iWinSizeY * 0.5f + 40.f), XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.5f)))
		return E_FAIL;

	Safe_Release(pGameInstance);
	return S_OK;
}

HRESULT CInteractionFont::Town_ItemFont()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	m_strItem = TEXT("È¹µæ");

	if (FAILED(pGameInstance->Render_Font(TEXT("Font_136"), m_strItem, _float2(g_iWinSizeX * 0.5f + 10.f, g_iWinSizeY * 0.5f + 40.f), XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.5f)))
		return E_FAIL;

	Safe_Release(pGameInstance);
	return S_OK;
}

HRESULT CInteractionFont::GamePlay_NPCFont()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	m_strNPC = TEXT("´ëÈ­");

	if (FAILED(pGameInstance->Render_Font(TEXT("Font_136"), m_strNPC, _float2(g_iWinSizeX * 0.5f + 10.f, g_iWinSizeY * 0.5f + 40.f), XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.5f)))
		return E_FAIL;

	Safe_Release(pGameInstance);
}

HRESULT CInteractionFont::GamePlay_ItemFont()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	m_strItem = TEXT("È¹µæ");

	if (FAILED(pGameInstance->Render_Font(TEXT("Font_136"), m_strItem, _float2(g_iWinSizeX * 0.5f + 10.f, g_iWinSizeY * 0.5f + 40.f), XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.5f)))
		return E_FAIL;

	Safe_Release(pGameInstance);
}

HRESULT CInteractionFont::Add_Components()
{
	/* Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	return S_OK;
}

CInteractionFont * CInteractionFont::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CInteractionFont*	pInstance = new CInteractionFont(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Created : CInteractionFont");
	}

	return pInstance;
}

CGameObject * CInteractionFont::Clone(void * pArg)
{
	CInteractionFont*	pInstance = new CInteractionFont(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Cloned : CInteractionFont");
	}

	return pInstance;
}

void CInteractionFont::Free()
{
	__super::Free();

	m_pObject = { nullptr };

	Safe_Release(m_pRendererCom);
}
