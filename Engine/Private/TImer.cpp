#include "..\Public\Timer.h"

CTimer::CTimer() 
	: m_fTimeDelta(0.f)
{
	ZeroMemory(&m_llCurrentTime, sizeof(LARGE_INTEGER));
	ZeroMemory(&m_llCpuTick, sizeof(LARGE_INTEGER));
	ZeroMemory(&m_llOldTime, sizeof(LARGE_INTEGER));
	ZeroMemory(&m_llOriginTime, sizeof(LARGE_INTEGER));
}

HRESULT CTimer::Initialize()
{
	QueryPerformanceCounter(&m_llCurrentTime);		
	QueryPerformanceCounter(&m_llOldTime);				
	QueryPerformanceCounter(&m_llOriginTime);				

	QueryPerformanceFrequency(&m_llCpuTick);		

	return S_OK;
}

void CTimer::Update(_bool bSlowdown, _bool bStop)
{
	QueryPerformanceCounter(&m_llCurrentTime);
		
	if (m_llCurrentTime.QuadPart - m_llOriginTime.QuadPart > m_llCpuTick.QuadPart)
	{
		QueryPerformanceFrequency(&m_llCpuTick);
		m_llOriginTime = m_llCurrentTime;
	}

	m_fTimeValue = bSlowdown ? 0.5f : 1.f;
	m_fTimeValue = bStop ? 0.f : 1.f;

	m_fTimeDelta = m_fTimeValue * float(m_llCurrentTime.QuadPart 
		- m_llOldTime.QuadPart) / m_llCpuTick.QuadPart;

	m_llOldTime = m_llCurrentTime;
}

CTimer * CTimer::Create()
{
	CTimer*	pInstance = new CTimer();

	if (FAILED(pInstance->Initialize()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Created : CTimer");
	}

	return pInstance;
}

void CTimer::Free()
{
}
