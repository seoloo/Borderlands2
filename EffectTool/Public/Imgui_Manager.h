#pragma once
#include "Base.h"
#include "Tool_Defines.h"
#include "Tool_Object.h"

BEGIN(Tool)

class CImgui_Manager final : public CBase
{
	DECLARE_SINGLETON(CImgui_Manager)
private:
	CImgui_Manager();
	virtual ~CImgui_Manager() = default;

public:
	HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void	Tick();
	void	Render();

public:
	void	Menu();
	void	Menu_Window();
	void	Position_Menu();
	void	Navi_Menu();
	void	Object_Menu();

	vector<wstring> DDSFileNames(const wstring& strPath);
	vector<wstring> StaticFbxFileNames(const wstring& strPath);

#pragma region Get, Set ÇÔ¼ö
public:
	wstring Get_TerrainName() { return m_strTerrainName; };
	wstring Get_FBXName() { return m_strFBXName; };
	wstring Get_ObjectName() { return m_strObjectName; };
	wstring Get_SelectedName() { return m_strSelectedName; };
	_bool	Get_Apply() { return m_bApply; };
	_bool	Get_Save() { return m_bSave; };
	_bool	Get_Load() { return m_bLoad; };
	_bool	Get_Fix() { return m_bFix; };
	_bool	Get_NaviFix1() { return m_bNaviFix1; };
	_bool	Get_NaviFix2() { return m_bNaviFix2; };
	_bool	Get_NaviFix3() { return m_bNaviFix3; };
	_bool	Get_NaviSave() { return m_bNaviSave; };
	_bool	Get_Create() { return m_bCreate; };
	_float3 Get_PickingPos() { return m_PickingPos; };
	map<wstring, tuple<_float3, _float, _vector>> Get_ObjectMap() { return m_ObjectInfo; };
	map<wstring, tuple<_float3, _float3, _float3>> Get_NaviVector() { return m_NaviMap; };

public:
	void	Set_PickTerrain(_bool bCheck) { m_bPickTerrain = bCheck; };
	void	Set_Apply(_bool bCheck) { m_bApply = bCheck; };		/* for. Terrain */
	void	Set_Create(_bool bCheck) { m_bCreate = bCheck; };	/* for. Object */
	void	Set_Save(_bool bSave) { m_bSave = bSave; };		
	void	Set_NaviSave(_bool bSave) { m_bNaviSave = bSave; };		
	void	Set_Fix(_bool bFix) { m_bFix = bFix; };
	void	Set_Load(_bool bLoad) { m_bLoad = bLoad; };
	void	Set_PickingPos(_float3 fPos) { m_PickingPos = fPos; };
	void	Set_ToolObjectName(wstring str) { m_strToolObjectName = str; };
	void	Set_Radian(_float Radian) { m_fRadian = Radian; };
	void	Set_Pos(_vector Pos) { m_CreatePos = Pos; };
	void	Set_Size(_float3 Size) { m_ObjectSize = Size; };
#pragma endregion

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	class CTool_ObjectManager* m_pObjectManager = { nullptr };
	class CNavi_Manager* m_pNaviManager = { nullptr };
private:
	wstring	m_strFBXName;
	wstring	m_strObjectName;
	wstring	m_strTerrainName;
	wstring	m_strSelectedName;
	wstring m_strToolObjectName;

	_bool	m_bObject_Menu = { false };
	_bool	m_bNavi_Menu = { false };
	_bool	m_bPosition_Menu = { false };

	_bool	m_bPickTerrain = { false };
	_bool	m_bCreate = { false };
	_bool	m_bApply = { false };
	_bool	m_bSave = { false };
	_bool	m_bFix = { false };
	_bool	m_bLoad = { false };
	_bool	m_bNaviFix1 = { false };
	_bool	m_bNaviFix2 = { false };
	_bool	m_bNaviFix3 = { false };
	_bool	m_bNaviSave = { false };
	_bool	m_bStoreNavi = { false };

	_uint	m_iObjectCount = { 0 };
	_uint	m_iNaviCount = { 0 };

	_float3 m_PickingPos = { 0.0f, 0.0f, 0.0f };

	map<wstring, _uint> m_ObjectCounter;

	map<wstring, tuple<_float3, _float, _vector>> m_ObjectInfo;

	map<wstring, tuple<_float3, _float3, _float3>> m_NaviMap;

	_float3	m_ObjectSize = { 1.f, 1.f, 1.f };
	_float	m_fRadian = { 0.f };
	_vector	m_CreatePos = { XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f) };

private:
	string wstringToString(const wstring& wstr)
	{
		string str;
		str.reserve(wstr.size());
		for (wchar_t wc : wstr)
		{
			str.push_back(static_cast<char>(wc));
		}
		return str;
	}

public:
	virtual void Free() override;
};

END