#include "stdafx.h"
#include "SpiderAnt_Shoot.h"

#include "SpiderAnt_State.h"
#include "SpiderAnt.h"
#include "SoundMgr.h"

#include "GameInstance.h"
#include "Monster_Fire.h"
#include "EMPBlue.h"

HRESULT CSpiderAnt_Shoot::Enter(CSpiderAnt* spiderant)
{
    m_pSpiderAnt = spiderant;
    CGameInstance* pGameInstance = CGameInstance::Get_Instance();
    Safe_AddRef(pGameInstance);

    m_pObject = pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_SpiderAnt"));

    m_iPreviousHp = m_pObject->Get_Hp();

    Safe_Release(pGameInstance);

    return S_OK;
}

void CSpiderAnt_Shoot::Tick(_float fTimeDelta)
{
    CGameInstance* pGameInstance = CGameInstance::Get_Instance();
    Safe_AddRef(pGameInstance);

    m_pModelCom = dynamic_cast<CModel*>(pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_SpiderAnt"), TEXT("Com_Model")));
    m_pModelCom->Play_Animation(fTimeDelta);

    m_pModelCom->SetUp_Animation(9, true);

    m_pTransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_SpiderAnt"), TEXT("Com_Transform")));
    m_pPlayerTransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_Transform")));

    m_vPlayerPos.x = m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION).m128_f32[0];
    m_vPlayerPos.z = m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION).m128_f32[2];

    m_vPos.x = m_pTransformCom->Get_State(CTransform::STATE_POSITION).m128_f32[0];
    m_vPos.z = m_pTransformCom->Get_State(CTransform::STATE_POSITION).m128_f32[2];

    m_Look = m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE_POSITION);

    m_pTransformCom->LookAt(m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION));

    m_iCurrentHp = m_pObject->Get_Hp();

    m_fTimer += fTimeDelta;
    if (m_fTimer >= 0.5f)
    {
        _ulong		dwByte = 0;
        HANDLE		hFile = CreateFile(TEXT("../../Client/Bin/Resources/Effect/BleedingSnow.txt"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
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
            Desc.fMinSpeed = 1.f;
            Desc.fMaxSpeed = 3.f;
            Desc.vPivotPos.y = 5.f;
            Desc.bBlue = true;
            if (FAILED(pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Layer_Effect_Explosion"),
                TEXT("Prototype_GameObject_Lightning"), &Desc)))
                return;
        }
        CloseHandle(hFile);
        m_fTimer = 0.f;
    }

    if (m_pModelCom->Get_AnimEnd())
    {
        m_pModelCom->Set_AnimEnd(false);
        if (m_pModelCom->Get_Animation()[9]->Get_AnimEnd())
        {
            m_pModelCom->Get_Animation()[9]->Set_AnimEnd(false);
            _ulong		dwByte = 0;
            HANDLE		hFile = CreateFile(TEXT("../../Client/Bin/Resources/Effect/BleedingSnow.txt"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
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

            if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Monster_Fire"),
                TEXT("Prototype_GameObject_EMPBlue"), &Desc)))
                return;
            }
            CloseHandle(hFile);


            if (m_iCurrentHp < m_iPreviousHp - 150)
            {
                m_pModelCom->Stop_Animation(fTimeDelta, true);
                m_pSpiderAnt->Change_State(CSpiderAnt::STATE_DIG);
            }


        }
    }

    Safe_Release(pGameInstance);
}

void CSpiderAnt_Shoot::Exit(CSpiderAnt* spiderant)
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
