#include "stdafx.h"
#include "..\Public\ThresherUI.h"

#include "GameInstance.h"
#include "Player.h"
#include "TownPlayer.h"

CThresherUI::CThresherUI(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CThresherUI::CThresherUI(const CThresherUI & rhs)
	: CGameObject(rhs)
{
}

HRESULT CThresherUI::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CThresherUI::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

void CThresherUI::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (g_bThersherSummon)
	{
		if (m_bDead)
			Set_Dead();
	}

	Safe_Release(pGameInstance);
}

void CThresherUI::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CThresherUI::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (g_bThersherSummon)
	{
		if (FAILED(pGameInstance->Render_Font(TEXT("Font_136"), to_wstring(g_ArmNumber), _float2(50.f, 50.f), XMVectorSet(1.f, 0.f, 0.f, 1.f), 1.f)))
			return E_FAIL;
		if (FAILED(pGameInstance->Render_Font(TEXT("Font_136"), to_wstring(g_Arm2Number), _float2(80.f, 50.f), XMVectorSet(0.f, 1.f, 0.f, 1.f), 1.f)))
			return E_FAIL;
		if (FAILED(pGameInstance->Render_Font(TEXT("Font_136"), to_wstring(g_MeleeNumber), _float2(110.f, 50.f), XMVectorSet(0.f, 0.f, 1.f, 1.f), 1.f)))
			return E_FAIL;

		if (g_ArmNumber == 0 && g_ArmNumber == 0 && g_MeleeNumber == 0)
			m_bDead = true;

		/*if (g_bPsychoCollidedMouse)
		{
			if (g_CollidedArmNumber != 0)
			{
				if (FAILED(pGameInstance->Render_Font(TEXT("Font_136"), to_wstring(g_CollidedArmNumber), _float2(50.f, 100.f), XMVectorSet(1.f, 0.f, 0.f, 1.f), 1.f)))
					return E_FAIL;
			}
		}
		if (g_bPsychoCollidedMouse2)
		{
			if (g_CollidedArm2Number != 0)
			{
				if (FAILED(pGameInstance->Render_Font(TEXT("Font_136"), to_wstring(g_CollidedArm2Number), _float2(75.f, 100.f), XMVectorSet(0.f, 1.f, 0.f, 1.f), 1.f)))
					return E_FAIL;
			}
		}
		if (g_bPsychoCollidedMouse3)
		{
			if (g_CollidedMeleeNumber != 0)
			{
				if (FAILED(pGameInstance->Render_Font(TEXT("Font_136"), to_wstring(g_CollidedMeleeNumber), _float2(100.f, 100.f), XMVectorSet(0.f, 0.f, 1.f, 1.f), 1.f)))
					return E_FAIL;
			}
		}*/
	}
	
	

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CThresherUI::Add_Components()
{
	/* Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	return S_OK;
}

CThresherUI * CThresherUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CThresherUI*	pInstance = new CThresherUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Created : CThresherUI");
	}

	return pInstance;
}

CGameObject * CThresherUI::Clone(void * pArg)
{
	CThresherUI*	pInstance = new CThresherUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Cloned : CThresherUI");
	}

	return pInstance;
}

void CThresherUI::Free()
{
	__super::Free();

	m_pObject = { nullptr };

	Safe_Release(m_pRendererCom);
}
