#include "Tool_Object.h"
#include "GameInstance.h"
#include "Imgui_Manager.h"
#include "Tool_ObjectManager.h"

CTool_Object::CTool_Object(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject(pDevice, pContext)
{
}

CTool_Object::CTool_Object(const CTool_Object& rhs)
    : CGameObject(rhs)
{
}

HRESULT CTool_Object::Initialize_Prototype()
{

    return S_OK;
}

HRESULT CTool_Object::Initialize(void* pArg)
{
    return S_OK;
}

void CTool_Object::Tick(_float fTimeDelta)
{
    __super::Tick(fTimeDelta);



}

void CTool_Object::Late_Tick(_float fTimeDelta)
{
    __super::Late_Tick(fTimeDelta);

}

HRESULT CTool_Object::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    return S_OK;
}

_matrix CTool_Object::Get_WorldMatrix()
{
    return m_pTransformCom->Get_WorldMatrix_Float4x4();
}

void CTool_Object::Free()
{
    __super::Free();

    CImgui_Manager::Destroy_Instance();
    Safe_Release(m_pImgui_Manager);

    CTool_ObjectManager::Destroy_Instance();
    Safe_Release(m_pObject_Manager);

    Safe_Release(m_pTransformCom);
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pRendererCom);
    Safe_Release(m_pModelCom);
}
