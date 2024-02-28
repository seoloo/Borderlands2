#include "..\Public\Layer.h"
#include "GameObject.h"

CLayer::CLayer()
{
}

CComponent * CLayer::Get_Component(const wstring & strComTag, _uint iIndex)
{
	auto	iter = m_GameObjects.begin();

	for (size_t i = 0; i < iIndex; i++)	
		++iter;

	return (*iter)->Find_Component(strComTag);	
}

HRESULT CLayer::Add_GameObject(CGameObject * pGameObject)
{
	if (nullptr == pGameObject)
		return E_FAIL;

	m_GameObjects.push_back(pGameObject);

	return S_OK;
}

void CLayer::Tick(_float fTimeDelta)
{
	for (auto& pGameObject : m_GameObjects)
	{
		if (nullptr != pGameObject)
			pGameObject->Tick(fTimeDelta);
	}
}

void CLayer::Late_Tick(_float fTimeDelta)
{
	for (auto it = m_GameObjects.begin(); it != m_GameObjects.end(); )
	{
		if (*it != nullptr)
		{
			(*it)->Late_Tick(fTimeDelta);
			if ((*it)->Get_Dead())
			{
				Safe_Release(*it);
				it = m_GameObjects.erase(it); // 제거 후 반복자 업데이트
				continue;
			}
		}
		++it; // 다음 반복자로 이동
	}
}


CLayer * CLayer::Create()
{
	return new CLayer;
}

void CLayer::Free()
{
	for (auto& pGameObject : m_GameObjects)
		Safe_Release(pGameObject);
	m_GameObjects.clear();
}


