#include "..\Public\Timer_Manager.h"
#include "Timer.h"

IMPLEMENT_SINGLETON(CTimer_Manager)

CTimer_Manager::CTimer_Manager()
{
}

HRESULT CTimer_Manager::Add_Timer(const wstring & strTimerTag)
{
	if (nullptr != Find_Timer(strTimerTag))
		return E_FAIL;

	m_Timers.emplace(strTimerTag, CTimer::Create());

	return S_OK;
}

_float CTimer_Manager::Compute_TimeDelta(const wstring & strTimerTag)
{
	CTimer*		pTimer = Find_Timer(strTimerTag);
	if (nullptr == pTimer)
		return 0.f;

	pTimer->Update();
	_float result = pTimer->Get_TimeDelta();
	if (result >= 1.f)
		result = 1 / 60.f;

	return result;
}

CTimer * CTimer_Manager::Find_Timer(const wstring & strTimerTag)
{
	auto	iter = m_Timers.find(strTimerTag);

	if (iter == m_Timers.end())
		return nullptr;

	return iter->second;	
}

void CTimer_Manager::Free()
{
	for (auto& Pair : m_Timers)
		Safe_Release(Pair.second);

	m_Timers.clear();
}
