#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
END

BEGIN(Client)

class CMissionFont final : public CGameObject
{
private:
	CMissionFont(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMissionFont(const CMissionFont& rhs);
	virtual ~CMissionFont() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	HRESULT	Town_Mission();
	HRESULT	GamePlay_Mission();
	HRESULT Ending_Mission();

private:
	CRenderer*		m_pRendererCom = { nullptr };

private:
	HRESULT Add_Components();

private:
	_float							m_fX, m_fY, m_fSizeX, m_fSizeY;
	_float4x4						m_WorldMatrix, m_ViewMatrix, m_ProjMatrix;

	wstring	m_strGuide;
	wstring	m_strMission;
	_float4 m_Color;
	_float4 m_ClearColor;

	_bool	m_bFirstMissionClear = { false };
	_bool	m_bSecondMissionClear = { false };
	_bool	m_bThirdMissionClear = { false };

public:
	static CMissionFont* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg)	override;
	virtual void Free() override;
};

END