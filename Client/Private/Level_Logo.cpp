#include "stdafx.h"
#include "..\Public\Level_Logo.h"
#include "GameInstance.h"
#include "Level_Loading.h"

CLevel_Logo::CLevel_Logo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Logo::Initialize()
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;

	/* 원형을 복제하여 사본을 만든다. */
	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	return S_OK;
}

void CLevel_Logo::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CLevel_Logo::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (GetKeyState(VK_RETURN) & 0x8000)
	{
		CGameInstance* pGameInstance = CGameInstance::Get_Instance();
		Safe_AddRef(pGameInstance);

		if (FAILED(pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_TOWN))))
		{
			Safe_Release(pGameInstance);
			return;
		}

		Safe_Release(pGameInstance);
	}

	SetWindowText(g_hWnd, TEXT("로고레벨입니다."));
}

HRESULT CLevel_Logo::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_BackGround(const wstring & strLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	/* For.BackGround */
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOGO, strLayerTag,
		TEXT("Prototype_GameObject_BackGround"))))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

CLevel_Logo * CLevel_Logo::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Logo*	pInstance = new CLevel_Logo(pDevice, pContext);	

	if (FAILED(pInstance->Initialize()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Created : CLevel_Logo");
	}

	return pInstance;
}

void CLevel_Logo::Free()
{
	__super::Free();


}
