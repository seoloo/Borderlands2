#include "stdafx.h"
#include "Explosion.h"
#include "GameInstance.h"

CExplosion::CExplosion(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject(pDevice, pContext)
{
}

CExplosion::CExplosion(const CExplosion& rhs)
    : CGameObject(rhs)
{
}

HRESULT CExplosion::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CExplosion::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

	CVIBuffer_Point_Instance::POINTINSTANCEDESC		Desc = *(CVIBuffer_Point_Instance::POINTINSTANCEDESC*)pArg;

	m_Desc.iNumInstance = Desc.iNumInstance;
	m_Desc.fLifeTime = Desc.fLifeTime;
	m_Desc.vPivotPos = Desc.vPivotPos;
	//m_Desc.vRange = Desc.vRange;
	m_Desc.vRange = _float3(3.f, 0.f, 3.f);
	m_Desc.fMinSpeed = 1.f;
	m_Desc.fMaxSpeed = 3.f;	
	m_Desc.bFalling = Desc.bFalling;
	m_Desc.bRising = Desc.bRising;
	m_Desc.bExplosion = Desc.bExplosion;
	m_Desc.bBleeding = Desc.bBleeding;
	m_Desc.bDig = Desc.bDig;
	m_Desc.bFog = Desc.bFog;
	if (FAILED(Add_Components()))
		return E_FAIL;

	m_fPos = Desc.vPivotPos;
	m_fDeadTimer = Desc.fLifeTime;
	m_bExplosion = Desc.bExplosion;
	m_bBleeding = Desc.bBleeding;
	m_bDig = Desc.bDig;
	m_bFog = Desc.bFog;
	m_bGray= Desc.bGray;
	m_bBlue = Desc.bBlue;
	m_bBlack = Desc.bBlack;
	m_bRed = Desc.bRed;

    return S_OK;
}

void CExplosion::Tick(_float fTimeDelta)
{
    __super::Tick(fTimeDelta);

	m_fFrame += 24.f * fTimeDelta;

	/*if (m_fFrame >= 16.f)
		Set_Dead();*/

    m_pVIBufferCom->Tick(fTimeDelta);
}

void CExplosion::Late_Tick(_float fTimeDelta)
{
    __super::Late_Tick(fTimeDelta);

    m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_BLEND, this);
}

HRESULT CExplosion::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

	/*if(!m_bGray && !m_bBlue)
		m_pShaderCom->Begin(7);
	if(m_bGray && !m_bBlue)
		m_pShaderCom->Begin(8);
	if(m_bBlack)
		m_pShaderCom->Begin(10);*/
	//if(m_bRed)
		m_pShaderCom->Begin(10);

    m_pVIBufferCom->Render();

    return S_OK;
}

HRESULT CExplosion::Add_Components()
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

	/* Com_Texture */
	if (FAILED(__super::Add_Component(g_CurLevel, TEXT("Prototype_Component_Texture_Explosion"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(g_CurLevel, TEXT("Prototype_Component_VIBuffer_Point_Instance"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom, &m_Desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CExplosion::Bind_ShaderResources()
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

CExplosion* CExplosion::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CExplosion* pInstance = new CExplosion(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Created : CExplosion");
	}

	return pInstance;
}

CGameObject* CExplosion::Clone(void* pArg)
{
	CExplosion* pInstance = new CExplosion(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Cloned : CExplosion");
	}

	return pInstance;
}

void CExplosion::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
