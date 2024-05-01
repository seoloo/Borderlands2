#pragma once

#include "Base.h"

BEGIN(Engine)

class CTimer final : public CBase
{
private:
	CTimer();
	virtual ~CTimer() = default;

public:
	float			Get_TimeDelta(void) { 
		return m_fTimeDelta; }

public:
	HRESULT				Initialize();
	void				Update(_bool bSlowdown = false, _bool bStop = false);

private:
	LARGE_INTEGER		m_llCurrentTime;
	LARGE_INTEGER		m_llOldTime;
	LARGE_INTEGER		m_llOriginTime;
	LARGE_INTEGER		m_llCpuTick;

	float				m_fTimeDelta;
	float				m_fTimeValue = 1.f;
public:
	static CTimer* Create();
	virtual void Free() override;
};

END