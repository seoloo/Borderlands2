#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
END

BEGIN(Client)

class CUIFont final : public CGameObject
{
private:
	CUIFont(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIFont(const CUIFont& rhs);
	virtual ~CUIFont() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CRenderer*		m_pRendererCom = { nullptr };

private:
	HRESULT Add_Components();

private:
	_float							m_fX, m_fY, m_fSizeX, m_fSizeY;
	_float4x4						m_WorldMatrix, m_ViewMatrix, m_ProjMatrix;

	_int m_iPlayerHp = { 0 };
	_int m_iPlayerShield = { 0 };

	wstring	m_strLevel;
	wstring	m_strPlayerName;
	wstring	m_strPlayerHp;
	wstring	m_strShield;
	wstring	m_strGun;

public:
	static CUIFont* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg)	override;
	virtual void Free() override;
};

END