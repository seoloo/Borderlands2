#include "..\Public\Level_ToolLoading.h"
#include "Tool_Loader.h"

#pragma region ENGINE_HEADER
#include "GameInstance.h"
#pragma endregion

#include "Level_Tool.h"

CLevel_ToolLoading::CLevel_ToolLoading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_ToolLoading::Initialize(LEVEL eNextLevel)
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;

	m_pLoader = CTool_Loader::Create(m_pDevice, m_pContext, eNextLevel);
	if (nullptr == m_pLoader)
		return E_FAIL;

	m_eNextLevel = eNextLevel;

	return S_OK;
}

void CLevel_ToolLoading::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CLevel_ToolLoading::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);


	if (true != m_pLoader->is_Finished())
		return;

	CLevel*		pLevel = { nullptr };

	switch (m_eNextLevel)
	{
	case LEVEL_TOOL:
		pLevel = CLevel_Tool::Create(m_pDevice, m_pContext);
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

	SetWindowText(g_hWnd, m_pLoader->Get_LoadingText());
}

HRESULT CLevel_ToolLoading::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

CLevel_ToolLoading* CLevel_ToolLoading::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevel)
{
	CLevel_ToolLoading*	pInstance = new CLevel_ToolLoading(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevel)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Created : CLeveL_ToolLoading");
	}

	return pInstance;
}

void CLevel_ToolLoading::Free()
{
	__super::Free();

	Safe_Release(m_pLoader);
}
