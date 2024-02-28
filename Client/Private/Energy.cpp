#include "stdafx.h"
#include "Energy.h"

#include "GameInstance.h"
#include "SoundMgr.h"

#include "Player.h"

CEnergy::CEnergy(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CEnergy::CEnergy(const CEnergy& rhs)
	:CGameObject(rhs)
{
}

HRESULT CEnergy::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEnergy::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	CVIBuffer_Point_Instance::POINTINSTANCEDESC		Desc = *(CVIBuffer_Point_Instance::POINTINSTANCEDESC*)pArg;

	m_Desc.iNumInstance = 1;
	m_Desc.fLifeTime = 1;
	//m_Desc.vPivotPos = _float4(10.f, 3.f, 10.f, 0.f);
	m_Desc.vPivotPos = _float4(0.f, 0.f, 0.f, 0.f);
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

	//m_fPos = Desc.vPivotPos;
	m_fDeadTimer = Desc.fLifeTime;
	m_bExplosion = Desc.bExplosion;
	m_bBleeding = Desc.bBleeding;
	m_bDig = Desc.bDig;
	m_bFog = Desc.bFog;
	m_bGray = Desc.bGray;
	m_bBlue = Desc.bBlue;
	m_bRed = Desc.bRed;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_iAtt = 1;

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);
	m_pObjectTransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Skag"), TEXT("Com_Transform")));
	m_pPlayerTransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_Transform")));

	XMStoreFloat4(&m_fPos, m_pObjectTransformCom->Get_State(CTransform::STATE_POSITION));

	m_fPos.y += 2.f;
	m_vPos = XMLoadFloat4(&m_fPos);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vPos);

	CSoundMgr::Get_Instance()->PlaySoundW(L"Docheol_pattern_r_2_1.wav", 1.f);

	m_vLook = m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION) - XMVectorSet(m_fPos.x, m_fPos.y - 1.5f, m_fPos.z, 1.f);

	Safe_Release(pGameInstance);

	return S_OK;
}

void CEnergy::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	m_pTransformCom->Go_Look(m_vLook,fTimeDelta);

	m_fFrame += 12.f * fTimeDelta;

	if (m_fFrame >= 4.f)
		m_fFrame = 0.f;

	m_pPlayer = pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Player"));

	Collision_Object();

	if (m_bCollision)
	{
		dynamic_cast<CPlayer*>(m_pPlayer)->Set_Attacked(true);

		random_device rd;
		mt19937 gen(rd());
		uniform_int_distribution<int> Demage(10, 15);

		if (dynamic_cast<CPlayer*>(m_pPlayer)->Get_Shield() > 0)
		{
			dynamic_cast<CPlayer*>(m_pPlayer)->Decrease_Shield(Demage(gen));
			dynamic_cast<CPlayer*>(m_pPlayer)->Set_MonsterAtt(50);
		}
		else
			dynamic_cast<CPlayer*>(m_pPlayer)->Decrease_Hp(Demage(gen));

	}
	else
		dynamic_cast<CPlayer*>(m_pPlayer)->Set_Attacked(false);

	if (m_bCollided || m_pTransformCom->Get_State(CTransform::STATE_POSITION).m128_f32[1] <= -1.f)
	{
		/*_ulong		dwByte = 0;
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
			if (!ReadFile(hFile, &Desc.bDig, sizeof(_bool), &dwByte, nullptr))
				break;
			if (!ReadFile(hFile, &Desc.bGreen, sizeof(_bool), &dwByte, nullptr))
				break;

			_vector TempPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

			XMStoreFloat4(&Desc.vPivotPos, TempPos);
			Desc.fMinSpeed = 1.f;
			Desc.fMaxSpeed = 3.f;
			Desc.vPivotPos.y = 5.f;
			Desc.bBlue = true;

			if (FAILED(pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Layer_Effect_Explosion"),
				TEXT("Prototype_GameObject_Lightning"), &Desc)))
				return;

		}
		CloseHandle(hFile);*/
		Set_Dead();
	}

	for (size_t i = 0; i < COLLIDER_END; i++)
	{
		if (nullptr != m_pColliderCom[i])
			m_pColliderCom[i]->Tick(m_pTransformCom->Get_WorldMatrix_Float4x4());
	}

	Safe_Release(pGameInstance);
}

void CEnergy::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_BLEND, this);
}

HRESULT CEnergy::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(19);

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

HRESULT CEnergy::Add_Components()
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
	if (FAILED(__super::Add_Component(g_CurLevel, TEXT("Prototype_Component_Shader_VtxPointInstance"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Component(g_CurLevel, TEXT("Prototype_Component_Texture_Energy"),
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

	ColliderDesc.fRadius = 3.f;
	ColliderDesc.vPosition = _float3(0.f, ColliderDesc.fRadius - 3.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_SPHERE"), (CComponent**)&m_pColliderCom[COLLIDER_SPHERE], &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CEnergy::Bind_ShaderResources()
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

HRESULT CEnergy::Collision_Object()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (m_pColliderCom[COLLIDER_SPHERE]->Collision((CCollider*)pGameInstance->Get_Component(g_CurLevel, TEXT("Layer_Player"), TEXT("Com_SPHERE"))) ||
		m_pColliderCom[COLLIDER_SPHERE]->Collision((CCollider*)pGameInstance->Get_Component(g_CurLevel, TEXT("Layer_Player"), TEXT("Com_AABB"))) ||
		m_pColliderCom[COLLIDER_SPHERE]->Collision((CCollider*)pGameInstance->Get_Component(g_CurLevel, TEXT("Layer_Player"), TEXT("Com_OBB"))))
	{

		m_bCollided = true;
		m_bCollision = true;
	}
	else
	{
		m_bCollision = false;

		m_bCollided = false;
	}

	Safe_Release(pGameInstance);

	return S_OK;
}

CEnergy* CEnergy::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEnergy* pInstance = new CEnergy(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Created : CEnergy");
	}

	return pInstance;
}

CGameObject* CEnergy::Clone(void* pArg)
{
	CEnergy* pInstance = new CEnergy(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Cloned : CEnergy");
	}

	return pInstance;
}

void CEnergy::Free()
{
	__super::Free();

	m_pPlayer = { nullptr };
	m_pPlayerTransformCom = { nullptr };
	m_pObjectTransformCom = { nullptr };

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pObjectTransformCom);
	Safe_Release(m_pPlayerTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);

	for (size_t i = 0; i < COLLIDER_END; i++)
		Safe_Release(m_pColliderCom[i]);
}
