#pragma once

#include "Base.h"

/* 내가 나누고 싶은 기준으로 객체들을 모아두기위해. */

BEGIN(Engine)

class CLayer final : public CBase
{
private:
	CLayer();
	virtual ~CLayer() = default;

public:
	class CComponent* Get_Component(const wstring& strComTag, _uint iIndex);

public:
	HRESULT Add_GameObject(class CGameObject* pGameObject);
	void Tick(_float fTimeDelta);
	void Late_Tick(_float fTimeDelta);

	class CGameObject* Get_GameObject() { return m_GameObjects.back(); }
	list<CGameObject*> Get_GameObjectList() { return m_GameObjects; };

private:
	list<class CGameObject*>			m_GameObjects;

public:
	static CLayer* Create();
	virtual void Free() override;
};

END