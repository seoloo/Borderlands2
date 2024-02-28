#include "stdafx.h"
#include "Snow_Blue.h"
#include "GameInstance.h"

CSnow_Blue::CSnow_Blue(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject(pDevice, pContext)
{
}

CSnow_Blue::CSnow_Blue(const CSnow_Blue& rhs)
    : CGameObject(rhs)
{
}

HRESULT CSnow_Blue::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CSnow_Blue::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

	CVIBuffer_Point_Instance::POINTINSTANCEDESC		Desc = *(CVIBuffer_Point_Instance::POINTINSTANCEDESC*)pArg;

	m_Desc.iNumInstance = Desc.iNumInstance;
	m_Desc.fLifeTime = Desc.fLifeTime;
	m_Desc.vPivotPos = _float4(0.f,2.f,0.f,0.f);
	m_Desc.vRange = Desc.vRange;
	m_Desc.fMinSpeed = Desc.fMinSpeed;
	m_Desc.fMaxSpeed = Desc.fMaxSpeed;	m_Desc.bFalling = Desc.bFalling;
	m_Desc.bRising = Desc.bRising;
	m_Desc.bExplosion = Desc.bExplosion;
	m_Desc.bBleeding = Desc.bBleeding;
	m_Desc.bDig = Desc.bDig;

    if (FAILED(Add_Components()))
        return E_FAIL;
	
	m_fPos = Desc.vPivotPos;
	m_fDeadTimer = Desc.fLifeTime;
	m_bExplosion = Desc.bExplosion;
	m_bBleeding = Desc.bBleeding;
	m_bDig = Desc.bDig;

	m_pTransformCom->Scaled(_float3(0.1f, 0.1f, 0.1f));

    return S_OK;
}

void CSnow_Blue::Tick(_float fTimeDelta)
{
    __super::Tick(fTimeDelta);

	m_fTimer += fTimeDelta;

	/*if (m_fDeadTimer < m_fTimer)
		Set_Dead();*/

	/*m_fPos.x = m_pTransformCom->Get_State(CTransform::STATE_POSITION).m128_f32[0];
	m_fPos.y = m_pTransformCom->Get_State(CTransform::STATE_POSITION).m128_f32[1];
	m_fPos.z = m_pTransformCom->Get_State(CTransform::STATE_POSITION).m128_f32[2];*/

	_float fHeight = 0.f;

	if (m_bExplosion)
	{
		fHeight = (m_fTimer * m_fTimer * -9.8f * 0.5f) + (m_fTimer * 2.f);

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_fPos.x, m_fPos.y + fHeight, m_fPos.z, 1.f));

		if (m_pTransformCom->Get_State(CTransform::STATE_POSITION).m128_f32[1] < -7.f)
		{
			Set_Dead();
		}
	}

	if (m_bBleeding)
	{
		fHeight = (m_fTimer * m_fTimer * -9.8f * 0.5f) + (m_fTimer * 1.f);

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_fPos.x, m_fPos.y + fHeight + 1.f, m_fPos.z, 1.f));

		/*if (m_pTransformCom->Get_State(CTransform::STATE_POSITION).m128_f32[1] < -1.f)
		{
			Set_Dead();
		}*/
	}


    m_pVIBufferCom->Tick(fTimeDelta);
}

void CSnow_Blue::Late_Tick(_float fTimeDelta)
{
    __super::Late_Tick(fTimeDelta);

    m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);
}

HRESULT CSnow_Blue::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    m_pShaderCom->Begin(0);

    m_pVIBufferCom->Render();

    return S_OK;
}

HRESULT CSnow_Blue::Add_Components()
{
	/* Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof TransformDesc);

	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	/* Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxPointInstance"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Snow_Blue"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Point_Instance"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom, &m_Desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CSnow_Blue::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

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

	if (FAILED(m_pTextureCom->Bind_ShaderResourceView(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;

	return S_OK;
}

CSnow_Blue* CSnow_Blue::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSnow_Blue* pInstance = new CSnow_Blue(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Created : CSnow_Blue");
	}

	return pInstance;
}

CGameObject* CSnow_Blue::Clone(void* pArg)
{
	CSnow_Blue* pInstance = new CSnow_Blue(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Cloned : CSnow_Blue");
	}

	return pInstance;
}

void CSnow_Blue::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
