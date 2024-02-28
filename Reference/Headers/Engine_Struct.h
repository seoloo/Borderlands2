#pragma once

namespace Engine
{
	typedef struct tagGraphicDesc
	{
		enum WINMODE { WINMODE_FULL, WINMODE_WIN, WINMODE_END };
		HWND			hWnd;
		unsigned int	iSizeX, iSizeY;
		WINMODE			eWinMode;

	}GRAPHICDESC;

	typedef struct tagKeyFrame
	{
		XMFLOAT4		vScale;
		XMFLOAT4		vRotation;
		XMFLOAT4		vTranslation;
		float			fTime;
	}KEYFRAME;

	typedef struct tagLightDesc
	{
		enum TYPE { TYPE_DIRECTIONAL, TYPE_POINT, TYPE_END };

		TYPE			eType;
		bool			bTurnOff;
		XMFLOAT4		vDirection;
		XMFLOAT4		vPosition;
		float			fRange;

		XMFLOAT4		vDiffuse;
		XMFLOAT4		vAmbient;
		XMFLOAT4		vSpecular;

	}LIGHTDESC;

	typedef struct tagModelMaterial
	{
		class CTexture* pMaterialTexture[AI_TEXTURE_TYPE_MAX];
	}MODEL_MATERIAL;

	typedef struct ENGINE_DLL tagVertexPosition
	{
		XMFLOAT3		vPosition;

		const static unsigned int					iNumElements = { 1 };
		const static D3D11_INPUT_ELEMENT_DESC		Elements[iNumElements];
	}VTXPOS;

	typedef struct ENGINE_DLL tagVertexPositionTexcoord
	{
		XMFLOAT3		vPosition;
		XMFLOAT2		vTexcoord;

		const static unsigned int					iNumElements = { 2 };
		const static D3D11_INPUT_ELEMENT_DESC		Elements[iNumElements];
	}VTXPOSTEX;

	typedef struct ENGINE_DLL tagVertexPositionPointSize
	{
		XMFLOAT3		vPosition;
		XMFLOAT2		vPSize;

		const static unsigned int					iNumElements = { 2 };
		const static D3D11_INPUT_ELEMENT_DESC		Elements[iNumElements];
	}VTXPOSPSIZE;

	typedef struct ENGINE_DLL tagVertexPositionCubeTexcoord
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vTexcoord;

		const static unsigned int					iNumElements = { 2 };
		const static D3D11_INPUT_ELEMENT_DESC		Elements[iNumElements];
	}VTXCUBETEX;

	typedef struct ENGINE_DLL tagVertexPositionNormalTexcoord
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexcoord;

		const static unsigned int					iNumElements = { 3 };
		const static D3D11_INPUT_ELEMENT_DESC		Elements[iNumElements];
	}VTXNORTEX;

	typedef struct ENGINE_DLL tagVertexMesh
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal; /* 픽셀마다 정의된 로컬스페이스에서의 z축 */
		XMFLOAT2		vTexcoord;
		XMFLOAT3		vTangent;/* 픽셀마다 정의된 로컬스페이스에서의 x축 */

		const static unsigned int					iNumElements = { 4 };
		const static D3D11_INPUT_ELEMENT_DESC		Elements[iNumElements];
	}VTXMESH;

	typedef struct ENGINE_DLL tagVertexAnimMesh
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexcoord;
		XMFLOAT3		vTangent;
		XMUINT4			vBlendIndices; /* 이 정점은 어떤 인덱스의 뼈를 이용하는가?! */
		XMFLOAT4		vBlendWeights;
		const static unsigned int					iNumElements = { 6 };
		const static D3D11_INPUT_ELEMENT_DESC		Elements[iNumElements];
	}VTXANIMMESH;

	typedef struct ENGINE_DLL tagVertexRectInstance
	{
		XMFLOAT4		vRight;
		XMFLOAT4		vUp;
		XMFLOAT4		vLook;
		XMFLOAT4		vTranslation;

		const static unsigned int					iNumElements = { 6 };
		const static D3D11_INPUT_ELEMENT_DESC		Elements[iNumElements];

	}VTXRECTINSTANCE;

	typedef struct ENGINE_DLL tagVertexPointInstance
	{
		XMFLOAT4		vRight;
		XMFLOAT4		vUp;
		XMFLOAT4		vLook;
		XMFLOAT4		vTranslation;

		const static unsigned int					iNumElements = { 6 };
		const static D3D11_INPUT_ELEMENT_DESC		Elements[iNumElements];

	}VTXPOINTINSTANCE;

	typedef struct ENGINE_DLL tagVertexMeshInstance
	{
		XMFLOAT4		vRight;
		XMFLOAT4		vUp;
		XMFLOAT4		vLook;
		XMFLOAT4		vTranslation;

		const static unsigned int					iNumElements = { 8 };
		const static D3D11_INPUT_ELEMENT_DESC		Elements[iNumElements];

	}VTXMESHINSTANCE;

	enum KEY_STATE
	{
		NONE, // 현재 눌리지 않았고, 이전에도 눌리지 않은 상태
		DOWN,  // 현재 처음으로 누른 상태(지금 누른 시점)
		HOLD, // 이전에도 눌렸고, 현재도 누른 상태
		UP,  // 이전에는 눌렸으나, 현재 뗀 상태(막 땐 시점)
		ERR //오류 상태 보통 게임인스턴스의 키매니저가 불완전한 상태
	};
}


