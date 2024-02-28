#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"

#include "Imgui_Manager.h"
#include "GameInstance.h"
#include "Tool_Terrain.h"
#include "Tool_ObjectManager.h"
#include "Navi_Manager.h"
#include "Tool_Object_Icebox01.h"

IMPLEMENT_SINGLETON(CImgui_Manager)

#pragma region 전역변수
_bool	  g_bIsCursorOnImgUi = false;
_bool     show_Menu_window = true;
_bool     show_Demo_window = true;

ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

namespace fs = std::filesystem;

int g_selectedDDSIndex = -1;
int g_selectedFbxIndex = -1;
_bool   g_bPicking = false;

XMVECTOR g_vObjPos = {XMVectorSet(0.f,0.f,0.f,1.f)};
XMFLOAT3 g_vObjSize = { 0.f,0.f,0.f };
float g_fRadian = { 0.f };
#pragma endregion

CImgui_Manager::CImgui_Manager()
{
}

HRESULT CImgui_Manager::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
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

void CImgui_Manager::Tick()
{
    // Start the Dear ImGui frame
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    ImGui::SetNextWindowPos(ImVec2(10, 10));
    Menu();

    ImGui::EndFrame();
}

void CImgui_Manager::Render()
{
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void CImgui_Manager::Menu()
{
    ImGuiIO& io = ImGui::GetIO();

    ImGui::Begin("Menu", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings); // Create a window called "Hello, world!" and append into it.

    ImGui::Text("App average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);

    if (ImGui::IsMousePosValid())
        ImGui::Text("Mouse pos: (%g, %g)", io.MousePos.x /*- g_iWinSizeX * 0.5f*/, /*g_iWinSizeY * 0.5f -*/ io.MousePos.y);
    else
        ImGui::Text("Mouse pos: <INVALID>");

    ImGui::Separator();
    ImGui::Spacing();

    ImGui::Checkbox("Object Window", &show_Menu_window);
    ImGui::Checkbox("Object Menu", &m_bObject_Menu);
    ImGui::Checkbox("Navi Menu", &m_bNavi_Menu);
    ImGui::Checkbox("Position Menu", &m_bPosition_Menu);

    ImGui::Separator();
    ImGui::Spacing();

    if (ImGui::Button("Save"))
    {
        m_bSave = true;
        CTool_ObjectManager::OBJECTDESC objDesc;
        ZeroMemory(&objDesc, sizeof(CTool_ObjectManager::OBJECTDESC));

        objDesc.bSave = true;
        m_pObjectManager->Get_Instance()->Add_Object(&objDesc);
    }

    ImGui::SameLine();

    if (ImGui::Button("Load"))
    {
        m_bLoad = true;
    }

    if (show_Menu_window)
    {
        ImGui::SetNextWindowPos(ImVec2(1000, 10));
        Menu_Window();
    }

    if (m_bObject_Menu)
    {
        ImGui::SetNextWindowPos(ImVec2(10, 200));
        Object_Menu();
    }

    if (m_bNavi_Menu)
    {
        ImGui::SetNextWindowPos(ImVec2(10, 220));
        Navi_Menu();
    }

    if (m_bPosition_Menu)
    {
        ImGui::SetNextWindowPos(ImVec2(300, 10));
        Position_Menu();
    }

    if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow))
        g_bIsCursorOnImgUi = true;
    else
        if (!ImGui::IsMouseDown(ImGuiMouseButton_Left))
            g_bIsCursorOnImgUi = false;

    ImGui::End();
}

void CImgui_Manager::Menu_Window()
{
    ImGui::Begin("Object Window", &show_Menu_window, ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::Spacing();

    wstring directoryTerrainPath = L"../../Client/Bin/Resources/Borderland/Terrain/";
    vector<wstring> ddsFileNames = DDSFileNames(directoryTerrainPath);

    if (ImGui::CollapsingHeader("Terrain"))
    {
        for (int i = 0; i < ddsFileNames.size(); i++) {
            string fileNameUtf8;
            int size = WideCharToMultiByte(CP_UTF8, 0, ddsFileNames[i].c_str(), -1, nullptr, 0, nullptr, nullptr);
            fileNameUtf8.resize(size);
            WideCharToMultiByte(CP_UTF8, 0, ddsFileNames[i].c_str(), -1, &fileNameUtf8[0], size, nullptr, nullptr);

            if (ImGui::Selectable(fileNameUtf8.c_str())) {
                g_selectedDDSIndex = i;
            }
        }
        if (g_selectedDDSIndex != -1)
        {
            ImGui::Separator();
            ImGui::Spacing();

            wstring selectedDDSFileName = ddsFileNames[g_selectedDDSIndex];

            size_t pos = selectedDDSFileName.find(L".dds");

            if (pos != wstring::npos)
            {
                selectedDDSFileName = selectedDDSFileName.substr(0, pos);
            }

            ImGui::Text("Selected File: %ls", selectedDDSFileName.c_str());

            m_strTerrainName = selectedDDSFileName.c_str();
        }
    }

    wstring directoryStaticPath = L"../../Client/Bin/Resources/Borderland/Thresher_Static/";
    vector<wstring> fbxFileNames = StaticFbxFileNames(directoryStaticPath);

    if (ImGui::CollapsingHeader("Static"))
    {

        for (int i = 0; i < fbxFileNames.size(); i++) {
            string fileNameUtf8;
            int size = WideCharToMultiByte(CP_UTF8, 0, fbxFileNames[i].c_str(), -1, nullptr, 0, nullptr, nullptr);
            fileNameUtf8.resize(size);
            WideCharToMultiByte(CP_UTF8, 0, fbxFileNames[i].c_str(), -1, &fileNameUtf8[0], size, nullptr, nullptr);

            if (ImGui::Selectable(fileNameUtf8.c_str())) {
                g_selectedFbxIndex = i;
            }
        }
        if (g_selectedFbxIndex != -1)
        {
            ImGui::Separator();
            ImGui::Spacing();

            wstring selectedFbxFileName = fbxFileNames[g_selectedFbxIndex];
            size_t pos = selectedFbxFileName.find(L".fbx");

            if (pos != wstring::npos)
            {
                selectedFbxFileName = selectedFbxFileName.substr(0, pos);
            }
            ImGui::Text("Selected File: %ls", selectedFbxFileName.c_str());
            m_strFBXName = selectedFbxFileName;
        }
    }

    ImGui::Separator();
    ImGui::Spacing();

    if (ImGui::Button("Apply"))
    {
        m_bApply = true;
    }

    ImGui::SameLine();

    CGameInstance* pGameInstance = CGameInstance::Get_Instance();
    Safe_AddRef(pGameInstance);

    //if (pGameInstance->Get_DIKeyState(DIK_G) & 0x8000)
    if (ImGui::Button("Create"))
    {
        m_strObjectName = m_strFBXName + TEXT("_Num") + to_wstring(m_ObjectCounter[m_strFBXName]);
        m_ObjectCounter[m_strFBXName]++;

        if (m_ObjectInfo.find(m_strObjectName) == m_ObjectInfo.end())
        {
            m_ObjectInfo[m_strObjectName] = make_tuple( _float3(1.0f, 1.0f, 1.0f), 0.0f, XMVectorSet(m_PickingPos.x, m_PickingPos.y, m_PickingPos.z, 1.0f));
        }

        m_bCreate = true;
    }
    ImGui::SameLine();

    if (ImGui::Button("Fix"))
    {
        m_bFix = true;
    }

    Safe_Release(pGameInstance);

    ImGui::SameLine();

    ImGui::End();
}

void CImgui_Manager::Position_Menu()
{
    ImGui::Begin("Position Menu", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::BeginChild("Position", ImVec2(200, 100), true, ImGuiWindowFlags_HorizontalScrollbar);

    ImGui::BulletText("Picking Position");
    ImGui::PushItemWidth(150);

    ImGui::InputFloat("X", &m_PickingPos.x, 1.0f, m_PickingPos.x, "%.2f");
    ImGui::InputFloat("Y", &m_PickingPos.y, 1.0f, m_PickingPos.y, "%.2f");
    ImGui::InputFloat("Z", &m_PickingPos.z, 1.0f, m_PickingPos.z, "%.2f");

    ImGui::EndChild();

    ImGui::End();
}

void CImgui_Manager::Navi_Menu()
{
    ImGui::Begin("Navi Menu", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

    if (m_bPickTerrain)
    {
        ImGui::Text("Terrain Picked");
        ImGui::SameLine();
        ImGui::Text("%.2f", m_PickingPos.x);
        ImGui::SameLine();
        ImGui::Text("%.2f", m_PickingPos.y);
        ImGui::SameLine();
        ImGui::Text("%.2f", m_PickingPos.z);
    }
    else
        ImGui::Text("Terrain not Picked");


    ImGui::Separator();
    ImGui::Spacing();

    wstring Key = L"Navi" + to_wstring(m_iNaviCount);
    if (ImGui::Button("Fix1"))
    {
        m_bNaviFix1 = true;
        m_NaviMap[Key] = make_tuple(m_PickingPos, get<1>(m_NaviMap[Key]), get<2>(m_NaviMap[Key]));
    }

    ImGui::SameLine();

    if (ImGui::Button("Fix2"))
    {
        m_bNaviFix2 = true;
        m_NaviMap[Key] = make_tuple(get<0>(m_NaviMap[Key]), m_PickingPos, get<2>(m_NaviMap[Key]));
    }

    ImGui::SameLine();

    if (ImGui::Button("Fix3"))
    {
        m_bNaviFix3 = true;
        m_NaviMap[Key] = make_tuple(get<0>(m_NaviMap[Key]), get<1>(m_NaviMap[Key]), m_PickingPos);
    }

    ImGui::SameLine();

    if (ImGui::Button("StoreContainer"))
    {
        m_bNaviFix1 = false;
        m_bNaviFix2 = false;
        m_bNaviFix3 = false;

        CNavi_Manager::NAVIINFO naviInfo;
        ZeroMemory(&naviInfo, sizeof(CNavi_Manager::NAVIINFO));

        naviInfo.strName = L"Navi" + to_wstring(m_iNaviCount);
        naviInfo.NaviPos[0] = get<0>(m_NaviMap[Key]);
        naviInfo.NaviPos[1] = get<1>(m_NaviMap[Key]);
        naviInfo.NaviPos[2] = get<2>(m_NaviMap[Key]);
        naviInfo.bSave = false;

        m_pNaviManager->Get_Instance()->Add_Navi(&naviInfo);

        m_iNaviCount++;
    }

    ImGui::SameLine();

    if (ImGui::Button("SaveFile"))
    {
        m_bNaviSave = true;
        CNavi_Manager::NAVIINFO naviInfo;
        ZeroMemory(&naviInfo, sizeof(CNavi_Manager::NAVIINFO));

        naviInfo.bSave = true;
        m_pNaviManager->Get_Instance()->Add_Navi(&naviInfo);

        // 저장 완료 메시지 팝업
        ImGui::OpenPopup("Save Confirmation");
    }

    // 저장 완료 메시지 팝업
    if (ImGui::BeginPopupModal("Save Confirmation", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Save.");

        if (ImGui::Button("Okay"))
            ImGui::CloseCurrentPopup(); // 팝업 닫기

        ImGui::EndPopup();
    }

    ImGui::Separator();
    ImGui::Spacing();
    ImGui::BeginChild("NaviList", ImVec2(300, 300), true, ImGuiWindowFlags_HorizontalScrollbar);

    
    for (auto& pair : m_NaviMap)
    {
        wstring Name = pair.first;
        tuple<_float3, _float3, _float3>& NaviInfoTuple = pair.second;

        string NameStr(Name.begin(), Name.end());

        if (ImGui::TreeNode(NameStr.c_str()))
        {
            _float3& firstElement = get<0>(NaviInfoTuple);
            _float3& secondElement = get<1>(NaviInfoTuple);
            _float3& thirdElement = get<2>(NaviInfoTuple);

            ImGui::BulletText("NaviPos1: (%.2f, %.2f, %.2f)", firstElement.x, firstElement.y, firstElement.z);
            ImGui::BulletText("NaviPos2: (%.2f, %.2f, %.2f)", secondElement.x, secondElement.y, secondElement.z);
            ImGui::BulletText("NaviPos3: (%.2f, %.2f, %.2f)", thirdElement.x, thirdElement.y, thirdElement.z);

            ImGui::TreePop();
        }
    }
    
    ImGui::EndChild();

    ImGui::End();
}

void CImgui_Manager::Object_Menu()
{
    ImGui::Begin("Object Menu", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

    if (m_bPickTerrain)
    {
        ImGui::Text("Terrain Picked");
        ImGui::SameLine();
        ImGui::Text("%.2f", m_PickingPos.x);
        ImGui::SameLine();
        ImGui::Text("%.2f", m_PickingPos.y);
        ImGui::SameLine();
        ImGui::Text("%.2f", m_PickingPos.z);
    }
    else
        ImGui::Text("Terrain not Picked");

    ImGui::Separator();
    ImGui::Spacing();

    if (!m_strSelectedName.empty()) {
        ImGui::Text("Selected Object: %ls", m_strSelectedName.c_str());
    }

    ImGui::Separator();
    ImGui::Spacing();
    ImGui::BeginChild("ObjectList", ImVec2(500, 300), true, ImGuiWindowFlags_HorizontalScrollbar);

    for (auto& pair : m_ObjectInfo)
    {
        wstring ObjName = pair.first;
        tuple<_float3, _float, _vector>& objectInfoTuple = pair.second;

        _float3& objSize = get<0>(objectInfoTuple);
        _float& objRadian = get<1>(objectInfoTuple);
        _vector& objPos = get<2>(objectInfoTuple);

        string ObjNameStr(ObjName.begin(), ObjName.end());

        if (ImGui::TreeNode(ObjNameStr.c_str()))
        {
            if (ImGui::IsItemClicked()) {
                m_strSelectedName = ObjName;
            }

            ImGui::BulletText("Object Size");
            ImGui::PushItemWidth(100);

            ImGui::InputFloat("SizeX", &objSize.x, 0.1f, objSize.x, "%.2f");
            ImGui::SameLine();
            ImGui::InputFloat("SizeY", &objSize.y, 0.1f, objSize.y, "%.2f");
            ImGui::SameLine();
            ImGui::InputFloat("SizeZ", &objSize.z, 0.1f, objSize.z, "%.2f");
            ImGui::Spacing();

            // ImGui를 사용하여 x, y, z 위치를 조정하는 부분
            ImGui::BulletText("Object Position");
            ImGui::PushItemWidth(150);

            ImGui::InputFloat("X", &objPos.m128_f32[0], 1.0f, objPos.m128_f32[0], "%.2f");
            ImGui::InputFloat("Y", &objPos.m128_f32[1], 1.0f, objPos.m128_f32[1], "%.2f");
            ImGui::InputFloat("Z", &objPos.m128_f32[2], 1.0f, objPos.m128_f32[2], "%.2f");

            // 회전
            ImGui::BulletText("Object Rotation");
            ImGui::InputFloat("Rotation", &objRadian, 0.1f, objRadian, "%.2f");

            ImGui::TreePop();
        }
    }
    ImGui::EndChild();
    
    ImGui::End();
}

vector<wstring> CImgui_Manager::DDSFileNames(const wstring& strPath)
{
    vector<wstring> fileNames;

    for (const auto& entry : fs::directory_iterator(strPath)) {
        if (entry.path().extension() == TEXT(".dds")) {
            fileNames.push_back(entry.path().filename().wstring());
        }
    }
    return fileNames;
}

vector<wstring> CImgui_Manager::StaticFbxFileNames(const wstring& strPath)
{
    vector<wstring> fileNames;

    for (const auto& entry : fs::directory_iterator(strPath)) {
        if (entry.path().extension() == TEXT(".fbx")) {
            fileNames.push_back(entry.path().filename().wstring());
        }
    }
    return fileNames;
}

void CImgui_Manager::Free()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    Safe_Release(m_pObjectManager);
    Safe_Release(m_pNaviManager);
    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
}
