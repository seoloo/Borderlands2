#pragma once

#include "Base.h"

/* 클라이언트 개발자가 만든 각각의 레벨들의 공통적인 부모가된다. */

BEGIN(Engine)

class ENGINE_DLL CLevel abstract : public CBase
{
protected:
	CLevel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel() = default;

public:
	virtual HRESULT Initialize();
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

protected:
	ID3D11Device*			m_pDevice = { nullptr }; 
	ID3D11DeviceContext*	m_pContext = { nullptr };

public:
	virtual void Free() override;
};

END