#pragma once
#include "Base.h"
#include "Tool_Defines.h"

BEGIN(Engine)
class CGameObject;
END

BEGIN(Tool)
class CNavi_Manager final: public CBase
{
	DECLARE_SINGLETON(CNavi_Manager)

public:
	typedef struct tageNaviInfo {
		wstring strName;
		_float3	NaviPos[3];
		_bool	bSave = { false };
	}NAVIINFO;

private:
	CNavi_Manager();
	virtual ~CNavi_Manager() = default;
public:
	HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void	Tick(_float _fTimeDelta);
	void	Late_Tick(_float _fTimeDelta);
	HRESULT	Add_Navi(void* pArg = nullptr);

public:
	_bool	Get_NaviSave() { return m_bSave; };
	void	Set_NaviSave(_bool bSave) { m_bSave = bSave; };

private:
	list<tuple<wstring, _float3, _float3, _float3>> m_ListNavi;
	NAVIINFO m_NaviInfo;
	class CImgui_Manager* m_pImgui_Manager = { nullptr };
	_bool	m_bSave = { false };

public:
	virtual void Free() override;
};

END