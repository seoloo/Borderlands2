#include "stdafx.h"
#include "MissionFont.h"

#include "GameInstance.h"
#include "Player.h"
#include "TownPlayer.h"

bool g_bFirstMissionClear;
bool g_bSceondMissionClear;
bool g_bThirdMissionClear;

CMissionFont::CMissionFont(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CMissionFont::CMissionFont(const CMissionFont & rhs)
	: CGameObject(rhs)
{
}

HRESULT CMissionFont::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMissionFont::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_ClearColor = { 1.f,1.f,0.f,1.f };
	m_Color = { 1.f,1.f,1.f,1.f };

	return S_OK;
}

void CMissionFont::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (g_bFirstMissionClear)
	{
		m_Color = m_ClearColor;
		g_bMissionClear = true;
		m_fTimer += fTimeDelta;
		if (m_fTimer > 3.f)
		{
			if (g_CurLevel != 5)
			{
				g_bMissionClear = false;
				m_Color = { 1.f,1.f,1.f,1.f };
				m_bFirstMissionClear = true;
				g_bFirstMissionClear = false;
				m_fTimer = 0.f;
			}
		}
	}
	if (g_bSceondMissionClear)
	{
		m_Color = m_ClearColor;
		g_bMissionClear = true;
		m_fTimer += fTimeDelta;
		if (m_fTimer > 3.f)
		{
			g_bMissionClear = false;
			m_Color = { 1.f,1.f,1.f,1.f };
			m_bSecondMissionClear = true;
			g_bSceondMissionClear = false;
			m_fTimer = 0.f;
		}
	}

	if (g_bThirdMissionClear)
	{
		m_bSecondMissionClear = false;
		m_Color = m_ClearColor;
		g_bMissionClear = true;
		m_fTimer += fTimeDelta;
		if (m_fTimer > 3.f)
		{
			g_bMissionClear = false;
			m_Color = { 1.f,1.f,1.f,1.f };
			g_bThirdMissionClear = false;
			m_bThirdMissionClear = true;
			m_fTimer = 0.f;
		}
	}

	Safe_Release(pGameInstance);
}

void CMissionFont::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CMissionFont::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (g_CurLevel == 3)
	{
		if (FAILED(Town_Mission()))
			return E_FAIL;
	}
	else if (g_CurLevel == 4)
	{
		if (FAILED(GamePlay_Mission()))
			return E_FAIL;
	}
	else if (g_CurLevel == 5)
	{
		if (FAILED(Ending_Mission()))
			return E_FAIL;
	}

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CMissionFont::Town_Mission()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);
	if (!g_bFirstMissionClear && !g_bSceondMissionClear && !g_bThirdMissionClear)
	{
		//wsprintf(&m_strGuide.front(), TEXT("의뢰 받기"));

		m_strGuide = TEXT("의뢰");
		if (FAILED(pGameInstance->Render_Font(TEXT("Font_136"), m_strGuide, _float2(g_iWinSizeX - 80, 200.f), XMVectorSet(0.5f, 1.f, 1.f, 1.f), 0.5f)))
			return E_FAIL;

		m_strMission = TEXT("Sir Hammerlock 만나기");
		if (FAILED(pGameInstance->Render_Font(TEXT("Font_136"), m_strMission, _float2(g_iWinSizeX - 300, 240.f), XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.5f)))
			return E_FAIL;

	}
	
	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CMissionFont::GamePlay_Mission()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);
	if (!m_bFirstMissionClear && !m_bSecondMissionClear && !m_bThirdMissionClear)
	{
		m_strGuide = TEXT("빼앗긴 마을");
		if (FAILED(pGameInstance->Render_Font(TEXT("Font_136"), m_strGuide, _float2(g_iWinSizeX - 140, 200.f), XMVectorSet(0.5f, 1.f, 1.f, 1.f), 0.5f)))
			return E_FAIL;

		m_strMission = TEXT("Sir Hammerlock과 대화하기");
		if (FAILED(pGameInstance->Render_Font(TEXT("Font_136"), m_strMission, _float2(g_iWinSizeX - 320, 240.f), XMVectorSet(m_Color.x, m_Color.y, m_Color.z, m_Color.w), 0.5f)))
			return E_FAIL;
	}

	if (g_bFirstMissionClear && !g_bSceondMissionClear && !g_bThirdMissionClear && !m_bFirstMissionClear&& !m_bSecondMissionClear)
	{
		m_strGuide = TEXT("빼앗긴 마을");
		if (FAILED(pGameInstance->Render_Font(TEXT("Font_136"), m_strGuide, _float2(g_iWinSizeX - 140, 200.f), XMVectorSet(0.5f, 1.f, 1.f, 1.f), 0.5f)))
			return E_FAIL;

		m_strMission = TEXT("Sir Hammerlock과 대화하기");
		if (FAILED(pGameInstance->Render_Font(TEXT("Font_136"), m_strMission, _float2(g_iWinSizeX - 320, 240.f), XMVectorSet(m_Color.x, m_Color.y, m_Color.z, m_Color.w), 0.5f)))
			return E_FAIL;
	}

	if (!g_bFirstMissionClear && !g_bSceondMissionClear && !g_bThirdMissionClear && m_bFirstMissionClear && !m_bSecondMissionClear && !m_bThirdMissionClear)
	{
		m_strGuide = TEXT("몬스터 처치");
		if (FAILED(pGameInstance->Render_Font(TEXT("Font_136"), m_strGuide, _float2(g_iWinSizeX - 170, 200.f), XMVectorSet(0.5f, 1.f, 1.f, 1.f), 0.5f)))
			return E_FAIL;

		m_strMission = TEXT("모든 몬스터 처치하기");
		if (FAILED(pGameInstance->Render_Font(TEXT("Font_136"), m_strMission, _float2(g_iWinSizeX - 250, 240.f), XMVectorSet(m_Color.x, m_Color.y, m_Color.z, m_Color.w), 0.5f)))
			return E_FAIL;
	}
	if (g_bSceondMissionClear && !m_bSecondMissionClear)
	{
		m_strGuide = TEXT("몬스터 처치");
		m_strMission = TEXT("모든 몬스터 처치하기");

		if (FAILED(pGameInstance->Render_Font(TEXT("Dongle"), TEXT("몬스터 처치"), 
			_float2(g_iWinSizeX - 170, 200.f), XMVectorSet(0.5f, 1.f, 1.f, 1.f), 0.5f)))
			return E_FAIL;

		if (FAILED(pGameInstance->Render_Font(TEXT("Dongle"), TEXT("모든 몬스터 처치하기"),
			_float2(g_iWinSizeX - 250, 240.f), XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.5f)))
			return E_FAIL;
	}

	if (!g_bSceondMissionClear && m_bSecondMissionClear)
	{
		m_strGuide = TEXT("생존");
		if (FAILED(pGameInstance->Render_Font(TEXT("Font_136"), m_strGuide, _float2(g_iWinSizeX - 110, 200.f), XMVectorSet(0.5f, 1.f, 1.f, 1.f), 0.5f)))
			return E_FAIL;

		m_strMission = TEXT("살아남기");
		if (FAILED(pGameInstance->Render_Font(TEXT("Font_136"), m_strMission, _float2(g_iWinSizeX - 150, 240.f), XMVectorSet(m_Color.x, m_Color.y, m_Color.z, m_Color.w), 0.5f)))
			return E_FAIL;
	}
	if (g_bThirdMissionClear && !m_bThirdMissionClear)
	{
		m_strGuide = TEXT("생존");
		if (FAILED(pGameInstance->Render_Font(TEXT("Font_136"), m_strGuide, _float2(g_iWinSizeX - 110, 200.f), XMVectorSet(0.5f, 1.f, 1.f, 1.f), 0.5f)))
			return E_FAIL;

		m_strMission = TEXT("살아남기");
		if (FAILED(pGameInstance->Render_Font(TEXT("Font_136"), m_strMission, _float2(g_iWinSizeX - 150, 240.f), XMVectorSet(m_Color.x, m_Color.y, m_Color.z, m_Color.w), 0.5f)))
			return E_FAIL;
	}

	if (m_bThirdMissionClear)
	{
		m_strGuide = TEXT("되찾은 마을");
		if (FAILED(pGameInstance->Render_Font(TEXT("Font_136"), m_strGuide, _float2(g_iWinSizeX - 170, 200.f), XMVectorSet(0.5f, 1.f, 1.f, 1.f), 0.5f)))
			return E_FAIL;

		m_strMission = TEXT("임시거처로 돌아가기");
		if (FAILED(pGameInstance->Render_Font(TEXT("Font_136"), m_strMission, _float2(g_iWinSizeX - 250, 240.f), XMVectorSet(m_Color.x, m_Color.y, m_Color.z, m_Color.w), 0.5f)))
			return E_FAIL;
	}

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CMissionFont::Ending_Mission()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (!m_bFirstMissionClear && !g_bFirstMissionClear)
	{
		m_strGuide = TEXT("의뢰 성공");
		if (FAILED(pGameInstance->Render_Font(TEXT("Font_136"), m_strGuide, _float2(g_iWinSizeX - 160, 200.f), XMVectorSet(0.5f, 1.f, 1.f, 1.f), 0.5f)))
			return E_FAIL;

		m_strMission = TEXT("Sir Hammerlock과 대화하기");
		if (FAILED(pGameInstance->Render_Font(TEXT("Font_136"), m_strMission, _float2(g_iWinSizeX - 330, 240.f), XMVectorSet(m_Color.x, m_Color.y, m_Color.z, m_Color.w), 0.5f)))
			return E_FAIL;
	}

	if (g_bFirstMissionClear && !m_bFirstMissionClear)
	{
		m_strGuide = TEXT("의뢰 성공");
		if (FAILED(pGameInstance->Render_Font(TEXT("Font_136"), m_strGuide, _float2(g_iWinSizeX - 160, 200.f), XMVectorSet(0.5f, 1.f, 1.f, 1.f), 0.5f)))
			return E_FAIL;

		m_strMission = TEXT("Sir Hammerlock과 대화하기");
		if (FAILED(pGameInstance->Render_Font(TEXT("Font_136"), m_strMission, _float2(g_iWinSizeX - 330, 240.f), XMVectorSet(m_Color.x, m_Color.y, m_Color.z, m_Color.w), 0.5f)))
			return E_FAIL;
	}

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CMissionFont::Add_Components()
{
	/* Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	return S_OK;
}

CMissionFont * CMissionFont::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMissionFont*	pInstance = new CMissionFont(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Created : CMissionFont");
	}

	return pInstance;
}

CGameObject * CMissionFont::Clone(void * pArg)
{
	CMissionFont*	pInstance = new CMissionFont(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Cloned : CMissionFont");
	}

	return pInstance;
}

void CMissionFont::Free()
{
	__super::Free();

	m_pObject = { nullptr };

	Safe_Release(m_pRendererCom);
}
