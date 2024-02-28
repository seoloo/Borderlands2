#include "stdafx.h"
#include "Critical.h"
#include "GameInstance.h"

CCritical::CCritical(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject(pDevice, pContext)
{
}

CCritical::CCritical(const CCritical& rhs)
    : CGameObject(rhs)
{
}

HRESULT CCritical::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CCritical::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

	CVIBuffer_Point_Instance::POINTINSTANCEDESC		Desc = *(CVIBuffer_Point_Instance::POINTINSTANCEDESC*)pArg;

	m_Desc.iNumInstance = 1;
	m_Desc.fLifeTime = 0;
	m_fPos = m_Desc.vPivotPos = Desc.vPivotPos;
	m_Desc.vRange = _float3(0.f, 0.f, 0.f);
	m_Desc.fMinSpeed = 1;
	m_Desc.fMaxSpeed = 2;	
	m_Desc.bBleeding = true;

    if (FAILED(Add_Components()))
        return E_FAIL;
	
	m_fPos.y += 2.f;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, m_fPos.y, 0.f, 1.f));

    return S_OK;
}

void CCritical::Tick(_float fTimeDelta)
{
    __super::Tick(fTimeDelta);

	/*m_fTimer += fTimeDelta;

	if (0.5f < m_fTimer)
		Set_Dead();*/

	m_fFrame += 6.f * fTimeDelta;

	if (m_fFrame >= 2.f)
		Set_Dead();

	_float fHeight = 0.f;

	fHeight = (m_fTimer * m_fTimer * -9.8f * 0.7f) + (m_fTimer * 3.f);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, m_fPos.y + fHeight, 0.f, 1.f));

	if (m_pTransformCom->Get_State(CTransform::STATE_POSITION).m128_f32[1] < -7.f)
	{
		Set_Dead();
	}

    m_pVIBufferCom->Tick(fTimeDelta);
}

void CCritical::Late_Tick(_float fTimeDelta)
{
    __super::Late_Tick(fTimeDelta);

    m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_BLEND, this);
}

HRESULT CCritical::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    m_pShaderCom->Begin(21);

    m_pVIBufferCom->Render();

    return S_OK;
}

HRESULT CCritical::Add_Components()
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
	if (FAILED(__super::Add_Component(g_CurLevel, TEXT("Prototype_Component_Shader_VtxPointInstance"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Component(g_CurLevel, TEXT("Prototype_Component_Texture_Critical"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(g_CurLevel, TEXT("Prototype_Component_VIBuffer_Point_Instance"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom, &m_Desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CCritical::Bind_ShaderResources()
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

	if (FAILED(m_pTextureCom->Bind_ShaderResourceView(m_pShaderCom, "g_DiffuseTexture", m_fFrame)))
		return E_FAIL;

	return S_OK;
}

CCritical* CCritical::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCritical* pInstance = new CCritical(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Created : CCritical");
	}

	return pInstance;
}

CGameObject* CCritical::Clone(void* pArg)
{
	CCritical* pInstance = new CCritical(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Cloned : CCritical");
	}

	return pInstance;
}

void CCritical::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
