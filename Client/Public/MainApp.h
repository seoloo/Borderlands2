#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CTransform;
class CTexture;
class CRenderer;
class CGameInstance;
END

BEGIN(Client)

class CMainApp final : public CBase
{
private:
	CMainApp();
	virtual ~CMainApp() = default;

public:
	HRESULT Initialize();
	void Tick(_float fTimeDelta);
	HRESULT Render();

private:
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };
	CGameInstance*				m_pGameInstance = { nullptr };
	CRenderer*					m_pRenderer = { nullptr };

#ifdef _DEBUG
private:
	wstring						m_strFPS;
	_uint						m_iRenderCnt = { 0 };
	_float						m_fTimeAcc = { 0.0f };

#endif

private:
	HRESULT Open_Level(LEVEL eLevel);
	/* 모든레벨에서 사용하기위한 컴포넌트 원형을 생성한다. */
	HRESULT Ready_Prototype_Component_ForStatic();
	HRESULT Ready_Prototype_Collider_ForStatic();

public:
	static CMainApp* Create();
	virtual void Free() override;	
};

END

