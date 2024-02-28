#pragma once
#include "Base.h"
#include "Tool_Defines.h"

BEGIN(Engine)
class CGameObject;
END

BEGIN(Tool)
class CTool_ObjectManager final: public CBase
{
	DECLARE_SINGLETON(CTool_ObjectManager)

public:
	typedef struct tagOBJECTDESC {
		wstring strName;
		_float3	ObjectSize = { 1.f, 1.f, 1.f };
		_float	fRadian = { 0.f };
		_vector	CreatePos = { XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f) };
		_bool	bSave = { false };
	}OBJECTDESC;

private:
	CTool_ObjectManager();
	virtual ~CTool_ObjectManager() = default;
public:
	HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void	Tick(_float _fTimeDelta);
	void	Late_Tick(_float _fTimeDelta);
	HRESULT	Add_Object(void* pArg = nullptr);

public:
	HRESULT	Save_Data(HANDLE handle);
	HRESULT	Load_Data(HANDLE handle);

public:
	_bool	Get_Save() { return m_bSave; };
	void	Set_Save(_bool bSave) { m_bSave = bSave; };

private:
	list<tuple<wstring, _float3, _vector, _float>> m_ObjectList;
	OBJECTDESC m_ObjectDesc;
	class CImgui_Manager* m_pImgui_Manager = { nullptr };
	_bool	m_bSave = { false };
public:
	virtual void Free() override;
};

END