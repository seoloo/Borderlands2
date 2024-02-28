#pragma once

/* 1. 원형객체들을 보관한다. */
/* 2. 실제사용할 객체를 복제하는 작업을 한다. 복제하여 생성한 객체를 내가 나누고싶은 기준에따라 보관하낟 */
/* 3. 복제된 객체들의 틱과 레이트 틱을 호출한다. */
/* 4. 객체들의 렌더를 호출한다.(x) */

#include "Base.h"

BEGIN(Engine)

class CObject_Manager final : public CBase
{
	DECLARE_SINGLETON(CObject_Manager)
private:
	CObject_Manager();
	virtual ~CObject_Manager() = default;

public:
	class CComponent* Get_Component(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strComTag, _uint iIndex);

public:
	HRESULT Reserve_Manager(_uint iNumLevels);
	HRESULT Add_Prototype(const wstring& strPrototypeTag, class CGameObject* pPrototype);
	HRESULT Add_GameObject(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strPrototypeTag, void* pArg);
	CGameObject* Clone_GameObject(const wstring& strPrototypeTag, void* pArg);
	CGameObject* Get_GameObject(_uint level,const wstring& strLayerTag);

	//CGameObject* Ray_Object(_uint level, _float3* pWorldOut);

	void Tick(_float fTimeDelta);
	void Late_Tick(_float fTimeDelta);

	void Clear(_uint iLevelIndex);


private: /* 객체들의 원형을 저장한다. */
	map<wstring, class CGameObject*>		m_Prototypes;

private:
	_uint									m_iNumLevels = { 0 };
	map<wstring, class CLayer*>*			m_pLayers = { nullptr };
	typedef map<wstring, class CLayer*>		LAYERS;

private:
	class CGameObject* Find_Prototype(const wstring& strPrototypeTag);
	class CLayer* Find_Layer(_uint iLevelIndex, const wstring& strLayerTag);

public:
	virtual void Free() override;
};

END