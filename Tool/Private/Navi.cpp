#include "Navi.h"

#include "GameInstance.h"

CNavi::CNavi(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CGameObject(pDevice, pContext)
{
}

CNavi::CNavi(const CNavi& rhs)
    : CGameObject(rhs)
{
}

HRESULT CNavi::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

    return S_OK;
}

HRESULT CNavi::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

    return S_OK;
}

void CNavi::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	_ulong		dwByte = 0;
	HANDLE		hFile = CreateFile(TEXT("../../Client/Bin/Resources/Map/NaviTest.txt"), GENERIC_WRITE, 0, nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

	_float3		vPoints[3];
	ZeroMemory(vPoints, sizeof(_float3) * 3);

	vPoints[0] = _float3(0.f, 0.f, 10.f);
	vPoints[1] = _float3(10.f, 0.f, 0.f);
	vPoints[2] = _float3(0.f, 0.f, 0.f);

	WriteFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);

	CloseHandle(hFile);
}

void CNavi::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CNavi::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	/*if (FAILED(Bind_ShaderResources()))
		return E_FAIL;*/

	//m_pShaderCom->Begin(0);

	//m_pVIBufferCom->Render();

#ifdef _DEBUG
	m_pNavigationCom->Render();
#endif

    return S_OK;
}

HRESULT CNavi::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxNorTex"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Terrain"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* Com_Navigation */
	CNavigation::NAVIDESC		NaviDesc;
	NaviDesc.pWorldMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	NaviDesc.iCurrentIndex = -1;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Navigation"),
		TEXT("Com_Navigation"), (CComponent**)&m_pNavigationCom, &NaviDesc)))
		return E_FAIL;
}

HRESULT CNavi::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	/*if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", &pGameInstance->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;*/

	Safe_Release(pGameInstance);

    return S_OK;
}

CNavi* CNavi::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CNavi* pInstance = new CNavi(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Created : CNavi");
	}

	return pInstance;
}

CGameObject* CNavi::Clone(void* pArg)
{
	CNavi* pInstance = new CNavi(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Cloned : CNavi");
	}

	return pInstance;
}

void CNavi::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
}
