#include "Bleeding.h"
#include "GameInstance.h"

CBleeding::CBleeding(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject(pDevice, pContext)
{
}

CBleeding::CBleeding(const CBleeding& rhs)
    : CGameObject(rhs)
{
}

HRESULT CBleeding::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CBleeding::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

	CVIBuffer_Point_Instance::POINTINSTANCEDESC		Desc = *(CVIBuffer_Point_Instance::POINTINSTANCEDESC*)pArg;

	m_Desc.iNumInstance = Desc.iNumInstance;
	m_Desc.fLifeTime = Desc.fLifeTime;
	m_Desc.vPivotPos = Desc.vPivotPos;
	m_Desc.vRange = Desc.vRange;
	m_Desc.fMinSpeed = Desc.fMinSpeed;
	m_Desc.fMaxSpeed = Desc.fMaxSpeed;
	m_Desc.bFalling = Desc.bFalling;
	m_Desc.bRising = Desc.bRising;
	m_Desc.bExplosion = Desc.bExplosion;
	m_Desc.bBleeding = Desc.bBleeding;
	m_Desc.bDig = Desc.bDig;

    if (FAILED(Add_Components()))
        return E_FAIL;
	

	m_fDeadTimer = Desc.fLifeTime;
	m_bExplosion = Desc.bExplosion;
	m_bBleeding = Desc.bBleeding;

    return S_OK;
}

void CBleeding::Tick(_float fTimeDelta)
{
    __super::Tick(fTimeDelta);

	m_fFrame += 24.f * fTimeDelta * 3;

	if (m_fFrame >= 24.f)
		Set_Dead();

	m_fTimer += fTimeDelta;

	if (m_fDeadTimer < m_fTimer)
		Set_Dead();

	m_fPos.x = m_pTransformCom->Get_State(CTransform::STATE_POSITION).m128_f32[0];
	m_fPos.y = m_pTransformCom->Get_State(CTransform::STATE_POSITION).m128_f32[1];
	m_fPos.z = m_pTransformCom->Get_State(CTransform::STATE_POSITION).m128_f32[2];

	_float fHeight = 0.f;

	//if (m_bBleeding)
	//{
	//	fHeight = (m_fTimer * m_fTimer * -9.8 * 0.5) + (m_fTimer * 0.1f);

	//	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_fPos.x, m_fPos.y + fHeight, m_fPos.z, 1.f));

	//	/*if (m_pTransformCom->Get_State(CTransform::STATE_POSITION).m128_f32[1] < -1.f)
	//	{
	//		Set_Dead();
	//	}*/
	//}


    m_pVIBufferCom->Tick(fTimeDelta);
}

void CBleeding::Late_Tick(_float fTimeDelta)
{
    __super::Late_Tick(fTimeDelta);

    m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);
}

HRESULT CBleeding::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    m_pShaderCom->Begin(0);

    m_pVIBufferCom->Render();

    return S_OK;
}

HRESULT CBleeding::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPointInstance"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Bleeding"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Point_Instance"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom, &m_Desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBleeding::Bind_ShaderResources()
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

	if (FAILED(m_pTextureCom->Bind_ShaderResourceView(m_pShaderCom, "g_DiffuseTexture", (_uint)m_fFrame)))
		return E_FAIL;

	return S_OK;
}

CBleeding* CBleeding::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBleeding* pInstance = new CBleeding(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Created : CBleeding");
	}

	return pInstance;
}

CGameObject* CBleeding::Clone(void* pArg)
{
	CBleeding* pInstance = new CBleeding(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Cloned : CBleeding");
	}

	return pInstance;
}

void CBleeding::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
