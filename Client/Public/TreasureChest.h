#pragma once

#include "Client_Defines.h"
#include "LandObject.h"

BEGIN(Engine)
class CModel;
class CShader;
class CCollider;
class CRenderer;
class CTransform;
class CNavigation;
class CTexture;
END

BEGIN(Client)

class CTreasureChest final : public CLandObject
{
public:
	enum COLLIDER { COLLIDER_AABB, COLLIDER_OBB, COLLIDER_SPHERE, COLLIDER_END };
	enum STATES { STATES_CLOSE, STATES_OPENING, STATES_OPENED, STATES_END };
	
	struct tagItemDesc {
		_vector CreatePos = { XMVectorSet(0.f,0.f,0.f,0.f) };
	};

private:
	CTreasureChest(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTreasureChest(const CTreasureChest& rhs);
	virtual ~CTreasureChest() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

public:
	void	Close(_float fTimeDelta);
	void	Opening(_float fTimeDelta);
	void	Opened(_float fTimeDelta);

private:
	CModel*							m_pModelCom = { nullptr };
	CShader*						m_pShaderCom = { nullptr };	
	CRenderer*						m_pRendererCom = { nullptr };
	CTransform*						m_pTransformCom = { nullptr };
	CTransform*						m_pPlayerTransformCom = { nullptr };
	CCollider*						m_pColliderCom[COLLIDER_END] = { nullptr };
	CTexture*						m_pTextureCom = { nullptr };
	map<const wstring, CGameObject*>	m_Parts;

	_bool m_bCheck = { false };
	_float m_fValue = { 0 };
	STATES m_State;
	_int m_iCount = { 0 };
	_vector m_vPos = { XMVectorSet(0.f,0.f,0.f,0.f) };

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();
	HRESULT Collision_Object();
	HRESULT Add_Parts();

public:
	static CTreasureChest* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg)	override;
	virtual void Free() override;
};

END