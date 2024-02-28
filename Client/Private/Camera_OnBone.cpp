#include "stdafx.h"
#include "Camera_OnBone.h"

#include "GameInstance.h"

#include "Player.h"
#include "TownPlayer.h"

XMVECTOR g_CameraPos;

CCamera_OnBone::CCamera_OnBone(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CCamera(pDevice, pContext)
{
}

CCamera_OnBone::CCamera_OnBone(const CCamera_OnBone& rhs)
    : CCamera(rhs)
{
}

HRESULT CCamera_OnBone::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CCamera_OnBone::Initialize(void* pArg)
{
    CAMERAONBONEDESC* pCameraDesc = (CAMERAONBONEDESC*)pArg;

    /* 나에게 필요한 정보를 셋한다. */
    if (FAILED(__super::Initialize(&pCameraDesc->CameraDesc)))
        return E_FAIL;

    m_pBoneMatrix = pCameraDesc->pBoneMatrix;
    m_pParentMatrix = pCameraDesc->pParentMatrix;
    m_PivotMatrix = pCameraDesc->PivotMatrix;

    m_pTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 3.f, 0.f, 1.f));


    return S_OK;
}

void CCamera_OnBone::Tick(_float fTimeDelta)
{
    __super::Tick(fTimeDelta);

    CGameInstance* pGameInstance = CGameInstance::Get_Instance();
    Safe_AddRef(pGameInstance);

    _matrix		BoneMatrix = XMLoadFloat4x4(m_pBoneMatrix);

    BoneMatrix.r[0] = XMVector3Normalize(BoneMatrix.r[0]);
    BoneMatrix.r[1] = XMVector3Normalize(BoneMatrix.r[1]);
    BoneMatrix.r[2] = XMVector3Normalize(BoneMatrix.r[2]);

    XMStoreFloat4x4(&m_WorldMatrix, m_pTransform->Get_WorldMatrix_Float4x4() *
        BoneMatrix *
        XMLoadFloat4x4(&m_PivotMatrix) *
        XMLoadFloat4x4(m_pParentMatrix));

    m_pPlayer = pGameInstance->Get_GameObject(g_CurLevel, L"Layer_Player");

    m_pPlayerTransform = dynamic_cast<CTransform*>(pGameInstance->Get_Component(g_CurLevel, TEXT("Layer_Player"), TEXT("Com_Transform")));

    m_PlayerPos = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);
    m_PlayerUp = m_pPlayerTransform->Get_State(CTransform::STATE_UP);
    m_PlayerLook = m_pPlayerTransform->Get_State(CTransform::STATE_LOOK);
    m_PlayerRight = m_pPlayerTransform->Get_State(CTransform::STATE_RIGHT);


    if (g_bSpiderAntScene)
    {
        m_pObjectTransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_Component(g_CurLevel, TEXT("Layer_SpiderAnt"), TEXT("Com_Transform")));

        m_ObjectPos = m_pObjectTransformCom->Get_State(CTransform::STATE_POSITION);
        m_ObjectUp = m_pObjectTransformCom->Get_State(CTransform::STATE_UP);
        m_ObjectLook = m_pObjectTransformCom->Get_State(CTransform::STATE_LOOK);
        m_ObjectRight = m_pObjectTransformCom->Get_State(CTransform::STATE_RIGHT);

        Moving_Camera(fTimeDelta);

    }

    if (g_bSkagScene)
    {
        m_pObjectTransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_Component(g_CurLevel, TEXT("Layer_Skag"), TEXT("Com_Transform")));

        m_ObjectPos = m_pObjectTransformCom->Get_State(CTransform::STATE_POSITION);
        m_ObjectUp = m_pObjectTransformCom->Get_State(CTransform::STATE_UP);
        m_ObjectLook = m_pObjectTransformCom->Get_State(CTransform::STATE_LOOK);
        m_ObjectRight = m_pObjectTransformCom->Get_State(CTransform::STATE_RIGHT);

        Moving_Camera(fTimeDelta);

        /*m_pTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_ObjectUp.m128_f32[0] + m_ObjectPos.m128_f32[0] + m_ObjectLook.m128_f32[0],
            m_ObjectUp.m128_f32[1] + m_ObjectPos.m128_f32[1] + 3.f, m_ObjectUp.m128_f32[2] + m_ObjectPos.m128_f32[2] + m_ObjectLook.m128_f32[2] - 15.f, m_ObjectPos.m128_f32[3]));*/
    }

    if (g_bThresherScene)
    {
        m_pObjectTransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_Component(g_CurLevel, TEXT("Layer_Thresher"), TEXT("Com_Transform")));

        m_ObjectPos = m_pObjectTransformCom->Get_State(CTransform::STATE_POSITION);
        m_ObjectUp = m_pObjectTransformCom->Get_State(CTransform::STATE_UP);
        m_ObjectLook = m_pObjectTransformCom->Get_State(CTransform::STATE_LOOK);
        m_ObjectRight = m_pObjectTransformCom->Get_State(CTransform::STATE_RIGHT);
        Moving_Camera(fTimeDelta);

        /*m_pTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_ObjectUp.m128_f32[0] + m_ObjectPos.m128_f32[0] + m_ObjectLook.m128_f32[0],
            m_ObjectUp.m128_f32[1] + m_ObjectPos.m128_f32[1] + 15.f, m_ObjectUp.m128_f32[2] + m_ObjectPos.m128_f32[2] + m_ObjectLook.m128_f32[2] - 40.f, m_ObjectPos.m128_f32[3]));*/
    }
    
    if(!g_bSpiderAntScene && !g_bSkagScene && !g_bThresherScene)
    {
        if (15.f == m_pPlayerTransform->Get_Speed())
        {
            m_CameraDesc.fFovy = XMConvertToRadians(52.0f);
        }
        else if (5.f == m_pPlayerTransform->Get_Speed())
        {
            m_CameraDesc.fFovy = XMConvertToRadians(60.0f);
        }

        m_pTransform->Set_State(CTransform::STATE_LOOK, m_PlayerLook);
        m_pTransform->Set_State(CTransform::STATE_UP, m_PlayerUp);
        m_pTransform->Set_State(CTransform::STATE_RIGHT, m_PlayerRight);

        if (pGameInstance->Get_DIMouseState(CInput_Device::DIMK_RBUTTON) & 0x8000)
        {
            m_CameraDesc.fFovy = XMConvertToRadians(40.0f);

            m_pTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_PlayerLook.m128_f32[0] + m_PlayerPos.m128_f32[0],
                m_PlayerUp.m128_f32[1], m_PlayerLook.m128_f32[2] + m_PlayerPos.m128_f32[2], m_PlayerPos.m128_f32[3]));

            m_bPistolCam = false;
        }
        else
        {
            m_pTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_PlayerUp.m128_f32[0] + m_PlayerPos.m128_f32[0],
                m_PlayerUp.m128_f32[1] + m_PlayerPos.m128_f32[1], m_PlayerUp.m128_f32[2] + m_PlayerPos.m128_f32[2], m_PlayerPos.m128_f32[3]));

            m_bPistolCam = true;
        }


        
    }

    random_device		RandomDevice;

    /* 난수 생성 초기화.  */
    m_RandomNumber = mt19937_64(RandomDevice());
    m_RandomFovy = uniform_real_distribution<float>(58.f, 60.f);

    if (pGameInstance->Get_DIMouseState(CInput_Device::DIMK_LBUTTON) & 0x8000 && m_bPistolCam)
    {
        if(g_BulletCount > 0)
            m_CameraDesc.fFovy = XMConvertToRadians(m_RandomFovy(m_RandomNumber));
    }

    m_RandomFovyForShake = uniform_real_distribution<float>(50.f, 60.f);

    if (g_CurLevel == 4)
    {
        if (g_bCamShake)
            m_CameraDesc.fFovy = XMConvertToRadians(m_RandomFovyForShake(m_RandomNumber));
    }

    Safe_Release(pGameInstance);

    __super::Set_Transform();
}

void CCamera_OnBone::Late_Tick(_float fTimeDelta)
{
    __super::Late_Tick(fTimeDelta);
}

void CCamera_OnBone::Moving_Camera(_float fTimeDelta)
{
    _vector TargetPos = m_ObjectPos + m_ObjectLook + m_ObjectUp;

    _vector vCameraPos = m_pTransform->Get_State(CTransform::STATE_POSITION);

    _vector vLength = XMVectorSubtract(TargetPos, vCameraPos);
    _float fLenghX = XMVectorGetX(XMVector3Length(vLength));
    _vector vFinalPos = XMVectorSet(0.f, 0.f, 0.f, 0.f);


    if (m_iCounter == 0)
    {
        /* 새로놓일 카메라의 위치 - 현재 카메라의 위치 ---> 거리를 구한다 */
        m_fTimer += fTimeDelta;
        vFinalPos.m128_f32[0] = vCameraPos.m128_f32[0] + vLength.m128_f32[0] * m_fTimer;
        vFinalPos.m128_f32[1] = vCameraPos.m128_f32[1] + vLength.m128_f32[1] * m_fTimer;
        vFinalPos.m128_f32[2] = vCameraPos.m128_f32[2] + vLength.m128_f32[2] * m_fTimer;
        vFinalPos.m128_f32[3] = 1.f;

        m_vCameraPos = m_pTransform->Get_State(CTransform::STATE_POSITION);

        m_iCounter++;
    }
    else if (m_iCounter == 1)
    {

        if (25.f <= fLenghX)
        {
            m_fTimer += fTimeDelta;
            vFinalPos.m128_f32[0] = vCameraPos.m128_f32[0] + vLength.m128_f32[0] * m_fTimer * 0.01f;
            vFinalPos.m128_f32[1] = vCameraPos.m128_f32[1] + vLength.m128_f32[1] * m_fTimer * 0.01f;
            vFinalPos.m128_f32[2] = vCameraPos.m128_f32[2] + vLength.m128_f32[2] * m_fTimer * 0.01f;
            vFinalPos.m128_f32[3] = 1.f;
            m_pTransform->Set_State(CTransform::STATE_POSITION, vFinalPos);
            m_vCameraPos = m_pTransform->Get_State(CTransform::STATE_POSITION);

        }
        else {
            m_iCounter = 0;
            m_fTimer = 0.f;
            m_vCameraPos = XMVectorSet(0.f, 0.f, 0.f, 0.f);
            m_pTransform->Set_State(CTransform::STATE_POSITION, m_pTransform->Get_State(CTransform::STATE_POSITION));

            m_vCameraPos = m_pTransform->Get_State(CTransform::STATE_POSITION);

            Cut_Scene(fTimeDelta);
        }

    }
    /* 카메라는 NPC 의 포지션을 보게 만든다 */
    m_pTransform->LookAt(TargetPos);
}

void CCamera_OnBone::Cut_Scene(_float fTimeDelta)
{
    if(g_bSpiderAntScene || g_bSkagScene || g_bThresherScene)
        m_pTransform->Set_State(CTransform::STATE_POSITION, m_vCameraPos);

    /*if (g_bSpiderAntScene)
        g_bSpiderAntScene = false;

    if (g_bSkagScene)
        g_bSkagScene = false;

    if (g_bThresherScene)
        g_bThresherScene = false;*/
}


CCamera_OnBone* CCamera_OnBone::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CCamera_OnBone* pInstance = new CCamera_OnBone(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        Safe_Release(pInstance);
        MSG_BOX("Failed to Created : CCamera_OnBone");
    }

    return pInstance;
}

CGameObject* CCamera_OnBone::Clone(void* pArg)
{
    CCamera_OnBone* pInstance = new CCamera_OnBone(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        Safe_Release(pInstance);
        MSG_BOX("Failed to Cloned : CCamera_OnBone");
    }

    return pInstance;
}

void CCamera_OnBone::Free()
{
    __super::Free();

    m_pSpiderAntTransformCom = { nullptr };
    m_pSkagTransformCom = { nullptr };
    m_pObjectTransformCom = { nullptr };
    Safe_Release(m_pObjectTransformCom);
    Safe_Release(m_pSpiderAntTransformCom);
    Safe_Release(m_pSkagTransformCom);

}
