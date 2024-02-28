#pragma once
#include "Base.h"
#include "Tool_Defines.h"

BEGIN(Engine)
class CRenderer;
class CGameInstance;
END

BEGIN(Tool)

class CToolMainApp final : public CBase
{
private:
	CToolMainApp();
	virtual ~CToolMainApp() = default;

public:
	HRESULT Initialize();
	void Tick(_float fTimeDelta);
	HRESULT Render();

private:
	class CImgui_Manager* m_pImgui_Manager = { nullptr };
	class CTool_ObjectManager* m_pObject_Manager = { nullptr };
	CGameInstance* m_pGameInstance = { nullptr };
	CRenderer* m_pRenderer = { nullptr };
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

public:
	ID3D11Device* Get_Device() { return m_pDevice; };
	ID3D11DeviceContext* Get_Context() { return m_pContext; };
private:
	HRESULT	Open_Level(LEVEL eNextLevel);
	HRESULT Ready_Prototype_Component_ForStatic();
	HRESULT Ready_Prototype_Object_ForStatic();

	HRESULT Ready_Lights();
	HRESULT	Ready_Layer_Camera(const wstring& strLayerTag);
	HRESULT	Ready_Layer_Terrain(const wstring& strLayerTag);

	void Add_Layer();

	HRESULT	Ready_Layer_IceBlock01(const wstring& strLayerTag);
	HRESULT	Ready_Layer_Large_Rock1(const wstring& strLayerTag);
	HRESULT	Ready_Layer_Bench(const wstring& strLayerTag);
	HRESULT	Ready_Layer_CircularSupport(const wstring& strLayerTag);
	HRESULT	Ready_Layer_Container(const wstring& strLayerTag);
	HRESULT	Ready_Layer_Crate(const wstring& strLayerTag);
	HRESULT	Ready_Layer_CrateSet(const wstring& strLayerTag);
	HRESULT	Ready_Layer_House(const wstring& strLayerTag);
	HRESULT	Ready_Layer_Rock1(const wstring& strLayerTag);
	HRESULT	Ready_Layer_TunnelDoor(const wstring& strLayerTag);
	HRESULT	Ready_Layer_WaterTower(const wstring& strLayerTag);
	HRESULT	Ready_Layer_Pillar(const wstring& strLayerTag);
	HRESULT	Ready_Layer_Yurt(const wstring& strLayerTag);
	HRESULT	Ready_Layer_Yurt2(const wstring& strLayerTag);
	HRESULT	Ready_Layer_MetalBeam(const wstring& strLayerTag);
	HRESULT	Ready_Layer_MetalLadder(const wstring& strLayerTag);
	HRESULT	Ready_Layer_DamWall(const wstring& strLayerTag);
	HRESULT	Ready_Layer_BarCounter(const wstring& strLayerTag);
	HRESULT	Ready_Layer_BedGarbage(const wstring& strLayerTag);
	HRESULT	Ready_Layer_Dropship(const wstring& strLayerTag);
	HRESULT	Ready_Layer_JakobsTank(const wstring& strLayerTag);
	HRESULT	Ready_Layer_DropzoneWall(const wstring& strLayerTag);
	HRESULT	Ready_Layer_LynchwoodHotel(const wstring& strLayerTag);
	HRESULT	Ready_Layer_Boat(const wstring& strLayerTag);
	HRESULT	Ready_Layer_ForkLift(const wstring& strLayerTag);

private:
	_uint m_iCount = { 0 };
	wstring m_ObjectName;
public:
	static CToolMainApp* Create();
	virtual void Free() override;
};

END