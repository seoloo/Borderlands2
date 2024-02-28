#include "stdafx.h"
#include "..\Public\Thresher_ArmCollider.h"

#include "GameInstance.h"
#include "Thresher_Arm.h"
#include "Thresher.h"
#include "Player.h"
CThresher_ArmCollider::CThresher_ArmCollider(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CThresher_ArmCollider::CThresher_ArmCollider(const CThresher_ArmCollider & rhs)
	: CGameObject(rhs)
{
}

HRESULT CThresher_ArmCollider::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CThresher_ArmCollider::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	COLLIDERDESC* pColliderDesc = (COLLIDERDESC*)pArg;

	m_pBoneMatrix = pColliderDesc->pBoneMatrix;
	m_pParentMatrix = pColliderDesc->pParentMatrix;
	m_PivotMatrix = pColliderDesc->PivotMatrix;
	g_CollidedArmNumber = m_iNumbering = pColliderDesc->Numbering;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(4.f, -9.f, -3.f, 1.f));

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);


	Safe_Release(pGameInstance);
	
	return S_OK;
}

void CThresher_ArmCollider::Tick(_float fTimeDelta)
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

	m_pObject = pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_ThresherArm"));

	m_pArmTransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_ThresherArm"), TEXT("Com_Transform")));

	if (m_bCollision)
	{
		m_pPlayer = pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Player"));
		m_fSoundTimer += fTimeDelta;
		if (m_fSoundTimer >= 0.5f)
		{
			dynamic_cast<CPlayer*>(m_pPlayer)->Decrease_Hp(1);

			m_fSoundTimer = 0.f;
		}
	}

	_float3 result = {};

	_float FontPosX = { m_pArmTransformCom->Get_State(CTransform::STATE_POSITION).m128_f32[0] };
	_float FontPosY = { m_pArmTransformCom->Get_State(CTransform::STATE_POSITION).m128_f32[2] };

	if (pGameInstance->Get_DIMouseState(CInput_Device::DIMK_LBUTTON) & 0x8000)
	{
		if (m_pColliderCom[COLLIDER_SPHERE]->Collision_Mouse())
		{
			_uint Number = dynamic_cast<CThresher_Arm*>(m_pObject)->Get_Numbering();
			if (m_iNumbering == Number)
			{
				m_fTimer += fTimeDelta;
				if (0.3f <= m_fTimer)
				{
					m_bDemage = true;
					m_pObject->Set_Attacked(true);

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

						XMStoreFloat4(&Desc.vPivotPos, m_pArmTransformCom->Get_State(CTransform::STATE_POSITION));
						Desc.vPivotPos.x += m_pArmTransformCom->Get_State(CTransform::STATE_LOOK).m128_f32[0];
						Desc.vPivotPos.z += m_pArmTransformCom->Get_State(CTransform::STATE_LOOK).m128_f32[2];
						Desc.vPivotPos.y = 1.5f;
						if (FAILED(pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Layer_Effect_Blood"),
							TEXT("Prototype_GameObject_Blood"), &Desc)))
							return;

					}
					CloseHandle(hFile);

					m_fTimer = 0.f;
					if (m_iNumbering == Number == 1)
					{
						dynamic_cast<CThresher_Arm*>(m_pObject)->Set_ArmsDead(true);
						g_ArmNumber = 0;
					}
				}
			}
			
		}
	}
	if (m_bDemage)
	{
		dynamic_cast<CThresher_Arm*>(m_pObject)->Decrease_Hp(1);
		m_bDemage = false;
	}
	/*if (m_bCollision)
	{
		m_pPlayer = pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Player"));
		m_fTimer += fTimeDelta;
		if (m_fTimer >= 1.0f)
		{
			m_pPlayer->Decrease_Hp(1);
			m_fTimer = 0.f;
		}
	}*/

	Safe_Release(pGameInstance);

	for (size_t i = 0; i < COLLIDER_END; i++)
	{
		if (nullptr != m_pColliderCom[i])
			m_pColliderCom[i]->Tick(XMLoadFloat4x4(&m_WorldMatrix));
	}
}

void CThresher_ArmCollider::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	Collision_Object();
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);

}

HRESULT CThresher_ArmCollider::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;	

	m_pShaderCom->Begin(0);
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	_uint Number = g_ArmNumber = dynamic_cast<CThresher_Arm*>(m_pObject)->Get_Numbering();

	//pGameInstance->Render_Font(TEXT("Font_136"), to_wstring(Number), _float2(50.f, 50.f), XMVectorSet(1.f, 0.f, 0.f, 1.f), 1.f);


	if (pGameInstance->Get_DIMouseState(CInput_Device::DIMK_RBUTTON) & 0x8000)
	{
		if (m_pColliderCom[COLLIDER_SPHERE]->Collision_Mouse())
		{
			g_bPsychoCollidedMouse = true;

			pGameInstance->Render_Font(TEXT("Font_136"), to_wstring(m_iNumbering), _float2(50.f, 100.f), XMVectorSet(1.f, 0.f, 0.f, 1.f), 1.f);
		}
		else
			g_bPsychoCollidedMouse = false;
	}
	Safe_Release(pGameInstance);

#ifdef _DEBUG

	for (size_t i = 0; i < COLLIDER_END; i++)
	{
		if (nullptr != m_pColliderCom[i])
			m_pColliderCom[i]->Render();
	}
#endif

	return S_OK;
}

HRESULT CThresher_ArmCollider::Add_Components()
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

	ColliderDesc.vSize = _float3(0.7f, 1.3f, 0.7f);
	ColliderDesc.vPosition = _float3(0.f, ColliderDesc.vSize.y * 0.5f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_AABB"), (CComponent**)&m_pColliderCom[COLLIDER_AABB], &ColliderDesc)))
		return E_FAIL;

	/* Com_OBB */
	ZeroMemory(&ColliderDesc, sizeof ColliderDesc);

	ColliderDesc.vSize = _float3(1.0f, 1.0f, 1.0f);
	ColliderDesc.vPosition = _float3(0.f, ColliderDesc.vSize.y * 0.5f, 0.f);
	ColliderDesc.vRotation = _float3(0.0f, XMConvertToRadians(45.0f), 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_OBB"), (CComponent**)&m_pColliderCom[COLLIDER_OBB], &ColliderDesc)))
		return E_FAIL;

	/* Com_SPHERE */
	ZeroMemory(&ColliderDesc, sizeof ColliderDesc);

	//15
	ColliderDesc.fRadius = 50.0f;
	ColliderDesc.vPosition = _float3(0.f, ColliderDesc.fRadius - 50.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_SPHERE"), (CComponent**)&m_pColliderCom[COLLIDER_SPHERE], &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CThresher_ArmCollider::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
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

HRESULT CThresher_ArmCollider::Collision_Object()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (m_pColliderCom[COLLIDER_SPHERE]->Collision((CCollider*)pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_OBB")))) {
		m_bCollision = true;
	}
	else
		m_bCollision = false;


	Safe_Release(pGameInstance);

	return S_OK;
}

CThresher_ArmCollider * CThresher_ArmCollider::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CThresher_ArmCollider*	pInstance = new CThresher_ArmCollider(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Created : CThresher_ArmCollider");
	}

	return pInstance;
}

CGameObject * CThresher_ArmCollider::Clone(void * pArg)
{
	CThresher_ArmCollider*	pInstance = new CThresher_ArmCollider(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Cloned : CThresher_ArmCollider");
	}

	return pInstance;
}

void CThresher_ArmCollider::Free()
{
	__super::Free();

	m_pPlayer = { nullptr };
	m_pArmTransformCom = { nullptr };
	Safe_Release(m_pArmTransformCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);

	for (size_t i = 0; i < COLLIDER_END; i++)
		Safe_Release(m_pColliderCom[i]);
}
