#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
END

BEGIN(Client)

class CThresherUI final : public CGameObject
{
private:
	CThresherUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CThresherUI(const CThresherUI& rhs);
	virtual ~CThresherUI() = default;

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

	_bool m_bDead = { false };

public:
	static CThresherUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg)	override;
	virtual void Free() override;
};

END