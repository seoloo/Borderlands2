#pragma once

#include "Tool_Defines.h"
#include "Level.h"

BEGIN(Tool)

class CLevel_Tool final : public CLevel
{
private:
	CLevel_Tool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Tool() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;


private:
	HRESULT Ready_Lights();
	HRESULT	Ready_Layer_Terrain(const wstring& strLayerTag);
	HRESULT	Ready_Layer_Camera(const wstring& strLayerTag);
	HRESULT	Ready_Layer_Object(const wstring& strLayerTag);
	HRESULT	Ready_Layer_IceBlock01(const wstring& strLayerTag);
	HRESULT	Ready_Layer_IceBlock06(const wstring& strLayerTag);

private:
	class CImgui_Manager* m_pImgui_Manager = { nullptr };

	_uint m_iCount = { 0 };
	wstring ObjectName;

	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

public:
	static CLevel_Tool* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END