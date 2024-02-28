#include "stdafx.h"
#include "Lightning.h"
#include "GameInstance.h"

CLightning::CLightning(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject(pDevice, pContext)
{
}

CLightning::CLightning(const CLightning& rhs)
    : CGameObject(rhs)
{
}

HRESULT CLightning::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CLightning::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

	CVIBuffer_Point_Instance::POINTINSTANCEDESC		Desc = *(CVIBuffer_Point_Instance::POINTINSTANCEDESC*)pArg;

	m_Desc.iNumInstance = 1;
	m_Desc.fLifeTime = 1;
	m_Desc.vPivotPos = Desc.vPivotPos;
	//m_Desc.vRange = Desc.vRange;
	m_Desc.vRange = _float3(3.f, 0.f, 3.f);
	m_Desc.fMinSpeed = 10.f;
	m_Desc.fMaxSpeed = 30.f;	
	m_Desc.bFalling = false;
	m_Desc.bRising = false;
	m_Desc.bExplosion = false;
	m_Desc.bBleeding = true;
	m_Desc.bDig = false;
	m_Desc.bFog = false;

	if (FAILED(Add_Components()))
		return E_FAIL;

	//m_fPos = Desc.vPivotPos;
	m_fDeadTimer = Desc.fLifeTime;
	m_bExplosion = Desc.bExplosion;
	m_bBleeding = Desc.bBleeding;
	m_bDig = Desc.bDig;
	m_bFog = Desc.bFog;
	m_bGray= Desc.bGray;
	m_bBlue = Desc.bBlue;
	m_bRed = Desc.bRed;

	random_device		RandomDevice;
	mt19937_64			RandomNumber;
	uniform_real_distribution<float>		RandomX;
	uniform_real_distribution<float>		RandomZ;

	RandomNumber = mt19937_64(RandomDevice());

	/*RandomX = uniform_real_distribution<float>(10.f, 30.f);
	RandomZ = uniform_real_distribution<float>(10.f, 50.f);*/

	RandomX = uniform_real_distribution<float>(70.f, 115.f);
	RandomZ = uniform_real_distribution<float>(10.f, 80.f);

	/*_vector TempPos = XMVectorSet(RandomX(RandomNumber), 0.f, RandomZ(RandomNumber) - 80.f, 1.f);
	XMStoreFloat4(&m_fPos, TempPos);*/

    return S_OK;
}

void CLightning::Tick(_float fTimeDelta)
{
    __super::Tick(fTimeDelta);

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(7.f, 3.f, 15.f, 1.f));
	
	m_fFrame += 18.f * fTimeDelta;

	/*if (m_fFrame >= 16.f)
		Set_Dead();*/

    m_pVIBufferCom->Tick(fTimeDelta);

	for (size_t i = 0; i < COLLIDER_END; i++)
	{
		if (nullptr != m_pColliderCom[i])
			m_pColliderCom[i]->Tick(m_pTransformCom->Get_WorldMatrix_Float4x4());
	}

	if (m_bCollided)
		int a = 9;

	Safe_Release(pGameInstance);
}

void CLightning::Late_Tick(_float fTimeDelta)
{
    __super::Late_Tick(fTimeDelta);

	Collision_Object();

    m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_BLEND, this);
}

HRESULT CLightning::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

	if(m_bBlue)
		m_pShaderCom->Begin(13);
	else if(m_bRed)
		m_pShaderCom->Begin(14);

    m_pVIBufferCom->Render();

#ifdef _DEBUG

	for (size_t i = 0; i < COLLIDER_END; i++)
	{
		if (nullptr != m_pColliderCom[i])
			m_pColliderCom[i]->Render();
	}
#endif

    return S_OK;
}

HRESULT CLightning::Add_Components()
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
	if (FAILED(__super::Add_Component(g_CurLevel, TEXT("Prototype_Component_Texture_Lightning"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(g_CurLevel, TEXT("Prototype_Component_VIBuffer_Point_Instance"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom, &m_Desc)))
		return E_FAIL;

	/* Com_AABB */
	CCollider::COLLIDERDESC		ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof ColliderDesc);

	ColliderDesc.vSize = _float3(0.5f, 0.5f, 0.5f);
	ColliderDesc.vPosition = _float3(0.f, ColliderDesc.vSize.y * 0.5f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_AABB"), (CComponent**)&m_pColliderCom[COLLIDER_AABB], &ColliderDesc)))
		return E_FAIL;

	/* Com_OBB */
	ZeroMemory(&ColliderDesc, sizeof ColliderDesc);

	ColliderDesc.vSize = _float3(0.5f, 0.5f, 0.5f);
	ColliderDesc.vPosition = _float3(0.f, ColliderDesc.vSize.y * 0.5f, 0.f);
	ColliderDesc.vRotation = _float3(0.0f, XMConvertToRadians(45.0f), 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_OBB"), (CComponent**)&m_pColliderCom[COLLIDER_OBB], &ColliderDesc)))
		return E_FAIL;

	/* Com_SPHERE */
	ZeroMemory(&ColliderDesc, sizeof ColliderDesc);

	ColliderDesc.fRadius = 30.f;
	ColliderDesc.vPosition = _float3(0.f, ColliderDesc.fRadius, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_SPHERE"), (CComponent**)&m_pColliderCom[COLLIDER_SPHERE], &ColliderDesc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CLightning::Bind_ShaderResources()
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

HRESULT CLightning::Collision_Object()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (m_pColliderCom[COLLIDER_SPHERE]->Collision((CCollider*)pGameInstance->Get_Component(g_CurLevel, TEXT("Layer_Player"), TEXT("Com_OBB")))) {
		m_bCollided = true;
	}

	Safe_Release(pGameInstance);

	return S_OK;
}

CLightning* CLightning::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLightning* pInstance = new CLightning(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Created : CLightning");
	}

	return pInstance;
}

CGameObject* CLightning::Clone(void* pArg)
{
	CLightning* pInstance = new CLightning(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Cloned : CLightning");
	}

	return pInstance;
}

void CLightning::Free()
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

	for (size_t i = 0; i < COLLIDER_END; i++)
		Safe_Release(m_pColliderCom[i]);
}
