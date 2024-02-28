#pragma once

#include "Tool_Defines.h"
#include "Level.h"

BEGIN(Tool)

class CLevel_ToolLoading final : public CLevel
{
private:
	CLevel_ToolLoading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_ToolLoading() = default;

public:
	virtual HRESULT Initialize(LEVEL eNextLevel);
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	LEVEL					m_eNextLevel = { LEVEL_END };
	class CTool_Loader*		m_pLoader = { nullptr };

public:
	static CLevel_ToolLoading* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevel);
	virtual void Free() override;
};

END