#include "Key_Mgr.h"

IMPLEMENT_SINGLETON(CKey_Mgr)


CKey_Mgr::CKey_Mgr()
	: m_bKeyState()
{

}

bool CKey_Mgr::Key_Down(int _iKey)
{
	//cout << cout.boolalpha;
	//cout << m_bKeyState['B'] << "\n";

	if (!m_bKeyState[_iKey] && (GetAsyncKeyState(_iKey) & 0x8000))
	{
		m_bKeyState[_iKey] = !m_bKeyState[_iKey];
		return true;
	}

	for (int i = 0; i < VK_MAX; ++i)
	{
		if (m_bKeyState[i] && !(GetAsyncKeyState(i) & 0x8000))
			m_bKeyState[i] = !m_bKeyState[i];
	}

	return false;
}

bool CKey_Mgr::Key_Up(int _iKey)
{
	return false;
}

bool CKey_Mgr::Key_Hold(int _iKey)
{
	return false;
}

void CKey_Mgr::Update()
{
	if (GetFocus())
	{
		for (int i = 0; i < VK_MAX; ++i)
		{
			if (GetAsyncKeyState(i) & 0x8000)
			{
				if (!m_arrKeyState[i].bPrevDown && KEY_STATE::NONE == m_arrKeyState[i].eState)
				{
					//�������� ������ �ʾ����� ���� ������
					m_arrKeyState[i].bPrevDown = true;
					m_arrKeyState[i].eState = KEY_STATE::DOWN;
				}
				else
				{
					//�������� ���Ⱦ���, ���絵 ������
					m_arrKeyState[i].eState = KEY_STATE::HOLD;
				}
			}
			else
			{
				m_arrKeyState[i].bPrevDown = false;

				if (KEY_STATE::NONE == m_arrKeyState[i].eState || KEY_STATE::UP == m_arrKeyState[i].eState)
				{
					// �������� �ȴ��Ȱų�, ������ �� ������ ��
					m_arrKeyState[i].eState = KEY_STATE::NONE;
				}
				else
				{
					// ������ �ѹ� �����ų�, �� ���� ������ ��
					m_arrKeyState[i].eState = KEY_STATE::UP;
				}
			}
		}
	}
	
}

KEY_STATE CKey_Mgr::Get_Key_State(int _iKey)
{
	return m_arrKeyState[_iKey].eState;
}

void CKey_Mgr::Free()
{
}
