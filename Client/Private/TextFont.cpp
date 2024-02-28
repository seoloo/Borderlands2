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

			m_strDialogue = TEXT("와주어서 고맙네. 우리는 몬스터들에게 공격을 받아 마을을 잃어버렸어");
		}
		else if (g_DialogueCount == 3)
		{
			m_strDialogue = TEXT("많은 용병들이 왔지만 몬스터에게 당하고 말았지. 남은 용병은 자네밖에 없네.");
		}
		else if (g_DialogueCount >= 3)
		{
			m_strDialogue = TEXT("그럼 마을로 이동하도록 하지");

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

			m_strDialogue = TEXT("여기가 마을이라네. 몬스터들이 밖으로 나오지 못하도록 문으로 막아놓았네");
			g_bFirstMissionClear = true;
		}
		else if (g_DialogueCount >= 2)
		{
			m_strDialogue = TEXT("부디 문을 열고 몬스터들을 처치해주게");
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

			m_strDialogue = TEXT("자네 덕분에 우리 마을을 되찾았어. 정말 고맙네.");
			g_bFirstMissionClear = true;
		}
		else if (g_DialogueCount >= 2)
		{
			m_strDialogue = TEXT("이제 우리는 다시 마을을 재건할 거야. 자네도 도와주겠나?");
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
