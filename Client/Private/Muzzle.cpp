#include "stdafx.h"
#include "Muzzle.h"

#include "GameInstance.h"

#include "Player.h"
#include "TownPlayer.h"

CMuzzle::CMuzzle(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CMuzzle::CMuzzle(const CMuzzle & rhs)
	: CGameObject(rhs)
{
}

HRESULT CMuzzle::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMuzzle::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	_ulong		dwByte = 0;
	HANDLE		hFile = CreateFile(TEXT("../../Client/Bin/Resources/Effect/BleedingSnow.txt"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	size_t		dataLength;
	wstring		Name;

	CVIBuffer_Point_Instance::POINTINSTANCEDESC		Desc;
	while (true)
	{
		if (!ReadFile(hFile, &dataLength, sizeof(size_t), &dwByte, nullptr))
			break;
		if (dataLength == 0)
			break;
		if (0 == dwByte)
			break;
		Name.resize(dataLength);
		if (!ReadFile(hFile, &Name[0], sizeof(wchar_t) * dataLength, &dwByte, nullptr))
			break;
		if (!ReadFile(hFile, &Desc.iNumInstance, sizeof(_int), &dwByte, nullptr))
			break;
		if (!ReadFile(hFile, &Desc.fLifeTime, sizeof(_float), &dwByte, nullptr))
			break;
		if (!ReadFile(hFile, &Desc.vPivotPos, sizeof(_float4), &dwByte, nullptr))
			break;
		if (!ReadFile(hFile, &Desc.vRange, sizeof(_float3), &dwByte, nullptr))
			break;
		if (!ReadFile(hFile, &Desc.fMinSpeed, sizeof(_float), &dwByte, nullptr))
			break;
		if (!ReadFile(hFile, &Desc.fMaxSpeed, sizeof(_float), &dwByte, nullptr))
			break;
		if (!ReadFile(hFile, &Desc.bFalling, sizeof(_bool), &dwByte, nullptr))
			break;
		if (!ReadFile(hFile, &Desc.bRising, sizeof(_bool), &dwByte, nullptr))
			break;
		if (!ReadFile(hFile, &Desc.bExplosion, sizeof(_bool), &dwByte, nullptr))
			break;
		if (!ReadFile(hFile, &Desc.bBleeding, sizeof(_bool), &dwByte, nullptr))
			break;
	}
	CloseHandle(hFile);

	//CVIBuffer_Point_Instance::POINTINSTANCEDESC		Desc = *(CVIBuffer_Point_Instance::POINTINSTANCEDESC*)pArg;
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);


	m_Desc.iNumInstance = Desc.iNumInstance;
	m_Desc.fLifeTime = Desc.fLifeTime;

	/*m_Desc.vPivotPos = _float4(m_pPlayerTransform->Get_State(CTransform::STATE_LOOK).m128_f32[0], m_pPlayerTransform->Get_State(CTransform::STATE_LOOK).m128_f32[1],
		m_pPlayerTransform->Get_State(CTransform::STATE_LOOK).m128_f32[2], m_pPlayerTransform->Get_State(CTransform::STATE_LOOK).m128_f32[3]);*/
	//m_Desc.vPivotPos = Desc.vPivotPos;
	m_Desc.vRange = Desc.vRange;
	m_Desc.fMinSpeed = Desc.fMinSpeed;
	m_Desc.fMaxSpeed = Desc.fMaxSpeed;
	m_Desc.bFalling = Desc.bFalling;
	m_Desc.bRising = Desc.bRising;
	m_Desc.bExplosion = Desc.bExplosion;
	m_Desc.bBleeding = Desc.bBleeding;
	m_Desc.bGreen = Desc.bGreen;

	if (FAILED(Add_Components()))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

void CMuzzle::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	//m_pPlayer = pGameInstance->Get_GameObject(g_CurLevel, L"Layer_Player");

	m_fFrame += 20.f * fTimeDelta;

	if (m_fFrame >= 4.f)
	{
		Set_Dead();
		//m_fFrame = 0.f;
	}
}

void CMuzzle::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_BLEND, this);
}

HRESULT CMuzzle::Render()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	m_pPlayerTransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_Component(g_CurLevel, TEXT("Layer_Player"), TEXT("Com_Transform")));

	if (FAILED(__super::Render()))
		return E_FAIL;

	m_vPlayerPos = m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION);
	m_vPlayerLook = m_pPlayerTransformCom->Get_State(CTransform::STATE_LOOK);
	m_vPlayerUp = m_pPlayerTransformCom->Get_State(CTransform::STATE_UP);
	m_vPlayerRight = m_pPlayerTransformCom->Get_State(CTransform::STATE_RIGHT);


	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;
	
	m_pShaderCom->Begin(4);

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CMuzzle::Add_Components()
{
	/* Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;
	
	/* Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(__super::Add_Component(g_CurLevel, TEXT("Prototype_Component_Shader_VtxPointInstance"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom))) 
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(g_CurLevel, TEXT("Prototype_Component_VIBuffer_Point_Instance"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom, &m_Desc)))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Component(g_CurLevel, TEXT("Prototype_Component_Texture_Muzzle"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMuzzle::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	m_WorldMatrix._41 = m_vPlayerLook.m128_f32[0] + m_vPlayerUp.m128_f32[0] + m_vPlayerPos.m128_f32[0];
	m_WorldMatrix._42 = m_vPlayerLook.m128_f32[1] + m_vPlayerUp.m128_f32[1] - 0.07f;
	m_WorldMatrix._43 = m_vPlayerLook.m128_f32[2] + m_vPlayerUp.m128_f32[2] + m_vPlayerPos.m128_f32[2];
	
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;

	_float4x4 viewMatrix = pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW);
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &viewMatrix)))
		return E_FAIL;

	_float4x4 projMatrix = pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ);
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &projMatrix)))
		return E_FAIL;

	_float4 camPosition = pGameInstance->Get_CamPosition();
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", &camPosition, sizeof(_float4))))
		return E_FAIL;

	Safe_Release(pGameInstance);

	if (FAILED(m_pTextureCom->Bind_ShaderResourceView(m_pShaderCom, "g_DiffuseTexture", (_uint)m_fFrame)))
		return E_FAIL;

	return S_OK;
}

CMuzzle * CMuzzle::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMuzzle*	pInstance = new CMuzzle(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Created : CMuzzle");
	}

	return pInstance;
}

CGameObject * CMuzzle::Clone(void * pArg)
{
	CMuzzle*	pInstance = new CMuzzle(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Cloned : CMuzzle");
	}

	return pInstance;
}

void CMuzzle::Free()
{
	__super::Free();

	m_pPlayerTransformCom = { nullptr };
	Safe_Release(m_pPlayerTransformCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
}
