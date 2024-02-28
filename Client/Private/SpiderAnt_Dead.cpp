#include "stdafx.h"
#include "SpiderAnt_State.h"
#include "SpiderAnt.h"
#include "GameInstance.h"

#include "SpiderAnt_Dead.h"
#include "SoundMgr.h"

HRESULT CSpiderAnt_Dead::Enter(CSpiderAnt* spiderant)
{
    m_pSpiderAnt = spiderant;

    return S_OK;
}

void CSpiderAnt_Dead::Tick(_float fTimeDelta)
{
    CGameInstance* pGameInstance = CGameInstance::Get_Instance();
    Safe_AddRef(pGameInstance);

    m_pModelCom = dynamic_cast<CModel*>(pGameInstance->Get_Component(g_CurLevel, TEXT("Layer_SpiderAnt"), TEXT("Com_Model")));
    m_pTransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_Component(g_CurLevel, TEXT("Layer_SpiderAnt"), TEXT("Com_Transform")));
    m_pObject = pGameInstance->Get_GameObject(g_CurLevel, TEXT("Layer_SpiderAnt"));

    m_pModelCom->Play_Animation(fTimeDelta);

    m_pModelCom->SetUp_Animation(23,true);

    m_vPos.x = m_pTransformCom->Get_State(CTransform::STATE_POSITION).m128_f32[0];
    m_vPos.z = m_pTransformCom->Get_State(CTransform::STATE_POSITION).m128_f32[2];

    //m_fTimer += fTimeDelta;
    //if (m_fTimer>=0.3f)
    //{
    //    _ulong		dwByte = 0;
    //    HANDLE		hFile = CreateFile(TEXT("../../Client/Bin/Resources/Effect/BleedingSnow.txt"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    //    size_t		dataLength;
    //    wstring		Name;

    //    CVIBuffer_Point_Instance::POINTINSTANCEDESC		Desc;
    //    while (true)
    //    {
    //        if (!ReadFile(hFile, &dataLength, sizeof(size_t), &dwByte, nullptr))
    //            break;
    //        if (dataLength == 0)
    //            break;
    //        if (0 == dwByte)
    //            break;
    //        Name.resize(dataLength);
    //        ReadFile(hFile, &Name[0], sizeof(wchar_t) * dataLength, &dwByte, nullptr);
    //        ReadFile(hFile, &Desc.iNumInstance, sizeof(_int), &dwByte, nullptr);
    //        ReadFile(hFile, &Desc.fLifeTime, sizeof(_float), &dwByte, nullptr);
    //        ReadFile(hFile, &Desc.vPivotPos, sizeof(_float4), &dwByte, nullptr);
    //        ReadFile(hFile, &Desc.vRange, sizeof(_float3), &dwByte, nullptr);
    //        ReadFile(hFile, &Desc.fMinSpeed, sizeof(_float), &dwByte, nullptr);
    //        ReadFile(hFile, &Desc.fMaxSpeed, sizeof(_float), &dwByte, nullptr);
    //        ReadFile(hFile, &Desc.bFalling, sizeof(_bool), &dwByte, nullptr);
    //        ReadFile(hFile, &Desc.bRising, sizeof(_bool), &dwByte, nullptr);
    //        ReadFile(hFile, &Desc.bExplosion, sizeof(_bool), &dwByte, nullptr);
    //        ReadFile(hFile, &Desc.bBleeding, sizeof(_bool), &dwByte, nullptr);
    //        ReadFile(hFile, &Desc.bDig, sizeof(_bool), &dwByte, nullptr);
    //        ReadFile(hFile, &Desc.bGreen, sizeof(_bool), &dwByte, nullptr);

    //        random_device		RandomDevice;
    //        mt19937_64			RandomNumber;
    //        uniform_real_distribution<float>		RandomX;
    //        uniform_real_distribution<float>		RandomZ;
    //        uniform_real_distribution<float>		RandomY;

    //        RandomNumber = mt19937_64(RandomDevice());

    //        RandomX = uniform_real_distribution<float>(m_vPos.x - 1.f, m_vPos.x + 1.f);
    //        RandomZ = uniform_real_distribution<float>(m_vPos.z - 1.f, m_vPos.z + 1.f);
    //        RandomY = uniform_real_distribution<float>(1.f, 7.f);

    //        _vector TempPos = XMVectorSet(RandomX(RandomNumber) + m_pTransformCom->Get_State(CTransform::STATE_LOOK).m128_f32[0], RandomY(RandomNumber), 
    //            RandomZ(RandomNumber) + m_pTransformCom->Get_State(CTransform::STATE_LOOK).m128_f32[2], 1.f);
    //        //_vector TempPos = m_pTransformCom->Get_State(CTransform::STATE_LOOK) + m_pTransformCom->Get_State(CTransform::STATE_POSITION);

    //        XMStoreFloat4(&Desc.vPivotPos, TempPos);
    //        Desc.fMinSpeed = 1.f;
    //        Desc.fMaxSpeed = 3.f;

    //        if (FAILED(pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Layer_Effect_Explosion"),
    //            TEXT("Prototype_GameObject_Smoke"), &Desc)))
    //            return;
    //    }
    //    CloseHandle(hFile);
    //    m_fTimer = 0.f;
    //}
    m_fTimer += fTimeDelta;

    if (m_fTimer >= 0.5f)
    {
        m_pSpiderAnt->Set_Dissolve(true);

    }

    if (m_pModelCom->Get_AnimEnd())
    {
        //m_pSpiderAnt->Set_DeathAnim(true);
        m_pModelCom->Set_AnimEnd(false);

        if (m_pModelCom->Get_Animation()[23]->Get_AnimEnd())
        {
        }
    }

    Safe_Release(pGameInstance);
}

void CSpiderAnt_Dead::Exit(CSpiderAnt* spiderant)
{
    m_pTransformCom = { nullptr };
    m_pModelCom = { nullptr };

    Safe_Release(m_pModelCom);
    Safe_Release(m_pPlayerTransformCom);
    Safe_Release(m_pTransformCom);
    Safe_Release(m_pNavigationCom);
}
