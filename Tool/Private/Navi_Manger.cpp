#include "Navi_Manager.h"

#include "Tool_Object.h"
#include "Imgui_Manager.h"

IMPLEMENT_SINGLETON(CNavi_Manager)

CNavi_Manager::CNavi_Manager()
{
}

HRESULT CNavi_Manager::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    return S_OK;
}

void CNavi_Manager::Tick(_float _fTimeDelta)
{

}

void CNavi_Manager::Late_Tick(_float _fTimeDelta)
{
}

HRESULT CNavi_Manager::Add_Navi(void* pArg)
{
    NAVIINFO* NaviInfo = (NAVIINFO*)pArg;

    if (!NaviInfo->bSave)
        m_ListNavi.emplace_back(NaviInfo->strName, NaviInfo->NaviPos[0], NaviInfo->NaviPos[1], NaviInfo->NaviPos[2]);
    else
    {
        _ulong		dwByte = 0;
        HANDLE		hFile = CreateFile(TEXT("../../Client/Bin/Resources/Map/NaviTownTest.txt"), GENERIC_WRITE, 0, nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

        SetFilePointer(hFile, 0, nullptr, FILE_END);

        for (const auto& obj : m_ListNavi) {
            size_t dataLength = get<0>(obj).size();

            WriteFile(hFile, &dataLength, sizeof(size_t), &dwByte, nullptr);
            WriteFile(hFile, get<0>(obj).c_str(), sizeof(wchar_t) * dataLength, &dwByte, nullptr);

            WriteFile(hFile, &get<1>(obj), sizeof(_float3), &dwByte, nullptr);
            WriteFile(hFile, &get<2>(obj), sizeof(_float3), &dwByte, nullptr);
            WriteFile(hFile, &get<3>(obj), sizeof(_float3), &dwByte, nullptr);
        }

        m_bSave = false;

        CloseHandle(hFile);
    }

    return S_OK;
}


void CNavi_Manager::Free()
{
	Safe_Release(m_pImgui_Manager);

}
