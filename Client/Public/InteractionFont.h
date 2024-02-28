#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
END

BEGIN(Client)

class CInteractionFont final : public CGameObject
{
private:
	CInteractionFont(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CInteractionFont(const CInteractionFont& rhs);
	virtual ~CInteractionFont() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT	Town_NPCFont();
	HRESULT	Town_ItemFont();
	HRESULT	GamePlay_NPCFont();
	HRESULT	GamePlay_ItemFont();

private:
	CRenderer*		m_pRendererCom = { nullptr };

private:
	HRESULT Add_Components();

private:
	_float							m_fX, m_fY, m_fSizeX, m_fSizeY;
	_float4x4						m_WorldMatrix, m_ViewMatrix, m_ProjMatrix;

	wstring	m_strNPC;
	wstring	m_strItem;

public:
	static CInteractionFont* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg)	override;
	virtual void Free() override;
};

END