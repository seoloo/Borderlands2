#include "stdafx.h"
#include "..\Public\Player.h"

#include "GameInstance.h"
#include "SoundMgr.h"

#include "Pistol.h"
#include "Camera_OnBone.h"
#include "Camera_Pistol.h"
#include "Zoom.h"
#include "Skag.h"
#include "Stalker.h"
#include "Psycho.h"

CPlayer::CPlayer(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CLandObject(pDevice, pContext)
{
}

CPlayer::CPlayer(const CPlayer & rhs)
	: CLandObject(rhs)
{
}

HRESULT CPlayer::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(Add_PlayerParts()))
		return E_FAIL;

	m_iMaxHp = 250;
	m_iHp = m_iMaxHp;

	m_iShield = 150;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(2.5f,0.f, 2.5f,1.f));
	m_vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(80.f,0.f, 2.5f,1.f));
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(110.f,0.f, 50.f,1.f));

	return S_OK;
}

void CPlayer::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (m_iHp <= 0)
		m_iHp = 0;

	if (m_iHp > m_iMaxHp)
		m_iHp = 250;

	m_vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	if (m_iShield <= 0)
	{
		m_iHp = Get_Hp();
		m_bShieldZero = true;

		if (m_iShield <= 0)
			m_iShield = 0;
	}
	else if (m_iShield > 0)
	{
		if (m_iShield <= 0)
			m_iShield = 0;
	}
	
	if (m_bShieldZero && !m_bAttacked)
	{
		m_fShieldRecoveryTimer += fTimeDelta;
		if (m_fShieldRecoveryTimer > 10.f)
		{
			m_fShieldTimer += fTimeDelta;
			m_bShieldRecovery = true;
			if (m_fShieldTimer >= 1.f && m_bShieldRecovery)
			{
				if (m_iShield < 150)
					m_iShield += 5;

				m_fShieldTimer = 0.f;
			}
		}
	}

	if (m_iShield >= 150)
	{
		m_iShield = 150;
		m_bShieldRecovery = false;
		m_bShieldZero = false;
		m_fShieldRecoveryTimer = 0.f;
	}

	if (m_bAttacked)
	{
		m_fShieldTimer = 0.f;
		m_bShieldRecovery = false;

		m_fDestroyUI += fTimeDelta;
		if (m_fDestroyUI > 0.4f)
		{
			if (m_iShield > 0)
			{
				if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_ShieldUI"),
					TEXT("Prototype_GameObject_ShieldUI"))))
					return;
			}
			else
			{
				if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_AttackedUI"),
					TEXT("Prototype_GameObject_AttackedUI"))))
					return;
			}

			m_fDestroyUI = 0.f;
			m_bAttacked = false;
		}
	}

	m_pModelCom->Play_Animation(fTimeDelta);

	Key_State(fTimeDelta);

	if (m_pColliderCom[COLLIDER_AABB]->Collision((CCollider*)pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_TunnelDoor"), TEXT("Com_AABB")))||
		m_pColliderCom[COLLIDER_AABB]->Collision((CCollider*)pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_DropzoneWall"), TEXT("Com_AABB"))))
	{
		if (m_MoveState == MOVE_FOWARD)
			m_pTransformCom->Go_Backward(fTimeDelta, m_pNavigationCom);
		if (m_MoveState == MOVE_BACKWARD)
			m_pTransformCom->Go_Straight(fTimeDelta, m_pNavigationCom);
		if (m_MoveState == MOVE_RIGHT)
			m_pTransformCom->Go_Left(fTimeDelta, m_pNavigationCom);
		if (m_MoveState == MOVE_LEFT)
			m_pTransformCom->Go_Right(fTimeDelta, m_pNavigationCom);
	}

	/*m_fTimer += fTimeDelta;

	if (m_fTimer >= 1.f && m_iHp >=0)
	{
		m_iHp -= Get_Demage();
		m_fTimer = 0.f;
	}*/

	/*m_pObject = pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_HealthVial"));
	if (m_pObject->Get_Dead())
		m_iHp += m_pObject->Get_HealthVial();*/

	CTransform* pTerrainTransform = (CTransform*)pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_BackGround"), TEXT("Com_Transform"));
	CVIBuffer_Terrain* pTerrainVIBuffer = (CVIBuffer_Terrain*)pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_BackGround"), TEXT("Com_VIBuffer"));

	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		__super::SetUp_OnTerrain(m_pTransformCom->Get_State(CTransform::STATE_POSITION), pTerrainVIBuffer, pTerrainTransform));

	Safe_Release(pGameInstance);

	for (auto& Pair : m_Parts)
		Pair.second->Tick(fTimeDelta);

	for (size_t i = 0; i < COLLIDER_END; i++)
	{
		if (nullptr != m_pColliderCom[i])
			m_pColliderCom[i]->Tick(m_pTransformCom->Get_WorldMatrix_Float4x4());
	}
}

void CPlayer::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	//Collision_Object();

	for (auto& Pair : m_Parts)
		Pair.second->Late_Tick(fTimeDelta);

	if (/* 화면에 그려진다면 */1)
	{
		for (auto& Pair : m_Parts)
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, Pair.second);

		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);

//#ifdef _DEBUG
//		for (size_t i = 0; i < COLLIDER_END; i++)
//			m_pRendererCom->Add_DebugComponent(m_pColliderCom[i]);
//
//		m_pRendererCom->Add_DebugComponent(m_pNavigationCom);
//#endif
	}
}

HRESULT CPlayer::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;
	
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_ShaderResourceView(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
			return E_FAIL;

		m_pShaderCom->Begin(0);

		m_pModelCom->Render(i);
	}

//#ifdef _DEBUG
//
//	for (size_t i = 0; i < COLLIDER_END; i++)
//	{
//		if (nullptr != m_pColliderCom[i])
//			m_pColliderCom[i]->Render();
//	}
//
//	m_pNavigationCom->Render();
//
//#endif
	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CPlayer::Render_ShadowDepth()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	Safe_Release(pGameInstance);

	return S_OK;
}

void CPlayer::Key_State(_float fTime)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	_long	MouseMove = 0;

	if (MouseMove = pGameInstance->Get_DIMouseMove(CInput_Device::DIMM_X))
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTime * MouseMove * 0.2f);

	if (MouseMove = pGameInstance->Get_DIMouseMove(CInput_Device::DIMM_Y))
	{
		m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), fTime * MouseMove * 0.1f);
	}

	if (KEYPRESSING('A'))
	{
		Walk_Sound(fTime);

		m_pModelCom->SetUp_Animation(36);
		m_pTransformCom->Go_Left(fTime, m_pNavigationCom);
		m_MoveState = MOVE_LEFT;
	}
	else if (KEYPRESSING('D'))
	{
		Walk_Sound(fTime);

		m_pModelCom->SetUp_Animation(36);
		m_pTransformCom->Go_Right(fTime, m_pNavigationCom);
		m_MoveState = MOVE_RIGHT;

	}
	else if (KEYPRESSING('S'))
	{
		Walk_Sound(fTime);

		m_pModelCom->SetUp_Animation(36);
		m_pTransformCom->Go_Backward(fTime, m_pNavigationCom);
		m_MoveState = MOVE_BACKWARD;

	}
	else if (KEYPRESSING('W'))
	{
		Walk_Sound(fTime);

		m_pModelCom->SetUp_Animation(36);
		m_pTransformCom->Go_Straight(fTime, m_pNavigationCom);
		m_MoveState = MOVE_FOWARD;

	}
	else
	{
		m_pModelCom->SetUp_Animation(9);
	}

	if (pGameInstance->Get_DIKeyState(DIK_LSHIFT) & 0x8000)
	{
		m_bRun = true;

		m_pTransformCom->Set_Speed(15.f);
		//m_pModelCom->SetUp_Animation(43);
		m_pModelCom->SetUp_Animation(35);
	}
	else
	{
		m_bRun = false;

		m_pTransformCom->Set_Speed(5.f);
	}


	if (pGameInstance->Get_DIMouseState(CInput_Device::DIMK_LBUTTON) & 0x8000)
	{
		m_pModelCom->SetUp_Animation(9);

		m_fBulletTimer += fTime;
		if (m_fBulletTimer >= 0.3f)
		{
			g_BulletCount--;
			m_fBulletTimer = 0.f;
		}
		if (g_BulletCount <= 0)
			g_BulletCount = 0;

		if (g_BulletCount > 0)
		{
			m_fSoundTimer += fTime;
			if (m_fSoundTimer > 0.2f)
			{
				//CSoundMgr::Get_Instance()->PlaySoundW(L"SMG_Shot.mp3", 0.7f);

				m_fSoundTimer = 0.f;
			}
		}
	}

	if (pGameInstance->Get_DIMouseState(CInput_Device::DIMK_RBUTTON) & 0x8000)
	{
		if (FAILED(pGameInstance->Add_GameObject(g_CurLevel, TEXT("Layer_Zoom"),
			TEXT("Prototype_GameObject_Zoom"))))
			return;

		m_bZoom = true;
	}
	else
		m_bZoom = false;

	if (KEYDOWN('R'))
	{
		CSoundMgr::Get_Instance()->PlaySoundW(L"SMG_Reload.mp3", 1.f);

		//m_pModelCom->Reset_Animation(fTime,true);
		//24
		m_pModelCom->SetUp_Animation(18, true);
		if (g_AmountOfBullet > 0)
		{
			g_BulletCount = 50;
			g_AmountOfBullet -= 50;
		}
	}

	Safe_Release(pGameInstance);
}

void CPlayer::Walk_Sound(_float fTimeDelta)
{
	if (!m_bRun)
	{
		m_fTimer += fTimeDelta;
		if (m_fTimer > 0.8f)
		{
			CSoundMgr::Get_Instance()->PlaySoundW(L"Step.mp3", 0.8f);
			m_fTimer = 0.f;
		}
	}
	else
	{
		m_fTimer += fTimeDelta;
		if (m_fTimer > 0.4f)
		{
			CSoundMgr::Get_Instance()->PlaySoundW(L"Step.mp3", 0.8f);
			m_fTimer = 0.f;
		}
	}

}

HRESULT CPlayer::Add_Components()
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
	if (FAILED(__super::Add_Component(g_CurLevel, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* Com_Model */
	if (FAILED(__super::Add_Component(g_CurLevel, TEXT("Prototype_Component_Model_Player"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;	

	/* Com_AABB */
	CCollider::COLLIDERDESC		ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof ColliderDesc);

	ColliderDesc.vSize = _float3(2.f, 1.3f, 2.f);
	ColliderDesc.vPosition = _float3(0.f, ColliderDesc.vSize.y * 0.5f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_AABB"), (CComponent**)&m_pColliderCom[COLLIDER_AABB], &ColliderDesc)))
		return E_FAIL;

	/* Com_OBB */
	ZeroMemory(&ColliderDesc, sizeof ColliderDesc);

	ColliderDesc.vSize = _float3(2.f, 7.f, 2.f);
	ColliderDesc.vPosition = _float3(0.f, ColliderDesc.vSize.y * 0.5f, 0.f);
	ColliderDesc.vRotation = _float3(0.0f, XMConvertToRadians(45.0f), 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_OBB"), (CComponent**)&m_pColliderCom[COLLIDER_OBB], &ColliderDesc)))
		return E_FAIL;

	/* Com_SPHERE */
	ZeroMemory(&ColliderDesc, sizeof ColliderDesc);

	ColliderDesc.fRadius = 3.f;
	ColliderDesc.vPosition = _float3(0.f, ColliderDesc.fRadius, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_SPHERE"), (CComponent**)&m_pColliderCom[COLLIDER_SPHERE], &ColliderDesc)))
		return E_FAIL;

	/* Com_Navigation */
	CNavigation::NAVIDESC		NaviDesc;

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CTransform* pTerrainTransform = (CTransform*)pGameInstance->Get_Component(g_CurLevel, TEXT("Layer_BackGround"), TEXT("Com_Transform"));

	NaviDesc.pWorldMatrix = pTerrainTransform->Get_WorldMatrixPtr();

	Safe_Release(pGameInstance);

	NaviDesc.iCurrentIndex = 0;
	//NaviDesc.iCurrentIndex = 72;

	if (FAILED(__super::Add_Component(g_CurLevel, TEXT("Prototype_Component_Navigation"),
		TEXT("Com_Navigation"), (CComponent**)&m_pNavigationCom, &NaviDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer::Add_PlayerParts()
{
	CGameObject* pPartObject = nullptr;
	CGameObject* pPartCamera = nullptr;

/*무기*/
	CPistol::WEAPONDESC			PistolDesc;
	PistolDesc.pBoneMatrix = m_pModelCom->Get_BoneCombinedMatrix("R_Weapon_Bone");
	PistolDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	PistolDesc.PivotMatrix = m_pModelCom->Get_PivotMatrix();

#pragma region 카메라
/*카메라*/
	CCamera_OnBone::CAMERAONBONEDESC CameraDesc;

	CameraDesc.pBoneMatrix = m_pModelCom->Get_BoneCombinedMatrix("R_Weapon_Bone");
	CameraDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	CameraDesc.PivotMatrix = m_pModelCom->Get_PivotMatrix();

	CameraDesc.CameraDesc.iLevelIndex = LEVEL_STATIC;
	CameraDesc.CameraDesc.strTranformTag = TEXT("Prototype_Component_Transform");
	// 위치
	CameraDesc.CameraDesc.vEye = _float4(0.f, 1.f, -7.f, 1.f);
	// 보는방향
	CameraDesc.CameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	// 시야각
	CameraDesc.CameraDesc.fFovy = XMConvertToRadians(60.0f);
	// 투영행렬의 Aspect (해상도에 따른 크기변화 반영)
	CameraDesc.CameraDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	// 가장 가까운 거리
	CameraDesc.CameraDesc.fNear = 0.2f;
	// 가장 먼 거리
	CameraDesc.CameraDesc.fFar = 300.f;
	// 초당 이동 속도
	CameraDesc.CameraDesc.TransformDesc.fSpeedPerSec = 10.0f;
	// 초당 회전 속도
	CameraDesc.CameraDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

#pragma endregion 

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	pPartObject = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Pistol"), &PistolDesc);
	
	pPartCamera = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Camera_OnBone"), &CameraDesc);

	if (nullptr == pPartObject)
		return E_FAIL;
	if (nullptr == pPartCamera)
		return E_FAIL;

	/* 중복태그에 대한 검사를 해^^ */
	/* Part_Weapon */
	m_Parts.emplace(TEXT("Part_Weapon"), pPartObject);
	m_Parts.emplace(TEXT("Part_Camera"), pPartCamera);

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CPlayer::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	/*if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", &pGameInstance->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;*/

	/*const LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(0);
	if (nullptr == pLightDesc)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDir", &pLightDesc->vDirection, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4))))
		return E_FAIL;*/

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CPlayer::Collision_Object()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (m_pColliderCom[COLLIDER_OBB]->Collision((CCollider*)pGameInstance->Get_Component(g_CurLevel, TEXT("Layer_TunnelDoor"), TEXT("Com_AABB")))||
		m_pColliderCom[COLLIDER_OBB]->Collision((CCollider*)pGameInstance->Get_Component(g_CurLevel, TEXT("Layer_DropzoneWall"), TEXT("Com_AABB")))) {
		m_bCollision = true;
	}

	Safe_Release(pGameInstance);

	return S_OK;
}

CPlayer * CPlayer::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPlayer*	pInstance = new CPlayer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Created : CPlayer");
	}

	return pInstance;
}

CGameObject * CPlayer::Clone(void * pArg)
{
	CPlayer*	pInstance = new CPlayer(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Cloned : CPlayer");
	}

	return pInstance;
}

void CPlayer::Free()
{
	__super::Free();

	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);

	for (auto& Pair : m_Parts)
		Safe_Release(Pair.second);

	m_Parts.clear();

	for (size_t i = 0; i < COLLIDER_END; i++)
		Safe_Release(m_pColliderCom[i]);
}
