#pragma once

#include <d3d11.h>
#include <string>
#include <tchar.h>
#include <process.h>
#include <iostream>
#include <filesystem>
#include <fstream>
#include "Engine_Defines.h"

namespace Tool
{
	enum LEVEL { LEVEL_STATIC, LEVEL_LOADING, LEVEL_TOOL, LEVEL_END };

	enum TOOL_TYPE {
		TOOLS_OBJECT, TOOLS_EFFECT
	};

	enum OBJECT_TYPE {
		TYPE_TERRAIN, TYPE_OBJECT
	};

	const unsigned int g_iWinSizeX = 1280;
	const unsigned int g_iWinSizeY = 720;
}

typedef struct tagObjInfo
{
	int iBindingCnt;
	wstring strPath;
	wstring strProtoTag;
	wstring strLayerTag;
}OBJINFO;

extern XMVECTOR g_vObjPos;
extern XMFLOAT3 g_vObjSize;
extern float g_fRadian;

extern HWND g_hWnd;
extern HINSTANCE	g_hInst;

using namespace Tool;
using namespace std;