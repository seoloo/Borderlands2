#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CInput_Device : public CBase
{
	DECLARE_SINGLETON(CInput_Device)

public:
	enum MOUSEKEYSTATE { DIMK_LBUTTON, DIMK_RBUTTON, DIMK_WHEEL, DIMK_END };
	enum MOUSEMOVESTATE { DIMM_X, DIMM_Y, DIMM_WHEEL, DIMM_END };

private:
	CInput_Device();
	virtual ~CInput_Device() = default;

public:
	_byte Get_DIKeyState(_ubyte ubyKeyID) 
	{
		return m_byKeyState[ubyKeyID];
	}
	_byte Get_DIMouseState(MOUSEKEYSTATE eMouseID)
	{
		return m_MouseState.rgbButtons[eMouseID];
	}

	_long Get_DIMouseMove(MOUSEMOVESTATE eMouseMoveID)
	{
		return *(((_long*)&m_MouseState) + eMouseMoveID);
	}



public:
	HRESULT			Ready_DInput(HINSTANCE hInst, HWND hWnd);
	void			Update_DInput(void);

private:
	LPDIRECTINPUT8				m_pInputSDK = { nullptr };

	LPDIRECTINPUTDEVICE8		m_pKeyBoard = { nullptr };
	LPDIRECTINPUTDEVICE8		m_pMouse = { nullptr };
	LPDIRECTINPUTDEVICE8		m_pJoypad = { nullptr };

private:
	/* 키보드의 모든 키에 대한 입력 상태를 저장한다. */
	_byte					m_byKeyState[256];

	/* 마우스의 모든 키에 대한 입력 상태를 저장한다. */
	DIMOUSESTATE			m_MouseState;

public:
	virtual void		Free(void);

};

END