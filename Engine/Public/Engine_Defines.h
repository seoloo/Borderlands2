#pragma once

#pragma warning (disable : 4005)
#pragma warning (disable : 4251)

#include <d3d11.h>

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <random>

#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <DirectXCollision.h>

#include "Effects11\d3dx11effect.h"
#include "DirectXTK\DDSTextureLoader.h"
#include "DirectXTK\WICTextureLoader.h"
#include "DirectXTK\ScreenGrab.h"
#include "DirectXTK\PrimitiveBatch.h"
#include "DirectXTK\VertexTypes.h"
#include "DirectXTK\Effects.h"
#include "DirectXTK\SpriteBatch.h"
#include "DirectXTK\SpriteFont.h"

#ifdef min

#undef min
#undef max

#endif

#include "assimp\Importer.hpp"
#include "assimp\scene.h"
#include "assimp\postprocess.h"

#include <string>
#include <queue>
#include <vector>
#include <list>
#include <map>
#include <algorithm>
#include <io.h>
#include <stdlib.h>
#include <crtdbg.h>
#include <iostream>

using namespace DirectX;

#include "Engine_Function.h"
#include "Engine_Macro.h"
#include "Engine_Typedef.h"
#include "Engine_Struct.h"


#include "fmod.h"
#include "fmod.hpp"



#ifdef _DEBUG

#define _CRTDBG_MAP_ALLOC

#ifndef DBG_NEW 

#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 
#define new DBG_NEW 

#endif

#endif // _DEBUG

#define VK_MAX 0xff

using namespace std;
using namespace Engine;

