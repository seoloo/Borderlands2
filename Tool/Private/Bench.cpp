#include "Bench.h"
#include "GameInstance.h"
#include "Imgui_Manager.h"
#include "Tool_ObjectManager.h"

CBench::CBench(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CTool_Object(pDevice, pContext)
{
}

CBench::CBench(const CBench& rhs)
    : CTool_Object(rhs)
{
}

HRESULT CBench::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CBench::Initialize(void* pArg)
{
    if (FAILED(Add_Components()))
        return E_FAIL;

    OBJECTDESC* Desc = (OBJECTDESC*)pArg;

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

    m_pImgui_Manager->Get_Instance()->Set_PickingPos(_float3(m_vPos.m128_f32[0], m_vPos.m128_f32[1], m_vPos.m128_f32[2]));

    return S_OK;
}

void CBench::Tick(_float fTimeDelta)
{
    __super::Tick(fTimeDelta);

    _float3 result = {};
    CGameInstance* pGameInstance = CGameInstance::Get_Instance();
    Safe_AddRef(pGameInstance);

    /*if (pGameInstance->Get_DIMouseState(CInput_Device::DIMK_LBUTTON) & 0x8000)
    {
        if (m_pModelCom->Intersect_Polygons(m_pTransformCom->Get_WorldMatrix(), &result))
        {
            m_pImgui_Manager->Get_Instance()->Set_PickTerrain(true);
            int a = 0;
        }
    }*/

    if (true == m_pImgui_Manager->Get_Instance()->Get_Fix()&&
        m_strName == m_pImgui_Manager->Get_Instance()->Get_SelectedName())
    {
        CTool_ObjectManager::OBJECTDESC objDesc;
        ZeroMemory(&objDesc, sizeof(CTool_ObjectManager::OBJECTDESC));

        objDesc.strName = m_strName;
        objDesc.ObjectSize = m_fSize;
        objDesc.CreatePos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
        objDesc.fRadian = m_fRadian;
        objDesc.bSave = false;
        m_pObject_Manager->Get_Instance()->Add_Object(&objDesc);

        m_pImgui_Manager->Get_Instance()->Set_Fix(false);
    }

    Key_Input();

    //Apply_Tool();

    Safe_Release(pGameInstance);
}

void CBench::Late_Tick(_float fTimeDelta)
{
    __super::Late_Tick(fTimeDelta);

    m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CBench::Render()
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

void CBench::Apply_Tool()
{
    const map<wstring, tuple<_float3, _float, _vector>>& ObjectInfoMap = m_pImgui_Manager->Get_Instance()->Get_ObjectMap();

    for (const auto& pair : ObjectInfoMap)
    {
        wstring ObjName = pair.first;
        const tuple<_float3, _float, _vector>& objectInfoTuple = pair.second;

        /* x, y, z 축 길이 조정 */
        m_pTransformCom->Set_State(CTransform::STATE_RIGHT, XMVector3Normalize(m_vRight) * get<0>(objectInfoTuple).x);
        m_pTransformCom->Set_State(CTransform::STATE_UP, XMVector3Normalize(m_vUp) * get<0>(objectInfoTuple).y);
        m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMVector3Normalize(m_vLook) * get<0>(objectInfoTuple).z);

        /* 위치 조정 */
        m_pTransformCom->Set_State(CTransform::STATE_POSITION, get<2>(objectInfoTuple));

        /* y 축 기준 회전 */
        m_pTransformCom->Rotation(m_vUp, get<1>(objectInfoTuple));
    }
    //m_pTransformCom->Set_State(CTransform::STATE_POSITION, g_vObjPos + m_vPos);
}

void CBench::Key_Input()
{
    CGameInstance* pGameInstance = CGameInstance::Get_Instance();
    Safe_AddRef(pGameInstance);
    if (m_strName == m_pImgui_Manager->Get_Instance()->Get_SelectedName())
    {
        if (pGameInstance->Get_DIKeyState(DIK_LSHIFT))
        {
            /* 위치 */
            if (pGameInstance->Get_DIKeyState(DIK_Z))
            {
                m_fPos.y -= 0.1f;
                m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_vPos.m128_f32[0] + m_fPos.x, m_vPos.m128_f32[1] + m_fPos.y, m_vPos.m128_f32[2] + m_fPos.z, 1.f));
            }

            /* 크기 */
            if (pGameInstance->Get_DIKeyState(DIK_X))
            {
                m_fSize.x -= 0.01f;
                m_pTransformCom->Set_State(CTransform::STATE_RIGHT, XMVector3Normalize(m_vRight) * m_fSize.x);
            }
            if (pGameInstance->Get_DIKeyState(DIK_C))
            {
                m_fSize.y -= 0.01f;
                m_pTransformCom->Set_State(CTransform::STATE_UP, XMVector3Normalize(m_vUp) * m_fSize.y);
            }
            if (pGameInstance->Get_DIKeyState(DIK_V))
            {
                m_fSize.z -= 0.01f;
                m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMVector3Normalize(m_vLook) * m_fSize.z);
            }

            /* 회전 */
            if (pGameInstance->Get_DIKeyState(DIK_B))
            {
                m_fRadian -= 0.01f;
                m_pTransformCom->Rotation(m_vUp, m_fRadian);
            }
        }
        else {
            /* 위치 */
            if (pGameInstance->Get_DIKeyState(DIK_RIGHT))
            {
                m_fPos.x += 0.1f;
                m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_vPos.m128_f32[0] + m_fPos.x, m_vPos.m128_f32[1] + m_fPos.y, m_vPos.m128_f32[2] + m_fPos.z, 1.f));
            }
            if (pGameInstance->Get_DIKeyState(DIK_LEFT))
            {
                m_fPos.x -= 0.1f;
                m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_vPos.m128_f32[0] + m_fPos.x, m_vPos.m128_f32[1] + m_fPos.y, m_vPos.m128_f32[2] + m_fPos.z, 1.f));
            }
            if (pGameInstance->Get_DIKeyState(DIK_UP))
            {
                m_fPos.z += 0.1f;
                m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_vPos.m128_f32[0] + m_fPos.x, m_vPos.m128_f32[1] + m_fPos.y, m_vPos.m128_f32[2] + m_fPos.z, 1.f));
            }
            if (pGameInstance->Get_DIKeyState(DIK_DOWN))
            {
                m_fPos.z -= 0.1f;
                m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_vPos.m128_f32[0] + m_fPos.x, m_vPos.m128_f32[1] + m_fPos.y, m_vPos.m128_f32[2] + m_fPos.z, 1.f));
            }
            if (pGameInstance->Get_DIKeyState(DIK_Z))
            {
                m_fPos.y += 0.1f;
                m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_vPos.m128_f32[0] + m_fPos.x, m_vPos.m128_f32[1] + m_fPos.y, m_vPos.m128_f32[2] + m_fPos.z, 1.f));
            }

            /* 크기 */
            if (pGameInstance->Get_DIKeyState(DIK_X))
            {
                m_fSize.x += 0.01f;
                m_pTransformCom->Set_State(CTransform::STATE_RIGHT, XMVector3Normalize(m_vRight) * m_fSize.x);
            }

            if (pGameInstance->Get_DIKeyState(DIK_C))
            {
                m_fSize.y += 0.01f;
                m_pTransformCom->Set_State(CTransform::STATE_UP, XMVector3Normalize(m_vUp) * m_fSize.y);
            }

            if (pGameInstance->Get_DIKeyState(DIK_V))
            {
                m_fSize.z += 0.01f;
                m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMVector3Normalize(m_vLook) * m_fSize.z);
            }

            /* 회전 */
            if (pGameInstance->Get_DIKeyState(DIK_B))
            {
                m_fRadian += 0.01f;
                m_pTransformCom->Rotation(m_vUp, m_fRadian);
            }
        }

        m_pImgui_Manager->Get_Instance()->Set_Pos(XMVectorSet(m_vPos.m128_f32[0], m_vPos.m128_f32[1], m_vPos.m128_f32[2], 1.f));
    }
    Safe_Release(pGameInstance);
}

HRESULT CBench::Add_Components()
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
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Bench"),
        TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
        return E_FAIL;

    return S_OK;
}

HRESULT CBench::Bind_ShaderResources()
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

    const LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(0);
    if (nullptr == pLightDesc)
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDir", &pLightDesc->vDirection, sizeof(_float4))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4))))
        return E_FAIL;

    Safe_Release(pGameInstance);

    return S_OK;
}

CBench* CBench::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CBench* pInstance = new CBench(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        Safe_Release(pInstance);
        MSG_BOX("Failed to Create : CBench");
    }

    return pInstance;
}

CBench* CBench::Clone(void* pArg)
{
    CBench* pInstance = new CBench(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        Safe_Release(pInstance);
        MSG_BOX("Failed to Clone : CBench");
    }

    return pInstance;
}

void CBench::Free()
{
    __super::Free();

    CImgui_Manager::Destroy_Instance();
    Safe_Release(m_pImgui_Manager);

    Safe_Release(m_pTransformCom);
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pRendererCom);
    Safe_Release(m_pModelCom);
}
