#include "stdafx.h"
#include "SpiderAnt_Dig.h"

#include "SpiderAnt_State.h"
#include "SpiderAnt.h"
#include "SoundMgr.h"

#include "GameInstance.h"

HRESULT CSpiderAnt_Dig::Enter(CSpiderAnt* spiderant)
{
    m_pSpiderAnt = spiderant;

    return S_OK;
}

void CSpiderAnt_Dig::Tick(_float fTimeDelta)
{
    CGameInstance* pGameInstance = CGameInstance::Get_Instance();
    Safe_AddRef(pGameInstance);

	if(m_iCount <= 7)
		m_fTimer += fTimeDelta;

    if (m_iCount == 0)
    {
        CSoundMgr::Get_Instance()->PlaySoundW(L"c2320_dust.mp3", 1.f);
        m_iCount++;
    }

	m_pTransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_SpiderAnt"), TEXT("Com_Transform")));
    m_pModelCom = dynamic_cast<CModel*>(pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_SpiderAnt"), TEXT("Com_Model")));
    m_pModelCom->Play_Animation(fTimeDelta);

    m_pModelCom->SetUp_Animation(19, true);

    m_vPos.x = m_pTransformCom->Get_State(CTransform::STATE_POSITION).m128_f32[0];
    m_vPos.z = m_pTransformCom->Get_State(CTransform::STATE_POSITION).m128_f32[2];

	if (m_fTimer > 0.2f)
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

        }
        CloseHandle(hFile);

		m_iCount++;
		m_fTimer = 0.f;
	}

    if (m_pModelCom->Get_AnimEnd())
    {
        m_pModelCom->Set_AnimEnd(false);
        if (m_pModelCom->Get_Animation()[19]->Get_AnimEnd())
        {
            m_pModelCom->Get_Animation()[19]->Set_AnimEnd(false);
            m_pSpiderAnt->Change_State(CSpiderAnt::STATE_APPEAR);
        }
    }
    

    Safe_Release(pGameInstance);
}

void CSpiderAnt_Dig::Exit(CSpiderAnt* spiderant)
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
