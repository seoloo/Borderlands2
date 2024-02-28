#pragma once

#include <d3d11.h>
#include <string>
#include <tchar.h>
#include <process.h>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <commdlg.h>
#include <Windows.h>
#include <sstream>
#include "Engine_Defines.h"

namespace EffectTool
{
	enum LEVEL { LEVEL_STATIC, LEVEL_END };
	const unsigned int g_iWinSizeX = 1280;
	const unsigned int g_iWinSizeY = 720;
}

extern HWND g_hWnd;
extern HINSTANCE	g_hInst;

using namespace EffectTool;
using namespace std;