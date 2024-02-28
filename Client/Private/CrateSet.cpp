#include "stdafx.h"
#include "CrateSet.h"

#include "GameInstance.h"

CCrateSet::CCrateSet(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject(pDevice, pContext)
{
}

CCrateSet::CCrateSet(const CCrateSet& rhs)
    : CGameObject(rhs)
{
}

HRESULT CCrateSet::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CCrateSet::Initialize(void* pArg)
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

void CCrateSet::Tick(_float fTimeDelta)
{
    __super::Tick(fTimeDelta);
}

void CCrateSet::Late_Tick(_float fTimeDelta)
{
    __super::Late_Tick(fTimeDelta);

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (true == pGameInstance->isInFrustum_InWorld(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 10.f))
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
	}

	Safe_Release(pGameInstance);

}

HRESULT CCrateSet::Render()
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
    return S_OK;
}

HRESULT CCrateSet::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_CrateSet"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CCrateSet::Bind_ShaderResources()
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
CCrateSet* CCrateSet::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCrateSet* pInstance = new CCrateSet(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Created : CCrateSet");
	}

	return pInstance;
}

CGameObject* CCrateSet::Clone(void* pArg)
{
	CCrateSet* pInstance = new CCrateSet(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Cloned : CCrateSet");
	}

	return pInstance;
}

void CCrateSet::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
}
