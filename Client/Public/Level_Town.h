#pragma once
#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)

class CLevel_Town :
    public CLevel
{
private:
	CLevel_Town(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Town() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Lights();
	HRESULT Ready_Layer_Camera(const wstring& strLayerTag);
	HRESULT Ready_Layer_BackGround(const wstring& strLayerTag);

	HRESULT Ready_Layer_DamWall(const wstring& strLayerTag);
	HRESULT Ready_Layer_MetalBeam(const wstring& strLayerTag);
	HRESULT Ready_Layer_MetalLadder(const wstring& strLayerTag);
	HRESULT Ready_Layer_WaterTower(const wstring& strLayerTag);
	HRESULT Ready_Layer_Dropship(const wstring& strLayerTag);
	HRESULT Ready_Layer_Yurt(const wstring& strLayerTag);
	HRESULT Ready_Layer_Yurt2(const wstring& strLayerTag);

private:
	_uint m_iCount = { 0 };
	wstring m_ObjectName;
	_float m_fTimer = { 0.f };
	_float m_fTimer2 = { 0.f };

	_uint m_iNumber = { 0 };
	_uint m_iEffectCount = { 0 };
	_uint m_iInteraction = { 0 };
public:
	static CLevel_Town* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END