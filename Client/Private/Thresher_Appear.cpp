#include "stdafx.h"
#include "Thresher_Appear.h"

#include "GameInstance.h"
#include "SoundMgr.h"

#include "Player.h"

HRESULT CThresher_Appear::Enter(CThresher* thresher)
{
    m_pThresher = thresher;

    CGameInstance* pGameInstance = CGameInstance::Get_Instance();
    Safe_AddRef(pGameInstance);

    m_pTransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Thresher"), TEXT("Com_Transform")));
    m_pPlayerTransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_Transform")));

    random_device		RandomDevice;
    mt19937_64			RandomNumber;
    uniform_real_distribution<float>		RandomX;
    uniform_real_distribution<float>		RandomZ;

    RandomNumber = mt19937_64(RandomDevice());

    RandomX = uniform_real_distribution<float>(70.f, 115.f);
    RandomZ = uniform_real_distribution<float>(10.f, 105.f);

    //m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(RandomX(RandomNumber), 0.f, RandomZ(RandomNumber), 1.f));
    _vector vPlayerPos = m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION);
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(vPlayerPos.m128_f32[0]+1.f, vPlayerPos.m128_f32[1], vPlayerPos.m128_f32[2] -1.f, 1.f));


    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> Sound(1, 3);
    _int iRandom = Sound(gen);

    if (iRandom == 1)
        CSoundMgr::Get_Instance()->PlaySoundW(L"c4100_bomb.mp3", 1.f);
    else if (iRandom == 2)
        CSoundMgr::Get_Instance()->PlaySoundW(L"kerberos_attack_icebreath_04_voice.ogg", 1.f);
    else if (iRandom == 3)
        CSoundMgr::Get_Instance()->PlaySoundW(L"Loar.wav", 1.f);

    Safe_Release(pGameInstance);

    return S_OK;
}

void CThresher_Appear::Tick(_float fTimeDelta)
{
    CGameInstance* pGameInstance = CGameInstance::Get_Instance();
    Safe_AddRef(pGameInstance);

    m_pModelCom = dynamic_cast<CModel*>(pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Thresher"), TEXT("Com_Model")));
    m_pTransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Thresher"), TEXT("Com_Transform")));
    m_pPlayerTransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_Transform")));
    m_pNavigationCom = dynamic_cast<CNavigation*>(pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Thresher"), TEXT("Com_Navigation")));
    m_pTransformCom->LookAt(m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION));
    m_pObject = pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Player"));

    m_pModelCom->Play_Animation(fTimeDelta);

    m_pModelCom->SetUp_Animation(12, true);

    m_fTimer += fTimeDelta;

    if (!m_bAnimDone && m_fTimer > 0.1f)
    {
        _ulong		dwByte = 0;
        HANDLE		hFile = CreateFile(TEXT("../../Client/Bin/Resources/Effect/AppearRock.txt"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
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

            XMStoreFloat4(&Desc.vPivotPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
            Desc.vPivotPos.x = m_pTransformCom->Get_State(CTransform::STATE_POSITION).m128_f32[0];
            Desc.vPivotPos.z = m_pTransformCom->Get_State(CTransform::STATE_POSITION).m128_f32[2];
            Desc.vPivotPos.y = 0.f;
            if (FAILED(pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Layer_Effect_Rock"),
                TEXT("Prototype_GameObject_AppearRock"), &Desc)))
                return;

        }
        CloseHandle(hFile);
        m_bAnimDone = true;
    }
    g_bCamShake = true;

    if (m_pModelCom->Get_AnimEnd())
    {
        m_pModelCom->Set_AnimEnd(false);
        m_pModelCom->Stop_Animation(fTimeDelta, true);

        if (m_pModelCom->Get_Animation()[12]->Get_AnimEnd())
        {
            g_bCamShake = false;
            m_pModelCom->Get_Animation()[12]->Set_AnimEnd(false);

            m_pThresher->ChangeState(CThresher::STATE_ATTACK);
        }
    }

    Safe_Release(pGameInstance);
}

void CThresher_Appear::Exit(CThresher* thresher)
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
