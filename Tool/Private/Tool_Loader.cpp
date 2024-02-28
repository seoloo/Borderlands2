#include "Tool_Loader.h"
#include "GameInstance.h"

#include "Tool_Terrain.h"
#include "Tool_Camera.h"
#include "Tool_Object_Icebox01.h"

CTool_Loader::CTool_Loader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice) 
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

unsigned int APIENTRY ThreadEntry(void* pArg)
{
	CTool_Loader*		pLoader = (CTool_Loader*)pArg;

	/*  모든 컴객체를 초기화하낟. */
	CoInitializeEx(nullptr, 0);

	if (FAILED(pLoader->LoadingForNextLevel()))
		return 1;

	return 0;
}

HRESULT CTool_Loader::Initialize(LEVEL eNextLevel)
{
	m_eNextLevel = eNextLevel;	

	InitializeCriticalSection(&m_CriticalSection);

	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, ThreadEntry, this, 0, nullptr);

	if (0 == m_hThread)
		return E_FAIL;	

	return S_OK;
}

HRESULT CTool_Loader::LoadingForNextLevel()
{
	EnterCriticalSection(&m_CriticalSection);	

	HRESULT		hr = { 0 };

	switch (m_eNextLevel)
	{
	case LEVEL_TOOL:
		hr = Loading_ForTool();
		break;
	}

	if (FAILED(hr))
		goto except;		

	LeaveCriticalSection(&m_CriticalSection);
	return S_OK;

except:
	LeaveCriticalSection(&m_CriticalSection);
	return E_FAIL;
}

HRESULT CTool_Loader::Loading_ForTool()
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	lstrcpy(m_szLoading, TEXT("텍스쳐를 로딩 중입니다."));
#pragma region Terrain

	/* For.Prototype_Component_Texture_SlateRock3_Dif */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Texture_SlateRock3_Dif"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Borderland/Terrain/SlateRock3_Dif.dds")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_SnowDrift_01_Dif */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Texture_SnowDrift_01_Dif"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Borderland/Terrain/SnowDrift_01_Dif.dds")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Bright_Lava_Tile2_Dif */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Texture_Bright_Lava_Tile2_Dif"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Borderland/Terrain/Bright_Lava_Tile2_Dif.dds")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Tile0 */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Texture_Tile0"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Terrain/Tile0.dds")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Snow */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Texture_Snow"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Borderland/Terrain/Snow.dds")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_FrozenLake */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Texture_FrozenLake"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Borderland/Terrain/FrozenLake.dds")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Grassland */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Texture_Grassland"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Borderland/Terrain/Grassland.dds")))))
		return E_FAIL;

	

#pragma endregion


	lstrcpy(m_szLoading, TEXT("모델을 로딩 중입니다."));
	/* For.Prototype_Component_VIBuffer_Terrain */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Terrain/Flatland.bmp")))))
		return E_FAIL;

#pragma region Glacial_Static
	_matrix		PivotMatrix = XMMatrixIdentity();
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(0.f));

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_IceBlock01"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Client/Bin/Resources/Borderland/Glacial_Static/IceBlock01.fbx", PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_IceBlock02"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Client/Bin/Resources/Borderland/Glacial_Static/IceBlock02.fbx", PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_IceBlock03"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Client/Bin/Resources/Borderland/Glacial_Static/IceBlock03.fbx", PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_IceBlock04"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Client/Bin/Resources/Borderland/Glacial_Static/IceBlock04.fbx", PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_IceBlock05"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Client/Bin/Resources/Borderland/Glacial_Static/IceBlock05.fbx", PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_IceBlock06"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Client/Bin/Resources/Borderland/Glacial_Static/IceBlock06.fbx", PivotMatrix))))
		return E_FAIL;

#pragma endregion

	lstrcpy(m_szLoading, TEXT("셰이더를 로딩 중입니다."));
	/* For.Prototype_Component_Shader_VtxNorTex */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Shader_VtxNorTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxMesh */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Shader_VtxMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxMesh.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;

	lstrcpy(m_szLoading, TEXT("객체원형 로딩 중입니다."));
	/* For.Terrain */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain"),
		CTool_Terrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Tool_Camera */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Tool_Camera"),
		CTool_Camera::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma region Glacial_Static
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_IceBlock01"),
		CTool_Object_Icebox01::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma endregion

	lstrcpy(m_szLoading, TEXT("로딩이 완료되었습니다."));

	Safe_Release(pGameInstance);

	m_isFinished = true;


	return S_OK;
}

CTool_Loader* CTool_Loader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevel)
{
	CTool_Loader*	pInstance = new CTool_Loader(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevel)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Created : CLoader");
	}

	return pInstance;
}

void CTool_Loader::Free()
{
	WaitForSingleObject(m_hThread, INFINITE);

	CloseHandle(m_hThread);

	DeleteObject(m_hThread);

	DeleteCriticalSection(&m_CriticalSection);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
