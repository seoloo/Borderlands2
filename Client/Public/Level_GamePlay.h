#pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)

class CLevel_GamePlay final : public CLevel
{
private:
	CLevel_GamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_GamePlay() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Lights();
	HRESULT Ready_Layer_Camera(const wstring& strLayerTag);
	HRESULT Ready_Layer_BackGround(const wstring& strLayerTag);
	HRESULT Ready_Layer_Effect(const wstring& strLayerTag);
	HRESULT Ready_Layer_UI(const wstring& strLayerTag);

public:
	void Create_Wave();
	void Create_Skag();
	void Create_SpiderAnt();
	void Create_Thresher();

private:
	HRESULT Ready_Layer_IceBlock01(const wstring& strLayerTag);
	HRESULT Ready_Layer_Large_Rock1(const wstring& strLayerTag);
	HRESULT Ready_Layer_DamWall(const wstring& strLayerTag);
	HRESULT Ready_Layer_Container(const wstring& strLayerTag);
	HRESULT Ready_Layer_TunnelDoor(const wstring& strLayerTag);
	HRESULT Ready_Layer_CircularSupport(const wstring& strLayerTag);
	HRESULT Ready_Layer_House(const wstring& strLayerTag);
	HRESULT Ready_Layer_MetalBeam(const wstring& strLayerTag);
	HRESULT Ready_Layer_MetalLadder(const wstring& strLayerTag);
	HRESULT Ready_Layer_Pillar(const wstring& strLayerTag);
	HRESULT Ready_Layer_CrateSet(const wstring& strLayerTag);
	HRESULT Ready_Layer_WaterTower(const wstring& strLayerTag);
	HRESULT Ready_Layer_BarCounter(const wstring& strLayerTag);
	HRESULT Ready_Layer_BedGarbage(const wstring& strLayerTag);
	HRESULT Ready_Layer_Boat(const wstring& strLayerTag);
	HRESULT Ready_Layer_Dropship(const wstring& strLayerTag);
	HRESULT Ready_Layer_DropzoneWall(const wstring& strLayerTag);
	HRESULT Ready_Layer_JakobsTank(const wstring& strLayerTag);
	HRESULT Ready_Layer_LynchwoodHotel(const wstring& strLayerTag);
	HRESULT Ready_Layer_Yurt(const wstring& strLayerTag);
	HRESULT Ready_Layer_Yurt2(const wstring& strLayerTag);
	


private:
	_uint m_iCount = { 0 };
	wstring m_ObjectName;

	_int m_iNumber = { 0 };
	_float m_fTimer = { 0.f };
	_float m_fCreateTimer = { 0.f };

public:
	static CLevel_GamePlay* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END