#include "Tool_ObjectManager.h"

#include "Tool_Object.h"
#include "Imgui_Manager.h"

IMPLEMENT_SINGLETON(CTool_ObjectManager)

CTool_ObjectManager::CTool_ObjectManager()
{
}

HRESULT CTool_ObjectManager::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    return S_OK;
}

void CTool_ObjectManager::Tick(_float _fTimeDelta)
{

}

void CTool_ObjectManager::Late_Tick(_float _fTimeDelta)
{
}

HRESULT CTool_ObjectManager::Add_Object(void* pArg)
{
    OBJECTDESC* ObjDesc = (OBJECTDESC*)pArg;
    
    wstring Name = m_ObjectDesc.strName = ObjDesc->strName;
    _float3 Size = m_ObjectDesc.ObjectSize = ObjDesc->ObjectSize;
    _vector Pos = m_ObjectDesc.CreatePos = ObjDesc->CreatePos;
    _float Radian = m_ObjectDesc.fRadian = ObjDesc->fRadian;
    _bool   bSave = m_ObjectDesc.bSave = ObjDesc->bSave;

    if(!bSave)
        m_ObjectList.emplace_back(Name, Size, Pos, Radian);
    else
    {
        _ulong		dwByte = 0;
        HANDLE		hFile = CreateFile(TEXT("../../Client/Bin/Resources/Map/Town.txt"), GENERIC_WRITE, 0, nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

        SetFilePointer(hFile, 0, nullptr, FILE_END);

        for (const auto& obj : m_ObjectList) {
            size_t dataLength = get<0>(obj).size();
            WriteFile(hFile, &dataLength, sizeof(size_t), &dwByte, nullptr);
            WriteFile(hFile, get<0>(obj).c_str(), sizeof(wchar_t) * dataLength, &dwByte, nullptr);
            WriteFile(hFile, &get<2>(obj), sizeof(_vector), &dwByte, nullptr);
            WriteFile(hFile, &get<1>(obj), sizeof(_float3), &dwByte, nullptr);
            WriteFile(hFile, &get<3>(obj), sizeof(_float), &dwByte, nullptr);
        }
        m_pImgui_Manager->Get_Instance()->Set_Save(false);
        m_bSave = false;

        CloseHandle(hFile);
    }

    return S_OK;
}

HRESULT CTool_ObjectManager::Save_Data(HANDLE handle)
{
    if (m_ObjectList.empty()) 
        return E_FAIL;

    return S_OK;
}

HRESULT CTool_ObjectManager::Load_Data(HANDLE handle)
{
    return S_OK;
}

void CTool_ObjectManager::Free()
{
	Safe_Release(m_pImgui_Manager);
}
