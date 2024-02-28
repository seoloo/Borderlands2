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
		XMFLOAT3		vNormal; /* �ȼ����� ���ǵ� ���ý����̽������� z�� */
		XMFLOAT2		vTexcoord;
		XMFLOAT3		vTangent;/* �ȼ����� ���ǵ� ���ý����̽������� x�� */

		const static unsigned int					iNumElements = { 4 };
		const static D3D11_INPUT_ELEMENT_DESC		Elements[iNumElements];
	}VTXMESH;

	typedef struct ENGINE_DLL tagVertexAnimMesh
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexcoord;
		XMFLOAT3		vTangent;
		XMUINT4			vBlendIndices; /* �� ������ � �ε����� ���� �̿��ϴ°�?! */
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
		NONE, // ���� ������ �ʾҰ�, �������� ������ ���� ����
		DOWN,  // ���� ó������ ���� ����(���� ���� ����)
		HOLD, // �������� ���Ȱ�, ���絵 ���� ����
		UP,  // �������� ��������, ���� �� ����(�� �� ����)
		ERR //���� ���� ���� �����ν��Ͻ��� Ű�Ŵ����� �ҿ����� ����
	};
}


