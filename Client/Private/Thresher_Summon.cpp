#include "stdafx.h"
#include "Thresher_Summon.h"

#include "GameInstance.h"
#include "SoundMgr.h"

#include "Thresher_Arm.h"
#include "Thresher_Arm2.h"
#include "Thresher_Melee.h"

#include "Player.h"

HRESULT CThresher_Summon::Enter(CThresher* thresher)
{
    m_pThresher = thresher;

    g_bThersherSummon = true;

    CGameInstance* pGameInstance = CGameInstance::Get_Instance();
    Safe_AddRef(pGameInstance);

    m_pTransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Thresher"), TEXT("Com_Transform")));

    random_device		RandomDevice;
    mt19937_64			RandomNumber;
    uniform_real_distribution<float>		RandomX;
    uniform_real_distribution<float>		RandomX2;
    uniform_real_distribution<float>		RandomX3;
    uniform_real_distribution<float>		RandomZ;
    uniform_real_distribution<float>		RandomZ2;
    uniform_real_distribution<float>		RandomZ3;

    RandomNumber = mt19937_64(RandomDevice());

    RandomX = uniform_real_distribution<float>(70.f, 115.f);
    RandomX2 = uniform_real_distribution<float>(70.f, 115.f);
    RandomX3 = uniform_real_distribution<float>(70.f, 115.f);
    RandomZ = uniform_real_distribution<float>(10.f, 80.f);
    RandomZ2 = uniform_real_distribution<float>(10.f, 80.f);
    RandomZ3 = uniform_real_distribution<float>(10.f, 80.f);

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(90.f, 0.f, 100.f, 1.f));

    for (int i = 1; i < 4; i++)
    {
        CThresher_Arm::ArmDesc Desc;
        Desc.CreatePos = XMVectorSet(RandomX(RandomNumber), -2.f, RandomZ(RandomNumber), 1.f);
        Desc.Numbering = i;
        if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_ThresherArm"),
            TEXT("Prototype_GameObject_ThresherArm"), &Desc)))
            return E_FAIL;

        CThresher_Arm2::ArmDesc Desc2;
        Desc2.CreatePos = XMVectorSet(RandomX2(RandomNumber), -2.f, RandomZ2(RandomNumber), 1.f);
        Desc2.Numbering = i;
        if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_ThresherArm2"),
            TEXT("Prototype_GameObject_ThresherArm2"), &Desc2)))
            return E_FAIL;

        CThresher_Melee::ArmDesc Desc3;
        Desc3.CreatePos = XMVectorSet(RandomX3(RandomNumber), -2.f, RandomZ3(RandomNumber), 1.f);
        Desc3.Numbering = i;
        if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_ThresherMelee"),
            TEXT("Prototype_GameObject_ThresherMelee"), &Desc3)))
            return E_FAIL;
    }
    

    Safe_Release(pGameInstance);

    return S_OK;
}

void CThresher_Summon::Tick(_float fTimeDelta)
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

    m_fAttackTimer += fTimeDelta;
    if (m_fAttackTimer > 1.5f)
    {
        CSoundMgr::Get_Instance()->PlaySoundW(L"BOSS_ChargeLast.ogg", 1.f);
        m_fAttackTimer = 0.f;
    }

    m_fBlastTimer += fTimeDelta;
    if (m_fBlastTimer > 0.3f)
    {
        m_pObject->Decrease_Hp(1);

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

            random_device		RandomDevice;
            mt19937_64			RandomNumber;
            uniform_real_distribution<float>		RandomX;
            uniform_real_distribution<float>		RandomZ;

            RandomNumber = mt19937_64(RandomDevice());

            /*RandomX = uniform_real_distribution<float>(10.f, 30.f);
            RandomZ = uniform_real_distribution<float>(10.f, 50.f);*/

            RandomX = uniform_real_distribution<float>(70.f, 115.f);
            RandomZ = uniform_real_distribution<float>(10.f, 80.f);

            _vector TempPos = XMVectorSet(RandomX(RandomNumber), 0.f, RandomZ(RandomNumber), 1.f);

            XMStoreFloat4(&Desc.vPivotPos, TempPos);

            for (int i = 0; i < 5; ++i)
            {
                if (FAILED(pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Layer_Effect_Blast"),
                    TEXT("Prototype_GameObject_Blast"), &Desc)))
                    return;

                if (FAILED(pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Layer_Effect_Blast"),
                    TEXT("Prototype_GameObject_Blast"), &Desc)))
                    return;
            }

        }
        CloseHandle(hFile);
        m_fBlastTimer = 0.f;
    }

    //9
    m_pModelCom->SetUp_Animation(41);

    if (m_pModelCom->Get_AnimEnd())
    {
        m_pModelCom->Set_AnimEnd(false);
    }
    if (m_pModelCom->Get_Animation()[41]->Get_AnimEnd())
    {
        m_pModelCom->Get_Animation()[41]->Set_AnimEnd(true);
    }


    if (m_pThresher->Get_ArmDead() && m_pThresher->Get_Arm2Dead() && m_pThresher->Get_MeleeDead())
    {
        g_bCamShake = false;
        m_pModelCom->Get_Animation()[41]->Set_AnimEnd(true);
        g_ArmNumber = g_Arm2Number = g_MeleeNumber = 0;
        m_pThresher->ChangeState(CThresher::STATE_DEAD);
    }

    //42 -> 41

    /*m_pModelCom->SetUp_Animation(41);

    if (m_pThresher->Get_ArmDead() && m_pThresher->Get_Arm2Dead()&& m_pThresher->Get_MeleeDead())
    {
        m_pModelCom->Get_Animation()[41]->Set_AnimEnd(true);
        m_pThresher->ChangeState(CThresher::STATE_DEAD);
    }*/

    Safe_Release(pGameInstance);
}

void CThresher_Summon::Exit(CThresher* thresher)
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
