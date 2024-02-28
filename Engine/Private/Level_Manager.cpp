#include "..\Public\Level_Manager.h"
#include "GameInstance.h"
#include "Level.h"


IMPLEMENT_SINGLETON(CLevel_Manager)

CLevel_Manager::CLevel_Manager()
{

}

HRESULT CLevel_Manager::Open_Level(_uint iLevelIndex, CLevel * pLevel)
{
	if (0 != Safe_Release(m_pCurrentLevel))
		return E_FAIL;

	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	pGameInstance->Clear(m_iLevelIndex);

	Safe_Release(pGameInstance);

	/* 이전레벨용 리소스 들을 정리하낟. */

	m_pCurrentLevel = pLevel;

	m_iLevelIndex = iLevelIndex;

	return S_OK;
}

void CLevel_Manager::Tick(_float fTimeDelta)
{
	if (nullptr == m_pCurrentLevel)
		return;

	m_pCurrentLevel->Tick(fTimeDelta);

}

void CLevel_Manager::Late_Tick(_float fTimeDelta)
{
	if (nullptr == m_pCurrentLevel)
		return;

	m_pCurrentLevel->Late_Tick(fTimeDelta);
}

void CLevel_Manager::Free()
{
	Safe_Release(m_pCurrentLevel);
}
