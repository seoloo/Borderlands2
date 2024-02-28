#include "stdafx.h"
#include "TextFont.h"

#include "GameInstance.h"
#include "SoundMgr.h"

#include "Player.h"
#include "TownPlayer.h"

CTextFont::CTextFont(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CTextFont::CTextFont(const CTextFont & rhs)
	: CGameObject(rhs)
{
}

HRESULT CTextFont::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTextFont::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_iCounter = 0;

	return S_OK;
}

void CTextFont::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (g_bInteractionNPC)
	{
		if (KEYDOWN('G'))
		{
			++g_DialogueCount;
		}
	}
	else
	{
		Set_Dead();
	}

	if (g_CurLevel == 3)
	{
		if (g_DialogueCount == 1)
		{
			if (m_iCounter == 0)
			{
				CSoundMgr::Get_Instance()->PlaySoundW(L"But_So_Hard.mp3", 1.f);
				m_iCounter++;
			}

			m_strDialogue = TEXT("���־ ����. �츮�� ���͵鿡�� ������ �޾� ������ �Ҿ���Ⱦ�");
		}
		else if (g_DialogueCount == 3)
		{
			m_strDialogue = TEXT("���� �뺴���� ������ ���Ϳ��� ���ϰ� ������. ���� �뺴�� �ڳ׹ۿ� ����.");
		}
		else if (g_DialogueCount >= 3)
		{
			m_strDialogue = TEXT("�׷� ������ �̵��ϵ��� ����");

			/*if(KEYDOWN('G'))
				g_bTownEnd = true;*/

		}
	}
	else if (g_CurLevel == 4)
	{
		if (g_DialogueCount == 1)
		{
			if (m_iCounter == 0)
			{
				CSoundMgr::Get_Instance()->PlaySoundW(L"But_So_Hard.mp3", 1.f);
				m_iCounter++;
			}

			m_strDialogue = TEXT("���Ⱑ �����̶��. ���͵��� ������ ������ ���ϵ��� ������ ���Ƴ��ҳ�");
			g_bFirstMissionClear = true;
		}
		else if (g_DialogueCount >= 2)
		{
			m_strDialogue = TEXT("�ε� ���� ���� ���͵��� óġ���ְ�");
		}
		//else if (g_DialogueCount >= 3)
		//{
		//	m_strDialogue = TEXT("");

		//	/*if(KEYDOWN('G'))
		//		g_bTownEnd = true;*/

		//}
	}
	else if (g_CurLevel == 5)
	{
		if (g_DialogueCount == 1)
		{
			if (m_iCounter == 0)
			{
				CSoundMgr::Get_Instance()->PlaySoundW(L"But_So_Hard.mp3", 1.f);
				m_iCounter++;
			}

			m_strDialogue = TEXT("�ڳ� ���п� �츮 ������ ��ã�Ҿ�. ���� ����.");
			g_bFirstMissionClear = true;
		}
		else if (g_DialogueCount >= 2)
		{
			m_strDialogue = TEXT("���� �츮�� �ٽ� ������ ����� �ž�. �ڳ׵� �����ְڳ�?");
		}
	}

	Safe_Release(pGameInstance);
}

void CTextFont::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CTextFont::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (g_CurLevel == 3)
	{
		if (FAILED(pGameInstance->Render_Font(TEXT("Font_136"), m_strDialogue, _float2(g_iWinSizeX * 0.5f - 300.f, g_iWinSizeY * 0.5f + 150.f), XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.5f)))
			return E_FAIL;
	}
	else if (g_CurLevel == 4)
	{
		if (FAILED(pGameInstance->Render_Font(TEXT("Font_136"), m_strDialogue, _float2(g_iWinSizeX * 0.5f - 300.f, g_iWinSizeY * 0.5f + 150.f), XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.5f)))
			return E_FAIL;
	}
	else if (g_CurLevel == 5)
	{
		if (FAILED(pGameInstance->Render_Font(TEXT("Font_136"), m_strDialogue, _float2(g_iWinSizeX * 0.5f - 300.f, g_iWinSizeY * 0.5f + 150.f), XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.5f)))
			return E_FAIL;
	}
		
	Safe_Release(pGameInstance);

	return S_OK;
}


HRESULT CTextFont::Add_Components()
{
	/* Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	return S_OK;
}

CTextFont * CTextFont::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CTextFont*	pInstance = new CTextFont(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Created : CTextFont");
	}

	return pInstance;
}

CGameObject * CTextFont::Clone(void * pArg)
{
	CTextFont*	pInstance = new CTextFont(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Cloned : CTextFont");
	}

	return pInstance;
}

void CTextFont::Free()
{
	__super::Free();

	m_pObject = { nullptr };

	Safe_Release(m_pRendererCom);
}
