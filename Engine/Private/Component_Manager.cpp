#include "..\Public\Component_Manager.h"

IMPLEMENT_SINGLETON(CComponent_Manager)

CComponent_Manager::CComponent_Manager()
{
}

HRESULT CComponent_Manager::Reserve_Manager(_uint iNumLevels)
{
	if (nullptr != m_pPrototypes)
		return E_FAIL;

	m_iNumLevels = iNumLevels;

	m_pPrototypes = new PROTOTYPE[iNumLevels];

	return S_OK;
}

HRESULT CComponent_Manager::Add_Prototype(_uint iLevelIndex, const wstring& strPrototypeTag, CComponent* pPrototype)
{
	if (nullptr != Find_Prototype(iLevelIndex, strPrototypeTag))
		return E_FAIL;

	m_pPrototypes[iLevelIndex].emplace(strPrototypeTag, pPrototype);

	return S_OK;
}

CComponent* CComponent_Manager::Clone_Component(_uint iLevelIndex, const wstring& strPrototypeTag, void* pArg)
{
	CComponent* pPrototype = Find_Prototype(iLevelIndex, strPrototypeTag);
	if (nullptr == pPrototype)
		return nullptr;

	return pPrototype->Clone(pArg);
}

CComponent* CComponent_Manager::Find_Prototype(_uint iLevelIndex, const wstring& strPrototypeTag)
{
	auto	iter = m_pPrototypes[iLevelIndex].find(strPrototypeTag);

	if (iter == m_pPrototypes[iLevelIndex].end())
		return nullptr;

	return iter->second;
}

void CComponent_Manager::Free()
{
	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& Pair : m_pPrototypes[i])
			Safe_Release(Pair.second);

		m_pPrototypes[i].clear();
	}

	Safe_Delete_Array(m_pPrototypes);

}
