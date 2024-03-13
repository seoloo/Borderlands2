#include "..\Public\Timer.h"

CTimer::CTimer() 
	: m_fTimeDelta(0.f)
{
	ZeroMemory(&m_CurrentTime, sizeof(LARGE_INTEGER));
	ZeroMemory(&m_CpuTick, sizeof(LARGE_INTEGER));
	ZeroMemory(&m_OldTime, sizeof(LARGE_INTEGER));
	ZeroMemory(&m_OriginTime, sizeof(LARGE_INTEGER));
}

HRESULT CTimer::Initialize()
{
	QueryPerformanceCounter(&m_CurrentTime);		
	QueryPerformanceCounter(&m_OldTime);				
	QueryPerformanceCounter(&m_OriginTime);				

	QueryPerformanceFrequency(&m_CpuTick);		

	return S_OK;
}

void CTimer::Update(_bool bSlowdown, _bool bStop)
{
	QueryPerformanceCounter(&m_CurrentTime);
		
	if (m_CurrentTime.QuadPart - m_OriginTime.QuadPart > m_CpuTick.QuadPart)
	{
		QueryPerformanceFrequency(&m_CpuTick);
		m_OriginTime = m_CurrentTime;
	}

	m_fTimeValue = bSlowdown ? 0.5f : 1.f;
	m_fTimeValue = bStop ? 0.f : 1.f;

	m_fTimeDelta = m_fTimeValue * float(m_CurrentTime.QuadPart 
		- m_OldTime.QuadPart) / m_CpuTick.QuadPart;

	m_OldTime = m_CurrentTime;
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
