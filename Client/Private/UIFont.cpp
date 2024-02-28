#include "stdafx.h"
#include "..\Public\UIFont.h"

#include "GameInstance.h"
#include "Player.h"
#include "TownPlayer.h"

CUIFont::CUIFont(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CUIFont::CUIFont(const CUIFont & rhs)
	: CGameObject(rhs)
{
}

HRESULT CUIFont::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIFont::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

void CUIFont::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	m_pObject = pGameInstance->Get_GameObject(g_CurLevel, L"Layer_Player");

	if (g_CurLevel == 3 || g_CurLevel == 5)
	{
		m_iPlayerShield = dynamic_cast<CTownPlayer*>(m_pObject)->Get_Shield();
		m_iPlayerHp = dynamic_cast<CTownPlayer*>(m_pObject)->Get_Hp();
	}
	else if (g_CurLevel == 4)
	{
		m_iPlayerShield = dynamic_cast<CPlayer*>(m_pObject)->Get_Shield();
		m_iPlayerHp = dynamic_cast<CPlayer*>(m_pObject)->Get_Hp();
	}

	if (g_AmountOfBullet <= 0)
		g_AmountOfBullet = 0;


	Safe_Release(pGameInstance);
}

void CUIFont::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CUIFont::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	//	wsprintf(&m_strFPS.front(), TEXT("FPS : %d"), m_iRenderCnt);
	//m_strLevel = TEXT("LV. %d"), g_PlayerLevel;
	m_strLevel = TEXT("0");
	m_strPlayerHp = TEXT("0");
	m_strGun = TEXT("0");
	m_strShield = TEXT("0");

	wsprintf(&m_strLevel.front(), TEXT("LV%d"), g_PlayerLevel);
	wsprintf(&m_strPlayerHp.front(), TEXT(" / %d"), 250);
	wsprintf(&m_strGun.front(), TEXT(" / %d"), g_AmountOfBullet);
	wsprintf(&m_strShield.front(), TEXT(" / %d"), 150);

	m_strPlayerName = TEXT("Gunzerker");

	if (FAILED(pGameInstance->Render_Font(TEXT("Dongle"), m_strLevel, _float2(g_iWinSizeX * 0.5f - 200.f, 650.f), XMVectorSet(1.f, 1.f, 1.f, 1.f), 1.2f)))
		return E_FAIL;

	if (FAILED(pGameInstance->Render_Font(TEXT("Dongle"), m_strPlayerName, _float2(g_iWinSizeX * 0.5f - 100.f, 650.f), XMVectorSet(1.f, 1.f, 1.f, 1.f), 1.2f)))
		return E_FAIL;

	if(FAILED(pGameInstance->Render_Font(TEXT("Font_136"), to_wstring(m_iPlayerHp), _float2(77.f, 660.f), XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.5f)))
		return E_FAIL;

	if (FAILED(pGameInstance->Render_Font(TEXT("Font_136"), m_strPlayerHp, _float2(110.f, 660.f), XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.5f)))
		return E_FAIL;

	if (FAILED(pGameInstance->Render_Font(TEXT("Font_136"), to_wstring(g_BulletCount), _float2(1120.f, 658.f), XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.5f)))
		return E_FAIL;

	if (FAILED(pGameInstance->Render_Font(TEXT("Font_136"), m_strGun, _float2(1140.f, 658.f), XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.5f)))
		return E_FAIL;

	if(FAILED(pGameInstance->Render_Font(TEXT("Font_136"), to_wstring(m_iPlayerShield), _float2(77.f, 605.f), XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.5f)))
		return E_FAIL;

	if (FAILED(pGameInstance->Render_Font(TEXT("Font_136"), m_strShield, _float2(120.f, 605.f), XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.5f)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CUIFont::Add_Components()
{
	/* Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	return S_OK;
}

CUIFont * CUIFont::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUIFont*	pInstance = new CUIFont(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Created : CUIFont");
	}

	return pInstance;
}

CGameObject * CUIFont::Clone(void * pArg)
{
	CUIFont*	pInstance = new CUIFont(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Cloned : CUIFont");
	}

	return pInstance;
}

void CUIFont::Free()
{
	__super::Free();

	m_pObject = { nullptr };

	Safe_Release(m_pRendererCom);
}
