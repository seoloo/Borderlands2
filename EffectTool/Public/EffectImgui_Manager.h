#pragma once
#include "Base.h"
#include "EffectTool_Defines.h"

BEGIN(EffectTool)
class CEffectImgui_Manager final : public CBase
{
	DECLARE_SINGLETON(CEffectImgui_Manager)

public:
	typedef struct tagEffectInfo {
		wstring	strName;
		_int	iNumber = { 0 };
		_float	fLifeTime = { 0.f };
		_float4	vPivotPos = { _float4(0.f,0.f,0.f,0.f) };
		_float3	vRange = { 0.f,0.f,0.f };
		_float	fMinSpeed = { 0.f };
		_float	fMaxSpeed = { 0.f };
		_bool	bFalling = { false };
		_bool	bRising = { false };
		_bool	bExplosion = { false };
		_bool	bBleeding = { false };
		_bool	bDig = { false };
		_bool	bGreen = { false };
	}EFFECTINFO;

private:
	CEffectImgui_Manager();
	virtual ~CEffectImgui_Manager() = default;

public:
	HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void	Tick();
	void	Render();

public:
	void	Menu();
	void	Menu_Window();
	void	Effect_List();
	HRESULT	Write_File();
	HRESULT	Save_File();
	HRESULT	Save(const wchar_t* filePath);
	HRESULT	Load_File();
	HRESULT	Load(const wchar_t* filePath);

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

private:
	wstring	m_strPNGName;
	wstring	m_strTypeName;

	_bool	m_bCreate = { false };
	_bool	m_bApply = { false };
	_bool	m_bSave = { false };
	_bool	m_bLoad = { false };

	/*_uint	m_iNumber = { 0 };
	_float	m_fLifeTime = { 0.f };
	_float4	m_vPivotPos = { _float4(0.f,0.f,0.f,0.f) };
	_float3	m_vRange = { 0.f,0.f,0.f };
	_float	m_fMinSpeed = { 0.f };
	_float	m_fMaxSpeed = { 0.f };
	_bool	m_bRising = { false };
	_bool	m_bFalling = { false };*/

	_int	m_iSelectType = { 0 };

	EFFECTINFO	m_EffectInfo;

	list<tuple<_int, _float, _float4, _float3, _float,_float,_bool,_bool,_bool>> m_EffectList;

public:
	wstring Get_PNGName() { return m_strPNGName; };
	_bool	Get_Create() { return m_bCreate; };
	_bool	Get_Load() { return m_bLoad; };

	/*_uint	Get_Number() { return m_iNumber; };
	_float	Get_LifeTime() { return m_fLifeTime; };
	_float4	Get_PivotPost() { return m_vPivotPos; };
	_float3	Get_Range() { return m_vRange; };
	_float	Get_MinSpeed() { return m_fMinSpeed; };
	_float	Get_MaxSpeed() { return m_fMaxSpeed; };
	_bool	Get_Rising() { return m_bRising; };
	_bool	Get_Falling() { return m_bFalling; };*/

	EFFECTINFO Get_EffectInfo() { return m_EffectInfo; };

public:
	void Set_Create(_bool bCreate) { m_bCreate = bCreate; };
	void Set_Load(_bool bLoad) { m_bLoad = bLoad; };

public:
	vector<wstring> PNGFileNames(const wstring& strPath);

public:
	virtual void Free() override;
};

END