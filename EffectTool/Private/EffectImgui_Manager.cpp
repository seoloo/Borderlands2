#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"

#include "EffectImgui_Manager.h"
#include "GameInstance.h"

IMPLEMENT_SINGLETON(CEffectImgui_Manager)

#pragma region 전역변수
_bool	  g_bIsCursorOnImgUi = false;
_bool     show_Menu_window = true;
_bool       show_Effect_Menu = true;
_int        g_selectedPNGIndex = -1;
namespace fs = std::filesystem;

#pragma endregion

CEffectImgui_Manager::CEffectImgui_Manager()
{
}

HRESULT CEffectImgui_Manager::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    m_pDevice = pDevice;
    m_pContext = pContext;
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(g_hWnd);
    ImGui_ImplDX11_Init(m_pDevice, m_pContext);

    return S_OK;
}

void CEffectImgui_Manager::Tick()
{
    // Start the Dear ImGui frame
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    ImGui::SetNextWindowPos(ImVec2(10, 10));
    Menu();

    ImGui::EndFrame();
}

void CEffectImgui_Manager::Render()
{
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void CEffectImgui_Manager::Menu()
{
    ImGuiIO& io = ImGui::GetIO();

    ImGui::Begin("Menu", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings); // Create a window called "Hello, world!" and append into it.

    ImGui::Text("App average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);

    if (ImGui::IsMousePosValid())
        ImGui::Text("Mouse pos: (%g, %g)", io.MousePos.x, io.MousePos.y);
    else
        ImGui::Text("Mouse pos: <INVALID>");

    ImGui::Separator();
    ImGui::Spacing();

    ImGui::Checkbox("Effect Window", &show_Menu_window);
    ImGui::Checkbox("Effect Menu", &show_Effect_Menu);

    if (show_Menu_window)
    {
        ImGui::SetNextWindowPos(ImVec2(10, 200));
        Menu_Window();
    }

    if (show_Effect_Menu)
    {
        ImGui::SetNextWindowPos(ImVec2(1000, 10));
        Effect_List();
    }

    if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow))
        g_bIsCursorOnImgUi = true;
    else
        if (!ImGui::IsMouseDown(ImGuiMouseButton_Left))
            g_bIsCursorOnImgUi = false;

   
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::SameLine();

    if (ImGui::Button("Store"))
    {
        m_bApply = true;

        // 저장 완료 메시지 팝업
        ImGui::OpenPopup("Store Confirmation");

        // 저장 완료 메시지 팝업
        if (ImGui::BeginPopupModal("Store Confirmation", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("Store.");

            if (ImGui::Button("Okay"))
                ImGui::CloseCurrentPopup(); // 팝업 닫기

            ImGui::EndPopup();
        }
        /*m_EffectList.push_back(make_tuple(m_EffectInfo.iNumber, m_EffectInfo.fLifeTime, m_EffectInfo.vPivotPos,
            m_EffectInfo.vRange, m_EffectInfo.fMinSpeed, m_EffectInfo.fMaxSpeed, m_EffectInfo.bFalling, 
            m_EffectInfo.bRising, m_EffectInfo.bBleeding));*/

    }

    ImGui::SameLine();

    if (ImGui::Button("Create"))
        m_bCreate = true;

    ImGui::SameLine();

    if (ImGui::Button("Save"))
    {
        m_bSave = true;

        //Save_File();
        Write_File();
    }
    ImGui::SameLine();

    if (ImGui::Button("Load"))
    {
        Load_File();

        m_bLoad = true;

    }

    ImGui::Separator();
    ImGui::Spacing();

    ImGui::End();
}

void CEffectImgui_Manager::Menu_Window()
{
    ImGui::Begin("Effect Window", &show_Menu_window, ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::BeginChild("List", ImVec2(400, 300), true, ImGuiWindowFlags_HorizontalScrollbar);

    if (!m_strPNGName.empty()) {
        ImGui::Text("Selected PNG: %ls", m_strPNGName.c_str());

        ImGui::Separator();
        ImGui::Spacing();

        string PngName(m_strPNGName.begin(), m_strPNGName.end());

        m_EffectInfo.strName = m_strPNGName;

        if (ImGui::TreeNode(PngName.c_str()))
        {
            ImGui::Spacing();
            ImGui::PushItemWidth(150);
            ImGui::SameLine();
            ImGui::InputInt("Number", &m_EffectInfo.iNumber, 1, m_EffectInfo.iNumber);

            ImGui::Spacing();
            ImGui::PushItemWidth(150);
            ImGui::SameLine();
            ImGui::InputFloat("LifeTime", &m_EffectInfo.fLifeTime, 0.1f, m_EffectInfo.fLifeTime, "%.2f");

            ImGui::Spacing();
            ImGui::PushItemWidth(150);
            ImGui::SameLine();
            ImGui::InputFloat("PivotPosX", &m_EffectInfo.vPivotPos.x, 0.1f, m_EffectInfo.vPivotPos.x, "%.2f");

            ImGui::Spacing();
            ImGui::PushItemWidth(150);
            ImGui::SameLine();
            ImGui::InputFloat("PivotPosY", &m_EffectInfo.vPivotPos.y, 0.1f, m_EffectInfo.vPivotPos.y, "%.2f");

            ImGui::Spacing();
            ImGui::PushItemWidth(150);
            ImGui::SameLine();
            ImGui::InputFloat("PivotPosZ", &m_EffectInfo.vPivotPos.z, 0.1f, m_EffectInfo.vPivotPos.z, "%.2f");

            ImGui::Spacing();
            ImGui::PushItemWidth(150);
            ImGui::SameLine();
            ImGui::InputFloat("RangeX", &m_EffectInfo.vRange.x, 0.1f, m_EffectInfo.vRange.x, "%.2f");

            ImGui::Spacing();
            ImGui::PushItemWidth(150);
            ImGui::SameLine();
            ImGui::InputFloat("RangeY", &m_EffectInfo.vRange.y, 0.1f, m_EffectInfo.vRange.y, "%.2f");

            ImGui::Spacing();
            ImGui::PushItemWidth(150);
            ImGui::SameLine();
            ImGui::InputFloat("RangeZ", &m_EffectInfo.vRange.z, 0.1f, m_EffectInfo.vRange.z, "%.2f");

            ImGui::Spacing();
            ImGui::PushItemWidth(150);
            ImGui::SameLine();
            ImGui::InputFloat("MinSpeed", &m_EffectInfo.fMinSpeed, 0.1f, m_EffectInfo.fMinSpeed, "%.2f");

            ImGui::Spacing();
            ImGui::PushItemWidth(150);
            ImGui::SameLine();
            ImGui::InputFloat("MaxSpeed", &m_EffectInfo.fMaxSpeed, 0.1f, m_EffectInfo.fMaxSpeed, "%.2f");

            ImGui::Spacing();

            if (ImGui::TreeNode("Type"))
            {
                if (!m_strTypeName.empty()) 
                    ImGui::Text("Selected Type : %ls", m_strTypeName.c_str());

                ImGui::Spacing();

                if (ImGui::RadioButton("Falling", m_iSelectType == 0))
                {
                    m_strTypeName = TEXT("Falling");
                    m_iSelectType = 0;
                }

                if (ImGui::RadioButton("Rising", m_iSelectType == 1))
                {
                    m_strTypeName = TEXT("Rising");
                    m_iSelectType = 1;
                }

                if (ImGui::RadioButton("Explosion", m_iSelectType == 2))
                {
                    m_strTypeName = TEXT("Explosion");
                    m_iSelectType = 2;
                }

                if (ImGui::RadioButton("Bleeding", m_iSelectType == 3))
                {
                    m_strTypeName = TEXT("Bleeding");
                    m_iSelectType = 3;
                }

                if (ImGui::RadioButton("Dig", m_iSelectType == 4))
                {
                    m_strTypeName = TEXT("Dig");
                    m_iSelectType = 4;
                }

                switch (m_iSelectType)
                {
                case 0 :
                    m_EffectInfo.bFalling = true;
                    m_EffectInfo.bRising = false;
                    m_EffectInfo.bExplosion = false;
                    m_EffectInfo.bBleeding = false;
                    m_EffectInfo.bDig = false;
                    break;
                case 1:
                    m_EffectInfo.bFalling = false;
                    m_EffectInfo.bRising = true;
                    m_EffectInfo.bExplosion = false;
                    m_EffectInfo.bBleeding = false;
                    m_EffectInfo.bDig = false;
                    break;
                case 2:
                    m_EffectInfo.bFalling = false;
                    m_EffectInfo.bRising = false;
                    m_EffectInfo.bExplosion = true;
                    m_EffectInfo.bBleeding = false;
                    m_EffectInfo.bDig = false;
                    break;
                case 3:
                    m_EffectInfo.bFalling = false;
                    m_EffectInfo.bRising = false;
                    m_EffectInfo.bExplosion = false;
                    m_EffectInfo.bBleeding = true;
                    m_EffectInfo.bDig = false;
                    break;
                case 4:
                    m_EffectInfo.bFalling = false;
                    m_EffectInfo.bRising = false;
                    m_EffectInfo.bExplosion = false;
                    m_EffectInfo.bBleeding = false;
                    m_EffectInfo.bDig = true;
                    break;
                default:
                    break;
                }

                ImGui::TreePop();
            }
            ImGui::TreePop();
        }
    }
    ImGui::EndChild();

    ImGui::End();
}

void CEffectImgui_Manager::Effect_List()
{
    ImGui::Begin("Effect List", &show_Menu_window, ImGuiWindowFlags_AlwaysAutoResize);

    //wstring directoryPath = L"E://쥬신/3D개인/Framework/Client/Bin/Resources/Borderland/Effect/";
    wstring directoryPath = L"D://쥬신/3D개인/Framework/Client/Bin/Resources/Borderland/Effect/";
    vector<wstring> pngFileNames = PNGFileNames(directoryPath);

    if (ImGui::CollapsingHeader("Effect"))
    {
        for (int i = 0; i < pngFileNames.size(); i++) {
            string fileNameUtf8;
            int size = WideCharToMultiByte(CP_UTF8, 0, pngFileNames[i].c_str(), -1, nullptr, 0, nullptr, nullptr);
            fileNameUtf8.resize(size);
            WideCharToMultiByte(CP_UTF8, 0, pngFileNames[i].c_str(), -1, &fileNameUtf8[0], size, nullptr, nullptr);

            if (ImGui::Selectable(fileNameUtf8.c_str())) {
                g_selectedPNGIndex = i;
            }
        }
        if (g_selectedPNGIndex != -1)
        {
            ImGui::Separator();
            ImGui::Spacing();

            wstring selectedPNGFileName = pngFileNames[g_selectedPNGIndex];

            size_t pos = selectedPNGFileName.find(L".dds");

            if (pos != wstring::npos)
                selectedPNGFileName = selectedPNGFileName.substr(0, pos);

            //ImGui::Text("Selected File: %ls", selectedDDSFileName.c_str());

            wstring fullFileName = selectedPNGFileName.c_str();

            size_t dotPos = fullFileName.find_last_of(L'.');

            if (dotPos != wstring::npos)
                m_strPNGName = fullFileName.substr(0, dotPos);
        }
    }

    ImGui::End();
}

HRESULT CEffectImgui_Manager::Write_File()
{
    _ulong		dwByte = 0;
    HANDLE		hFile = CreateFile(TEXT("../../Client/Bin/Resources/Effect/Catridge.txt"), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

    SetFilePointer(hFile, 0, nullptr, FILE_END);

    wstring Name = m_EffectInfo.strName.c_str();
    size_t dataLength = m_EffectInfo.strName.size();
    WriteFile(hFile, &dataLength, sizeof(size_t), &dwByte, nullptr);
    WriteFile(hFile, &Name, sizeof(wchar_t) * dataLength, &dwByte, nullptr);
    WriteFile(hFile, &m_EffectInfo.iNumber, sizeof(_int), &dwByte, nullptr);
    WriteFile(hFile, &m_EffectInfo.fLifeTime, sizeof(_float), &dwByte, nullptr);
    WriteFile(hFile, &m_EffectInfo.vPivotPos, sizeof(_float4), &dwByte, nullptr);
    WriteFile(hFile, &m_EffectInfo.vRange, sizeof(_float3), &dwByte, nullptr);
    WriteFile(hFile, &m_EffectInfo.fMinSpeed, sizeof(_float), &dwByte, nullptr);
    WriteFile(hFile, &m_EffectInfo.fMaxSpeed, sizeof(_float), &dwByte, nullptr);
    WriteFile(hFile, &m_EffectInfo.bFalling, sizeof(_bool), &dwByte, nullptr);
    WriteFile(hFile, &m_EffectInfo.bRising, sizeof(_bool), &dwByte, nullptr);
    WriteFile(hFile, &m_EffectInfo.bExplosion, sizeof(_bool), &dwByte, nullptr);
    WriteFile(hFile, &m_EffectInfo.bBleeding, sizeof(_bool), &dwByte, nullptr);
    WriteFile(hFile, &m_EffectInfo.bDig, sizeof(_bool), &dwByte, nullptr);
    WriteFile(hFile, &m_EffectInfo.bGreen, sizeof(_bool), &dwByte, nullptr);

    CloseHandle(hFile);

    m_bSave = false;

    return S_OK;
}

HRESULT CEffectImgui_Manager::Save_File()
{
    OPENFILENAME ofn;
    ZeroMemory(&ofn, sizeof(OPENFILENAME));
    TCHAR szFile[MAX_PATH] = { 0 };

    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = g_hWnd;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrFilter = TEXT("텍스트 파일 (*.txt)\0*.txt\0모든 파일 (*.*)\0*.*\0");
    ofn.lpstrFile = szFile;
    ofn.lpstrDefExt = TEXT("txt");
    ofn.lpstrInitialDir = TEXT("E:\\쥬신\3D개인\Framework\Client\Bin\Resources\Effect\\");
    ofn.Flags = OFN_OVERWRITEPROMPT;

    if (GetSaveFileName(&ofn) != 0) {
        if (FAILED(Save(szFile))) {
            MSG_BOX("Failed to Save");
        }
    }

    m_bSave = false;

    return S_OK;
}

HRESULT CEffectImgui_Manager::Save(const wchar_t* filePath)
{
    wofstream file(filePath);

    if (file.is_open())
    {
        file << ' ';
        file << m_EffectInfo.iNumber;
        file << m_EffectInfo.fLifeTime;
        file << m_EffectInfo.vPivotPos.x;
        file << m_EffectInfo.vPivotPos.y;
        file << m_EffectInfo.vPivotPos.z;
        file << m_EffectInfo.vRange.x;
        file << m_EffectInfo.vRange.y;
        file << m_EffectInfo.vRange.z;
        file << m_EffectInfo.fMinSpeed;
        file << m_EffectInfo.fMaxSpeed;
        file << m_EffectInfo.bFalling;
        file << m_EffectInfo.bRising;
        file << m_EffectInfo.bExplosion;
        file << m_EffectInfo.bBleeding;
        file << '\n';

        file.close();
    }
    else
    {
        file.close();
        MSG_BOX("Failed to open");
        return E_FAIL;
    }


    return S_OK;
}

HRESULT CEffectImgui_Manager::Load_File()
{
    OPENFILENAME ofn;
    ZeroMemory(&ofn, sizeof(OPENFILENAME));
    TCHAR szFile[MAX_PATH] = TEXT("");

    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = NULL;
    ofn.lpstrFilter = TEXT("텍스트 파일(*.txt)\0 * .txt\0모든 파일(*.*)\0 * .*\0");
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrTitle = TEXT("파일 선택");
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;

    if (GetOpenFileName(&ofn) == TRUE) {
        if (FAILED(Load(szFile))) {
            MSG_BOX("Failed to Load");
        }
    }

    m_bLoad = false;

    return S_OK;
}

HRESULT CEffectImgui_Manager::Load(const wchar_t* filePath)
{
    wifstream file(filePath);
    wstring line;
    if (file.is_open())
    {
        while (getline(file, line))
        {
            wistringstream ss(line);
            wstring field;

            while (getline(ss, field))
            {
                ss >> m_EffectInfo.iNumber;
                ss >> m_EffectInfo.fLifeTime;
                ss >> m_EffectInfo.vPivotPos.x;
                ss >> m_EffectInfo.vPivotPos.y;
                ss >> m_EffectInfo.vPivotPos.z;
                ss >> m_EffectInfo.vRange.x;
                ss >> m_EffectInfo.vRange.y;
                ss >> m_EffectInfo.vRange.z;
                ss >> m_EffectInfo.fMinSpeed;
                ss >> m_EffectInfo.fMaxSpeed;
                ss >> m_EffectInfo.bFalling;
                ss >> m_EffectInfo.bRising;
                ss >> m_EffectInfo.bExplosion;
                ss >> m_EffectInfo.bBleeding;
            }

        }

        file.close();
    }
    else
    {
        file.close();
        MSG_BOX("Failed to open");
        return E_FAIL;
    }


    return S_OK;
}

vector<wstring> CEffectImgui_Manager::PNGFileNames(const wstring& strPath)
{
    vector<wstring> fileNames;

    for (const auto& entry : fs::directory_iterator(strPath)) {
        if (entry.path().extension() == TEXT(".png")) {
            fileNames.push_back(entry.path().filename().wstring());
        }
    }
    return fileNames;
}

void CEffectImgui_Manager::Free()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
}
