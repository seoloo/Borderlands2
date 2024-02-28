#include "stdafx.h"
#include "DropzoneWall.h"

#include "GameInstance.h"
#include "Player.h"
#include "SoundMgr.h"
CDropzoneWall::CDropzoneWall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject(pDevice, pContext)
{
}

CDropzoneWall::CDropzoneWall(const CDropzoneWall& rhs)
    : CGameObject(rhs)
{
}

HRESULT CDropzoneWall::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CDropzoneWall::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Add_Components()))
        return E_FAIL;

	OBJECTINFO* Desc = (OBJECTINFO*)pArg;

	m_vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	m_vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
	m_vUp = m_pTransformCom->Get_State(CTransform::STATE_UP);

	m_strName = Desc->strName;
	m_vPos = Desc->CreatePos;
	m_fSize = Desc->ObjectSize;
	m_fRadian = Desc->fRadian;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vPos);
	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, XMVector3Normalize(m_vRight) * m_fSize.x);
	m_pTransformCom->Set_State(CTransform::STATE_UP, XMVector3Normalize(m_vUp) * m_fSize.y);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMVector3Normalize(m_vLook) * m_fSize.z);
	m_pTransformCom->Rotation(m_vUp, m_fRadian);

    return S_OK;
}

void CDropzoneWall::Tick(_float fTimeDelta)
{
    __super::Tick(fTimeDelta);
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	m_pPlayerTransform = dynamic_cast<CTransform*>(pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_Transform")));
	m_pPlayer = pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Player"));


	if (m_bCollision && !m_bCheck)
	{
		if (KEYDOWN('G'))
		{
			CSoundMgr::Get_Instance()->StopAll();
			CSoundMgr::Get_Instance()->PlaySoundW(L"o1201open.mp3", 1.f);

			m_bOpen = true;
			g_bSceondMissionClear = true;
		}
	}

	if (m_bOpen)
	{

		_float IncreasePos = 0.1f;
		m_vPos.m128_f32[1] += IncreasePos;

		if (m_vPos.m128_f32[1] >= 10.f)
			m_bOpen = false;
	}

	if (!m_bCheck)
	{
		if (m_pPlayerTransform->Get_State(CTransform::STATE_POSITION).m128_f32[0] >= m_pTransformCom->Get_State(CTransform::STATE_POSITION).m128_f32[0])
		{
			if (m_iCounter == 0)
			{
				CSoundMgr::Get_Instance()->PlaySoundW(L"o1201open.mp3", 0.7f);
				m_iCounter++;
			}
			m_bClose = true;
		}
	}

	if (m_bClose)
	{

		_float DecreasePos = 0.05f;
		m_vPos.m128_f32[1] -= DecreasePos;

		if (m_vPos.m128_f32[1] <= -0.3f)
		{
			m_bClose = false;

			m_bCheck = true;
			g_bSecondDoorOpened = true;
		}
	}

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vPos);

	Safe_Release(pGameInstance);

	for (size_t i = 0; i < COLLIDER_END; i++)
	{
		if (nullptr != m_pColliderCom[i])
			m_pColliderCom[i]->Tick(m_pTransformCom->Get_WorldMatrix_Float4x4());
	}
}

void CDropzoneWall::Late_Tick(_float fTimeDelta)
{
    __super::Late_Tick(fTimeDelta);
	Collision_Object();

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);

}

HRESULT CDropzoneWall::Render()
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

HRESULT CDropzoneWall::Add_Components()
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

	/* Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_DropzoneWall"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* Com_AABB */
	CCollider::COLLIDERDESC		ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof ColliderDesc);

	ColliderDesc.vSize = _float3(7.f, 10.f, 25.f);
	ColliderDesc.vPosition = _float3(0.f, ColliderDesc.vSize.y * 0.5f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_AABB"), (CComponent**)&m_pColliderCom[COLLIDER_AABB], &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CDropzoneWall::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}
HRESULT CDropzoneWall::Collision_Object()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	Safe_AddRef(pGameInstance);

	if (m_pColliderCom[COLLIDER_AABB]->Collision((CCollider*)pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_SPHERE"))))
	{
		m_bCollision = true;
	}

	Safe_Release(pGameInstance);

	return S_OK;
}
CDropzoneWall* CDropzoneWall::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDropzoneWall* pInstance = new CDropzoneWall(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Created : CDropzoneWall");
	}

	return pInstance;
}

CGameObject* CDropzoneWall::Clone(void* pArg)
{
	CDropzoneWall* pInstance = new CDropzoneWall(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Cloned : CDropzoneWall");
	}

	return pInstance;
}

void CDropzoneWall::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);

	for (size_t i = 0; i < COLLIDER_END; i++)
		Safe_Release(m_pColliderCom[i]);
}
