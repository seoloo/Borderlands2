#pragma once

#include "Base.h"

/* 현재 게임내에 보여줘야할 레벨을 저장한다. */
/* 갱신과 렏너 */
/* 레벨교체 */
/* 이전레벨용 리소스 들을 정리하낟. */

BEGIN(Engine)

class CLevel_Manager final : public CBase
{
	DECLARE_SINGLETON(CLevel_Manager)
private:
	CLevel_Manager();
	virtual ~CLevel_Manager() = default;

public:
	HRESULT Open_Level(_uint iLevelIndex, class CLevel* pLevel);
	void Tick(_float fTimeDelta);
	void Late_Tick(_float fTimeDelta);	

private: /* 현재 화면에 보여줘야할 레벨이다. */
	class CLevel*			m_pCurrentLevel = { nullptr };
	_uint					m_iLevelIndex = { 0 };

public:
	virtual void Free() override;
};

END