#include "stdafx.h"
#include "Dust.h"
#include "GameInstance.h"

CDust::CDust(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject(pDevice, pContext)
{
}

CDust::CDust(const CDust& rhs)
    : CGameObject(rhs)
{
}

HRESULT CDust::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CDust::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

	CVIBuffer_Point_Instance::POINTINSTANCEDESC		Desc = *(CVIBuffer_Point_Instance::POINTINSTANCEDESC*)pArg;

	m_Desc.iNumInstance = 300;
	m_Desc.fLifeTime = Desc.fLifeTime;
	m_Desc.vPivotPos = Desc.vPivotPos;
	//m_Desc.vRange = Desc.vRange;
	m_Desc.vRange = _float3(3.f, 0.f, 3.f);
	m_Desc.fMinSpeed = 10.f;
	m_Desc.fMaxSpeed = 30.f;	
	m_Desc.bFalling = Desc.bFalling;
	m_Desc.bRising = Desc.bRising;
	m_Desc.bExplosion = Desc.bExplosion;
	m_Desc.bBleeding = Desc.bBleeding;
	m_Desc.bDig = Desc.bDig;
	m_Desc.bFog = Desc.bFog;
	if (FAILED(Add_Components()))
		return E_FAIL;

	//m_fPos = Desc.vPivotPos;
	m_fDeadTimer = Desc.fLifeTime;
	m_bExplosion = Desc.bExplosion;
	m_bBleeding = Desc.bBleeding;
	m_bDig = Desc.bDig;
	m_bFog = Desc.bFog;
	m_bGray= Desc.bGray;

	random_device		RandomDevice;
	mt19937_64			RandomNumber;
	uniform_real_distribution<float>		RandomX;
	uniform_real_distribution<float>		RandomZ;

	RandomNumber = mt19937_64(RandomDevice());

	/*RandomX = uniform_real_distribution<float>(10.f, 30.f);
	RandomZ = uniform_real_distribution<float>(10.f, 50.f);*/

	/*RandomX = uniform_real_distribution<float>(70.f, 115.f);
	RandomZ = uniform_real_distribution<float>(10.f, 80.f);*/

	/*_vector TempPos = XMVectorSet(RandomX(RandomNumber), 0.f, RandomZ(RandomNumber) - 80.f, 1.f);
	XMStoreFloat4(&m_fPos, TempPos);*/

    return S_OK;
}

void CDust::Tick(_float fTimeDelta)
{
    __super::Tick(fTimeDelta);

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	m_fFrame += 32.f * fTimeDelta;

	if (m_fFrame >= 32.f)
		Set_Dead();

	//m_fPos.x = m_pTransformCom->Get_State(CTransform::STATE_POSITION).m128_f32[0];
	//m_fPos.y = m_pTransformCom->Get_State(CTransform::STATE_POSITION).m128_f32[1];
	//m_fPos.z = m_pTransformCom->Get_State(CTransform::STATE_POSITION).m128_f32[2];

	//_float fHeight = 0.f;

	//if (m_bExplosion || m_bDig)
	//{
	//	fHeight = (m_fTimer * m_fTimer * -9.8f * 1.f) + (m_fTimer * 2.f);

	//	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_fPos.x, m_fPos.y + fHeight, m_fPos.z, 1.f));

	//	/*if (m_pTransformCom->Get_State(CTransform::STATE_POSITION).m128_f32[1] < -7.f)
	//	{
	//		Set_Dead();
	//	}*/
	//}

    m_pVIBufferCom->Tick(fTimeDelta);

	Safe_Release(pGameInstance);
}

void CDust::Late_Tick(_float fTimeDelta)
{
    __super::Late_Tick(fTimeDelta);

    m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_BLEND, this);
}

HRESULT CDust::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

	m_pShaderCom->Begin(11);

    m_pVIBufferCom->Render();

    return S_OK;
}

HRESULT CDust::Add_Components()
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
	if (FAILED(__super::Add_Component(g_CurLevel, TEXT("Prototype_Component_Texture_Dust"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(g_CurLevel, TEXT("Prototype_Component_VIBuffer_Point_Instance"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom, &m_Desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CDust::Bind_ShaderResources()
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

CDust* CDust::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDust* pInstance = new CDust(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Created : CDust");
	}

	return pInstance;
}

CGameObject* CDust::Clone(void* pArg)
{
	CDust* pInstance = new CDust(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Cloned : CDust");
	}

	return pInstance;
}

void CDust::Free()
{
	__super::Free();

	m_pObject = { nullptr };
	m_pMonsterFireTransformCom = { nullptr };

	Safe_Release(m_pMonsterFireTransformCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
