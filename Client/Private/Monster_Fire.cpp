#include "stdafx.h"
#include "Monster_Fire.h"

#include "GameInstance.h"

#include "Player.h"

CMonster_Fire::CMonster_Fire(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CMonster_Fire::CMonster_Fire(const CMonster_Fire& rhs)
	:CGameObject(rhs)
{
}

HRESULT CMonster_Fire::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonster_Fire::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	BulletDesc* Desc = (BulletDesc*)pArg;

	Desc->CreatePos.m128_f32[1] += 5.f;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, Desc->CreatePos);
	m_Dir = Desc->LookDir;

	//m_pTransformCom->Set_State(CTransform::STATE_LOOK, Desc->UpDir);
	//m_pTransformCom->Set_State(CTransform::STATE_UP, Desc->LookDir);

	m_iAtt = 1;

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	Safe_Release(pGameInstance);

	return S_OK;
}

void CMonster_Fire::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	m_pTransformCom->Go_Look(m_Dir,fTimeDelta);


	m_fTimer += fTimeDelta;

	if (m_fTimer > 5.f)
		Set_Dead();

	if (m_bCollision || m_pTransformCom->Get_State(CTransform::STATE_POSITION).m128_f32[1] <= 2.f)
	{
		_ulong		dwByte = 0;
		HANDLE		hFile = CreateFile(TEXT("../../Client/Bin/Resources/Effect/DigRock.txt"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
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
			if (!ReadFile(hFile, &Desc.bDig, sizeof(_bool), &dwByte, nullptr))
				break;
			if (!ReadFile(hFile, &Desc.bGreen, sizeof(_bool), &dwByte, nullptr))
				break;

			XMStoreFloat4(&Desc.vPivotPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
			Desc.vPivotPos.x = m_pTransformCom->Get_State(CTransform::STATE_POSITION).m128_f32[0];
			Desc.vPivotPos.z = m_pTransformCom->Get_State(CTransform::STATE_POSITION).m128_f32[2];
			Desc.vPivotPos.y = 0.f;
			if (FAILED(pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Layer_Effect_Rock"),
				TEXT("Prototype_GameObject_Rock"), &Desc)))
				return;

		}
		CloseHandle(hFile);
		Set_Dead();
	}

	/*if (m_pTransformCom->Get_State(CTransform::STATE_POSITION).m128_f32[1] <= 0.f) {


		Set_Dead();
	}*/

	for (size_t i = 0; i < COLLIDER_END; i++)
	{
		if (nullptr != m_pColliderCom[i])
			m_pColliderCom[i]->Tick(m_pTransformCom->Get_WorldMatrix_Float4x4());
	}

	Safe_Release(pGameInstance);
}

void CMonster_Fire::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	Collision_Object();

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CMonster_Fire::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_ShaderResourceView(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		m_pShaderCom->Begin(0);

		m_pModelCom->Render(i);
	}

#ifdef _DEBUG

	for (size_t i = 0; i < COLLIDER_END; i++)
	{
		if (nullptr != m_pColliderCom[i])
			m_pColliderCom[i]->Render();
	}
#endif

	return S_OK;
}

HRESULT CMonster_Fire::Add_Components()
{
	/* Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof TransformDesc);

	TransformDesc.fSpeedPerSec = 50.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_GeoSphere"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
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

	ColliderDesc.fRadius = 3.f;
	ColliderDesc.vPosition = _float3(0.f, ColliderDesc.fRadius - 3.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_SPHERE"), (CComponent**)&m_pColliderCom[COLLIDER_SPHERE], &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonster_Fire::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	/*if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;*/

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	/*_float4 camPosition = pGameInstance->Get_CamPosition();
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", &camPosition, sizeof(_float4))))
		return E_FAIL;*/

	Safe_Release(pGameInstance);

	/*if (FAILED(m_pTextureCom->Bind_ShaderResourceView(m_pShaderCom, "g_DiffuseTexture", (_uint)m_fFrame)))
		return E_FAIL;*/

	return S_OK;
}

HRESULT CMonster_Fire::Collision_Object()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (m_pColliderCom[COLLIDER_SPHERE]->Collision((CCollider*)pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_SPHERE"))))
	{
		m_bCollision = true;
	}

	Safe_Release(pGameInstance);

	return S_OK;
}

CMonster_Fire* CMonster_Fire::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMonster_Fire* pInstance = new CMonster_Fire(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Created : CMonster_Fire");
	}

	return pInstance;
}

CGameObject* CMonster_Fire::Clone(void* pArg)
{
	CMonster_Fire* pInstance = new CMonster_Fire(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Cloned : CMonster_Fire");
	}

	return pInstance;
}

void CMonster_Fire::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);

	for (size_t i = 0; i < COLLIDER_END; i++)
		Safe_Release(m_pColliderCom[i]);
}
