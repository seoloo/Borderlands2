#include "stdafx.h"
#include "SpiderAnt_Attack.h"

#include "SpiderAnt_State.h"
#include "SpiderAnt.h"

#include "GameInstance.h"
#include "SoundMgr.h"

HRESULT CSpiderAnt_Attack::Enter(CSpiderAnt* spiderant)
{
    CGameInstance* pGameInstance = CGameInstance::Get_Instance();
    Safe_AddRef(pGameInstance);

   
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> Attack(1, 2);
    _int iAttack = Attack(gen);

    if (iAttack == 1)
        m_iAttackAnim = 15;
    else if (iAttack == 2)
        m_iAttackAnim = 16;
    else if (iAttack == 3)
        m_iAttackAnim = 17;

    Safe_Release(pGameInstance);
    m_pSpiderAnt = spiderant;

    return S_OK;
}

void CSpiderAnt_Attack::Tick(_float fTimeDelta)
{
    CGameInstance* pGameInstance = CGameInstance::Get_Instance();
    Safe_AddRef(pGameInstance);

    m_pModelCom = dynamic_cast<CModel*>(pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_SpiderAnt"), TEXT("Com_Model")));
    m_pTransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_SpiderAnt"), TEXT("Com_Transform")));
    m_pPlayerTransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_Transform")));
    m_pObject = pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_SpiderAnt"));

    m_fTimer += fTimeDelta;
    if (m_fTimer >= 0.5f)
    {
        CSoundMgr::Get_Instance()->PlaySoundW(L"c2320_groundpunch.mp3", 1.f);

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
            _vector TempPos = m_pTransformCom->Get_State(CTransform::STATE_LOOK) + m_pTransformCom->Get_State(CTransform::STATE_POSITION);

            XMStoreFloat4(&Desc.vPivotPos, TempPos);

            Desc.bDig = false;
            Desc.bFog = true;
            Desc.bGray = true;
            Desc.fMinSpeed = 25.f;
            Desc.fMaxSpeed = 50.f;

            if (FAILED(pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Layer_Effect_Explosion"),
                TEXT("Prototype_GameObject_Dust"), &Desc)))
                return;


        }
        CloseHandle(hFile);
        m_fTimer = -1.f;
    }

    m_pModelCom->Play_Animation(fTimeDelta);

    m_pModelCom->SetUp_Animation(m_iAttackAnim);

    m_pModelCom->Set_AnimEnd(false);

    if (m_pModelCom->Get_Animation()[m_iAttackAnim]->Get_AnimEnd())
    {
        dynamic_cast<CSpiderAnt*>(m_pObject)->Set_Collided(false);
        m_pModelCom->Get_Animation()[m_iAttackAnim]->Set_AnimEnd(false);

        m_pSpiderAnt->Change_State(CSpiderAnt::STATE_WALK);
    }

    Safe_Release(pGameInstance);
}

void CSpiderAnt_Attack::Exit(CSpiderAnt* spiderant)
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
