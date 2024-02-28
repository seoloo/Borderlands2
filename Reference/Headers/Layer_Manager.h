#pragma once

#include "Base.h"

BEGIN(Engine)

class CLayer_Manager final : public CBase
{
	DECLARE_SINGLETON(CLayer_Manager)
private:
	CLayer_Manager();
	virtual ~CLayer_Manager() = default;

public:
	//_int Get_ObjectHp(_uint iLevelIndex, const wstring& strLayerTag);
	//void Set_ObjectHp(_uint iLevelIndex, const wstring& strLayerTag, _int iHp);

public:
	void Tick(_float fTimeDelta);
	void Late_Tick(_float fTimeDelta);

	void Clear(_uint iLevelIndex);
private:
	_uint						m_iNumLevels = { 0 };
	map<wstring, class CLayer*>* m_pLayers = { nullptr };
	typedef map<wstring, class CLayer*>		LAYERS;
	_int	m_iHp = { 0 };
private:
	class CLayer* Find_Layer(_uint iLevelIndex, const wstring& strLayerTag);

public:
	virtual void Free() override;
};

END