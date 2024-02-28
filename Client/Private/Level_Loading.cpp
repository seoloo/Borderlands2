#include "stdafx.h"
#include "..\Public\Level_Loading.h"

#pragma region ENGINE_HEADER
#include "GameInstance.h"
#include "SoundMgr.h"
#pragma endregion

#pragma region CLIENT_HEADER
#include "Loader.h"
#include "Level_Logo.h"
#include "Level_GamePlay.h"
#include "Level_Town.h"
#include "Level_Ending.h"
#pragma endregion

CLevel_Loading::CLevel_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Loading::Initialize(LEVEL eNextLevel)
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;

	/* 원형을 복제하여 사본을 만든다. */
	if (FAILED(Ready_Layer_LoadingUI(TEXT("Layer_LoadingUI"))))
		return E_FAIL;

	m_pLoader = CLoader::Create(m_pDevice, m_pContext, eNextLevel);
	if (nullptr == m_pLoader)
		return E_FAIL;

	CSoundMgr::Get_Instance()->StopAll();

	/*if (g_CurLevel == 2)
		CSoundMgr::Get_Instance()->PlayBGM(L"2-1.wav", 1.f);
	else if(g_CurLevel == 3)
		CSoundMgr::Get_Instance()->PlayBGM(L"Outwash.mp3", 1.f);
	else if (g_CurLevel == 4)
		CSoundMgr::Get_Instance()->PlayBGM(L"Lyrics.mp3", 1.f);*/

	m_eNextLevel = eNextLevel;

	return S_OK;
}

void CLevel_Loading::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CLevel_Loading::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (GetKeyState(VK_SPACE) & 0x8000)
	{
		if (true != m_pLoader->is_Finished())
			return;

		CLevel*		pLevel = { nullptr };

		switch (m_eNextLevel)
		{
		case LEVEL_LOGO:
			g_CurLevel = m_eNextLevel;
			pLevel = CLevel_Logo::Create(m_pDevice, m_pContext);
			break;
		case LEVEL_TOWN:
			g_CurLevel = m_eNextLevel;
			pLevel = CLevel_Town::Create(m_pDevice, m_pContext);
			break;
		case LEVEL_GAMEPLAY:
			g_CurLevel = m_eNextLevel;
			pLevel = CLevel_GamePlay::Create(m_pDevice, m_pContext);
			break;
		case LEVEL_ENDING:
			g_CurLevel = m_eNextLevel;
			pLevel = CLevel_Ending::Create(m_pDevice, m_pContext);
			break;
		}

		if (nullptr == pLevel)
			return;

		CGameInstance* pGameInstance = CGameInstance::Get_Instance();
		Safe_AddRef(pGameInstance);

		if (FAILED(pGameInstance->Open_Level(m_eNextLevel, pLevel)))
		{
			Safe_Release(pGameInstance);
			return;
		}

		Safe_Release(pGameInstance);
	}

	SetWindowText(g_hWnd, m_pLoader->Get_LoadingText());
}

HRESULT CLevel_Loading::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Loading::Ready_Layer_LoadingUI(const wstring& strLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	/* For.LoadingUI */
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOADING, strLayerTag,
		TEXT("Prototype_GameObject_LoadingUI"))))
		return E_FAIL;
	
	Safe_Release(pGameInstance);

	return S_OK;
}

CLevel_Loading * CLevel_Loading::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevel)
{
	CLevel_Loading*	pInstance = new CLevel_Loading(pDevice, pContext);	

	if (FAILED(pInstance->Initialize(eNextLevel)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Created : CLevel_Loading");
	}

	return pInstance;
}

void CLevel_Loading::Free()
{
	__super::Free();

	Safe_Release(m_pLoader);
}
