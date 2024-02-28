#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CKey_Mgr : public CBase
{
	DECLARE_SINGLETON(CKey_Mgr)
public :
	

	struct tKeyInfo
	{
		KEY_STATE	eState;
		bool		bPrevDown; // ������ ���ȴ��� Ȯ�ο�

		tKeyInfo() : eState(KEY_STATE::NONE), bPrevDown(false) {} // �⺻ ������
	};

private:
	CKey_Mgr();
	virtual ~CKey_Mgr()= default;


private:
	bool		m_bKeyState[VK_MAX];
	tKeyInfo	m_arrKeyState[VK_MAX];

public:
	void		Update();
	KEY_STATE	Get_Key_State(int _iKey);
	virtual void Free();
	
public:
	bool Key_Down(int _iKey);
	bool Key_Up(int _iKey);
	bool Key_Hold(int _iKey);
};

END