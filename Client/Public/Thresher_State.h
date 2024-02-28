#pragma once

#include "Client_Defines.h"
#include "Base.h"
#include "Thresher.h"

BEGIN(Engine)
class CTransform;
class CModel;
class CNavigation;
END

BEGIN(Client)
class CThresher;

class CThresher_State abstract
{
public:
	CThresher_State();
	virtual ~CThresher_State() = default;

	virtual HRESULT Enter(CThresher* thresher) = 0;
	virtual void Tick(_float fTimeDelta) = 0;
	virtual void Exit(CThresher* thresher) = 0;

protected:
	_float m_fTimer = { 0.f };
};
END
