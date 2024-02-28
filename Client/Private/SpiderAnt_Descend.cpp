#include "stdafx.h"
#include "SpiderAnt_Descend.h"

#include "SpiderAnt_State.h"
#include "SpiderAnt.h"

#include "GameInstance.h"
#include "SoundMgr.h"

#include "SoundMgr.h"

HRESULT CSpiderAnt_Descend::Enter(CSpiderAnt* spiderant)
{
    CGameInstance* pGameInstance = CGameInstance::Get_Instance();
    Safe_AddRef(pGameInstance);

    Safe_Release(pGameInstance);

    m_pSpiderAnt = spiderant;

    return S_OK;
}

void CSpiderAnt_Descend::Tick(_float fTimeDelta)
{
    CGameInstance* pGameInstance = CGameInstance::Get_Instance();
    Safe_AddRef(pGameInstance);

    g_bSpiderAntScene = true;

    m_pModelCom = dynamic_cast<CModel*>(pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_SpiderAnt"), TEXT("Com_Model")));
    m_pTransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_SpiderAnt"), TEXT("Com_Transform")));

    m_pPlayerTransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_Transform")));
    m_pTransformCom->LookAt(m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION));

    m_pModelCom->Play_Animation(fTimeDelta * 0.9f);

    m_pModelCom->SetUp_Animation(71);

    m_vPlayerPos.x = m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION).m128_f32[0];
    m_vPlayerPos.z = m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION).m128_f32[2];

    m_vPos.x = m_pTransformCom->Get_State(CTransform::STATE_POSITION).m128_f32[0];
    m_vPos.z = m_pTransformCom->Get_State(CTransform::STATE_POSITION).m128_f32[2];

    m_fTimer += fTimeDelta;
    if (m_fTimer > 1.5f)
    {
        _ulong		dwByte = 0;
        HANDLE		hFile = CreateFile(TEXT("../../Client/Bin/Resources/Effect/DigRock.txt"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
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

            //_vector TempPos = XMVectorSet(RandomX(RandomNumber), 0.f, RandomZ(RandomNumber), 1.f);
            _vector TempPos = XMVectorSet(m_vPos.x, 0.5f, m_vPos.z, 1.f);

            XMStoreFloat4(&Desc.vPivotPos, TempPos);

            Desc.bDig = false;
            Desc.bFog = true;
            Desc.bGray = true;
            Desc.fMinSpeed = 25.f;
            Desc.fMaxSpeed = 50.f;
            
            if (FAILED(pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Layer_Effect_Explosion"),
                TEXT("Prototype_GameObject_Dust"), &Desc)))
                return;
            CSoundMgr::Get_Instance()->PlaySoundW(L"Baggi_Land.wav", 1.f);
        }
        CloseHandle(hFile);
        m_fTimer = 0.f;
    }


    if (m_pModelCom->Get_Animation()[71]->Get_AnimEnd())
    {
        m_pModelCom->Get_Animation()[71]->Set_AnimEnd(false);

        m_pSpiderAnt->Change_State(CSpiderAnt::STATE_IDLE);
    }
    
    /*m_fTimer += fTimeDelta;

    if (m_fTimer > 2.f)
    {
        m_fTimer = 0.f;
        m_pModelCom->Stop_Animation(fTimeDelta, true);
        m_pSpiderAnt->Change_State(CSpiderAnt::STATE_IDLE);
    }*/
    Safe_Release(pGameInstance);

}

void CSpiderAnt_Descend::Exit(CSpiderAnt* spiderant)
{
    m_pModelCom = { nullptr };
    m_pPlayerTransformCom = { nullptr };
    m_pTransformCom = { nullptr };
    m_pNavigationCom = { nullptr };
    m_pObject = { nullptr };
    Safe_Release(m_pModelCom);
    Safe_Release(m_pPlayerTransformCom);
    Safe_Release(m_pTransformCom);
    Safe_Release(m_pNavigationCom);
}
