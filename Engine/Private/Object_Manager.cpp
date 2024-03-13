#include "..\Public\Object_Manager.h"
#include "Layer.h"
#include "GameObject.h"
#include "Picking.h"
#include "Model.h"
#include "Transform.h"

IMPLEMENT_SINGLETON(CObject_Manager)

CObject_Manager::CObject_Manager()
{
}

CComponent* CObject_Manager::Get_Component(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strComTag, _uint iIndex)
{
	CLayer* pLayer = Find_Layer(iLevelIndex, strLayerTag);

	if (nullptr == pLayer)
		return nullptr;

	return pLayer->Get_Component(strComTag, iIndex);
}

HRESULT CObject_Manager::Reserve_Manager(_uint iNumLevels)
{
	if (nullptr != m_pLayers)
		return E_FAIL;

	m_iNumLevels = iNumLevels;

	m_pLayers = new LAYERS[iNumLevels];

	return S_OK;
}

HRESULT CObject_Manager::Add_Prototype(const wstring& strPrototypeTag, CGameObject* pPrototype)
{
	if (nullptr != Find_Prototype(strPrototypeTag))
		return E_FAIL;

	m_Prototypes.emplace(strPrototypeTag, pPrototype);

	return S_OK;
}

HRESULT CObject_Manager::Add_GameObject(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strPrototypeTag, void* pArg)
{
	CGameObject* pPrototype = Find_Prototype(strPrototypeTag);

	if (nullptr == pPrototype)
		return E_FAIL;

	CGameObject* pGameObject = pPrototype->Clone(pArg);

	if (nullptr == pGameObject)
		return E_FAIL;

	CLayer* pLayer = Find_Layer(iLevelIndex, strLayerTag);

	if (nullptr == pLayer)
	{
		pLayer = CLayer::Create();
		if (nullptr == pLayer)
			return E_FAIL;

		pLayer->Add_GameObject(pGameObject);

		m_pLayers[iLevelIndex].emplace(strLayerTag, pLayer);
	}
	else
		pLayer->Add_GameObject(pGameObject);

	return S_OK;
}

CGameObject* CObject_Manager::Clone_GameObject(const wstring& strPrototypeTag, void* pArg)
{
	CGameObject* pPrototype = Find_Prototype(strPrototypeTag);

	if (nullptr == pPrototype)
		return nullptr;

	return pPrototype->Clone(pArg);
}

CGameObject* CObject_Manager::Get_GameObject(_uint level,const wstring& strLayerTag)
{
	if (level >= m_iNumLevels)
		return nullptr;

	return m_pLayers[level][strLayerTag]->Get_GameObject();
}

//CGameObject * CObject_Manager::Ray_Object(_uint level, _float3 * pWorldOut)
//{
//	CPicking* pPickingPtr = CPicking::Get_Instance();
//
//	_float3 vWorldOut = {};
//
//	for (auto& Pair : m_pLayers[level])
//	{
//		/*if (Pair.second->GetSize() == 0)
//			continue;*/
//
//		for (auto& pObject : Pair.second->Get_GameObjectList())
//		{
//			CTransform* pTempTransform = static_cast<CTransform*>(pObject->Find_Component(L"Com_Transform"));
//
//			if (pTempTransform == nullptr)
//				continue;
//
//			CModel* pModel = static_cast<CModel*>(pObject->Find_Component(L"Com_Model"));
//
//			if (pModel == nullptr)
//				continue;
//
//			_float3 vTempWolrdPos = {};
//			_float3 vTempWorldDir = {};
//
//			if (pModel->Intersect_Polygons(pTempTransform->Get_WorldMatrix(), pWorldOut))
//			{
//				return pObject;
//			}
//		}
//	}
//	return nullptr;
//}

void CObject_Manager::Tick(_float fTimeDelta)
{
	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& Pair : m_pLayers[i])
		{
			if (nullptr != Pair.second)
				Pair.second->Tick(fTimeDelta);
		}
	}
}

void CObject_Manager::Late_Tick(_float fTimeDelta)
{
	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& Pair : m_pLayers[i])
		{
			if (nullptr != Pair.second)
				Pair.second->Late_Tick(fTimeDelta);
		}
	}
}

void CObject_Manager::Clear(_uint iLevelIndex)
{
	if (iLevelIndex >= m_iNumLevels)
		return;

	for (auto& Pair : m_pLayers[iLevelIndex])
		Safe_Release(Pair.second);

	m_pLayers[iLevelIndex].clear();
}


CGameObject* CObject_Manager::Find_Prototype(const wstring& strPrototypeTag)
{
	auto	iter = m_Prototypes.find(strPrototypeTag);

	if (iter == m_Prototypes.end())
		return nullptr;

	return iter->second;
}

CLayer* CObject_Manager::Find_Layer(_uint iLevelIndex, const wstring& strLayerTag)
{
	if (iLevelIndex >= m_iNumLevels)
		return nullptr;

	auto	iter = m_pLayers[iLevelIndex].find(strLayerTag);

	if (iter == m_pLayers[iLevelIndex].end())
		return nullptr;

	return iter->second;
}

void CObject_Manager::Free()
{
	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& Pair : m_pLayers[i])
		{
			Safe_Release(Pair.second);
		}
		m_pLayers[i].clear();
	}
	Safe_Delete_Array(m_pLayers);

	if (!m_Prototypes.empty())
	{
		for (auto& Pair : m_Prototypes)
			Safe_Release(Pair.second);

		m_Prototypes.clear();
	}
}
