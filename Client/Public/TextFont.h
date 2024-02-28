#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
END

BEGIN(Client)

class CTextFont final : public CGameObject
{
private:
	CTextFont(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTextFont(const CTextFont& rhs);
	virtual ~CTextFont() = default;

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

	wstring	m_strDialogue;
	wstring	m_strMission;

	_int m_iCount = { 0 };
	_float4 m_vColor = { 0.f,0.f,0.f,0.f };

public:
	static CTextFont* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg)	override;
	virtual void Free() override;
};

END