#pragma once

#include "Client_Defines.h"
#include "Base.h"
#include "SpiderAnt.h"

BEGIN(Engine)
class CTransform;
class CModel;
class CNavigation;
END

BEGIN(Client)
class CSpiderAnt;

class CSpiderAnt_State abstract
{
public:
	CSpiderAnt_State();
	virtual ~CSpiderAnt_State() = default;

	virtual HRESULT Enter(CSpiderAnt* spiderant) = 0;
	virtual void Tick(_float fTimeDelta) = 0;
	virtual void Exit(CSpiderAnt* spiderant) = 0;

protected:
	_float3 m_vPos = { 0.f,0.f,0.f };
	_float3 m_vPlayerPos = { 0.f,0.f,0.f };
	_vector m_Look = { XMVectorSet(0.f,0.f,0.f,0.f) };
	_float m_fTimer = { 0.f };
};
END
