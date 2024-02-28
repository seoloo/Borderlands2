#include "stdafx.h"
#include "..\Public\SpiderAnt_Collider.h"

#include "GameInstance.h"

CSpiderAnt_Collider::CSpiderAnt_Collider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CSpiderAnt_Collider::CSpiderAnt_Collider(const CSpiderAnt_Collider& rhs)
	: CGameObject(rhs)
{
}

HRESULT CSpiderAnt_Collider::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSpiderAnt_Collider::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	COLLIDERDESC* pColliderDesc = (COLLIDERDESC*)pArg;

	m_pBoneMatrix = pColliderDesc->pBoneMatrix;
	m_pParentMatrix = pColliderDesc->pParentMatrix;
	m_PivotMatrix = pColliderDesc->PivotMatrix;

	if (FAILED(Add_Components()))
		return E_FAIL;


	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	Safe_Release(pGameInstance);

	return S_OK;
}

void CSpiderAnt_Collider::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	_matrix		BoneMatrix = XMLoadFloat4x4(m_pBoneMatrix);

	BoneMatrix.r[0] = XMVector3Normalize(BoneMatrix.r[0]);
	BoneMatrix.r[1] = XMVector3Normalize(BoneMatrix.r[1]);
	BoneMatrix.r[2] = XMVector3Normalize(BoneMatrix.r[2]);

	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix_Float4x4() *
		BoneMatrix *
		XMLoadFloat4x4(&m_PivotMatrix) *
		XMLoadFloat4x4(m_pParentMatrix));

	m_pObject = pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_SpiderAnt"));


	_float3 result = {};

	if (pGameInstance->Get_DIMouseState(CInput_Device::DIMK_LBUTTON) & 0x8000)
	{
		if (m_pColliderCom[COLLIDER_AABB]->Collision_Mouse() ||
			m_pColliderCom[COLLIDER_OBB]->Collision_Mouse() ||
			m_pColliderCom[COLLIDER_SPHERE]->Collision_Mouse())
		{
			m_fTimer += fTimeDelta;
			if (0.3f <= m_fTimer)
			{
				m_bDemage = true;
				m_pObject->Set_Attacked(true);

				m_pPlayerTransform = dynamic_cast<CTransform*>(pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_SpiderAnt"), TEXT("Com_Transform")));

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
					ReadFile(hFile, &Name[0], sizeof(wchar_t) * dataLength, &dwByte, nullptr);
					ReadFile(hFile, &Desc.iNumInstance, sizeof(_int), &dwByte, nullptr);
					ReadFile(hFile, &Desc.fLifeTime, sizeof(_float), &dwByte, nullptr);
					ReadFile(hFile, &Desc.vPivotPos, sizeof(_float4), &dwByte, nullptr);
					ReadFile(hFile, &Desc.vRange, sizeof(_float3), &dwByte, nullptr);
					ReadFile(hFile, &Desc.fMinSpeed, sizeof(_float), &dwByte, nullptr);
					ReadFile(hFile, &Desc.fMaxSpeed, sizeof(_float), &dwByte, nullptr);
					ReadFile(hFile, &Desc.bFalling, sizeof(_bool), &dwByte, nullptr);
					ReadFile(hFile, &Desc.bRising, sizeof(_bool), &dwByte, nullptr);
					ReadFile(hFile, &Desc.bExplosion, sizeof(_bool), &dwByte, nullptr);
					ReadFile(hFile, &Desc.bBleeding, sizeof(_bool), &dwByte, nullptr);
					ReadFile(hFile, &Desc.bDig, sizeof(_bool), &dwByte, nullptr);
					ReadFile(hFile, &Desc.bGreen, sizeof(_bool), &dwByte, nullptr);

					XMStoreFloat4(&Desc.vPivotPos, m_pPlayerTransform->Get_State(CTransform::STATE_POSITION));
					Desc.vPivotPos.x += m_pPlayerTransform->Get_State(CTransform::STATE_LOOK).m128_f32[0];
					Desc.vPivotPos.z += m_pPlayerTransform->Get_State(CTransform::STATE_LOOK).m128_f32[2];
					Desc.vPivotPos.y = 6.f;
					if (FAILED(pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Layer_Effect_Blood"),
						TEXT("Prototype_GameObject_Explosion"), &Desc)))
						return;

				}
				CloseHandle(hFile);
				m_fTimer = 0.f;
			}
		}
	}
	if (m_bDemage)
	{
		random_device rd;
		mt19937 gen(rd());
		uniform_int_distribution<int> Demage(30, 37);

		m_pObject->Decrease_Hp(Demage(gen));
		m_bDemage = false;

		CVIBuffer_Point_Instance::POINTINSTANCEDESC		Desc;
		_vector TempPos = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);
		TempPos.m128_f32[1] += 4.f;
		Desc.iNum = 1;

		XMStoreFloat4(&Desc.vPivotPos, TempPos);
		Desc.iDamage = Demage(gen);
		if (FAILED(pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Layer_Effect_Blast"),
			TEXT("Prototype_GameObject_DamageFont"), &Desc)))
			return;
	}
	Safe_Release(pGameInstance);

	for (size_t i = 0; i < COLLIDER_END; i++)
	{
		if (nullptr != m_pColliderCom[i])
			m_pColliderCom[i]->Tick(XMLoadFloat4x4(&m_WorldMatrix));
	}
}

void CSpiderAnt_Collider::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

}

HRESULT CSpiderAnt_Collider::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(0);
#ifdef _DEBUG

	for (size_t i = 0; i < COLLIDER_END; i++)
	{
		if (nullptr != m_pColliderCom[i])
			m_pColliderCom[i]->Render();
	}
#endif
	
	
	return S_OK;
}

HRESULT CSpiderAnt_Collider::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* Com_AABB */
	CCollider::COLLIDERDESC		ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof ColliderDesc);

	ColliderDesc.vSize = _float3(55.0f, 75.0f, 55.0f);
	ColliderDesc.vPosition = _float3(0.f, ColliderDesc.vSize.y * 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_AABB"), (CComponent**)&m_pColliderCom[COLLIDER_AABB], &ColliderDesc)))
		return E_FAIL;

	/* Com_OBB */
	ZeroMemory(&ColliderDesc, sizeof ColliderDesc);

	ColliderDesc.vSize = _float3(10.0f, 10.0f, 10.0f);
	ColliderDesc.vPosition = _float3(0.f, ColliderDesc.vSize.y * 0.5f, 0.f);
	ColliderDesc.vRotation = _float3(0.0f, XMConvertToRadians(45.0f), 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_OBB"), (CComponent**)&m_pColliderCom[COLLIDER_OBB], &ColliderDesc)))
		return E_FAIL;

	/* Com_SPHERE */
	ZeroMemory(&ColliderDesc, sizeof ColliderDesc);

	ColliderDesc.fRadius = 25.f;
	ColliderDesc.vPosition = _float3(0.f, ColliderDesc.fRadius - 3.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_SPHERE"), (CComponent**)&m_pColliderCom[COLLIDER_SPHERE], &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CSpiderAnt_Collider::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;


	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CSpiderAnt_Collider::Collision_Object()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (m_pColliderCom[COLLIDER_AABB]->Collision((CCollider*)pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_OBB")))) {
		m_bCollision = true;
	}

	Safe_Release(pGameInstance);

	return S_OK;
}

CSpiderAnt_Collider* CSpiderAnt_Collider::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSpiderAnt_Collider* pInstance = new CSpiderAnt_Collider(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Created : CSpiderAnt_Collider");
	}

	return pInstance;
}

CGameObject* CSpiderAnt_Collider::Clone(void* pArg)
{
	CSpiderAnt_Collider* pInstance = new CSpiderAnt_Collider(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Cloned : CSpiderAnt_Collider");
	}

	return pInstance;
}

void CSpiderAnt_Collider::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);

	for (size_t i = 0; i < COLLIDER_END; i++)
		Safe_Release(m_pColliderCom[i]);
}
