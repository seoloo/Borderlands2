#pragma once
#include "Base.h"
#include "EffectTool_Defines.h"

BEGIN(Engine)
class CTransform;
class CTexture;
class CRenderer;
class CGameInstance;
END

BEGIN(EffectTool)

class CEffectToolMainApp final : public CBase
{
private:
	CEffectToolMainApp();
	virtual ~CEffectToolMainApp() = default;

public:
	HRESULT Initialize();
	void Tick(_float fTimeDelta);
	HRESULT Render();

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	CGameInstance* m_pGameInstance = { nullptr };
	CRenderer* m_pRenderer = { nullptr };

private:
	class CEffectImgui_Manager* m_pImgui_Manager = { nullptr };

private:
	/* 모든레벨에서 사용하기위한 컴포넌트 원형을 생성한다. */
	HRESULT Ready_Prototype_Component_ForStatic();
	HRESULT Ready_Prototype_Object_ForStatic();

private:
	HRESULT	Add_Layer();
	HRESULT Ready_Lights();
	HRESULT	Ready_Layer_Camera(const wstring& strLayerTag);
	HRESULT	Ready_Layer_Terrain(const wstring& strLayerTag);
	HRESULT Ready_Layer_Effect(const wstring& strLayerTag);

private:
	HRESULT Falling_Snow();
	HRESULT Bleeding();
public:
	static CEffectToolMainApp* Create();
	virtual void Free() override;
};
END
