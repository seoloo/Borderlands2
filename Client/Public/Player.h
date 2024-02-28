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
END

BEGIN(Client)

class CPlayer final : public CLandObject
{
public:
	typedef struct tagPlayerInfo {
		_vector CreatePos = { XMVectorSet(0.f,0.f,0.f,0.f) };
	}PLAYERINFO;

	enum Movestate { MOVE_FOWARD, MOVE_RIGHT, MOVE_LEFT, MOVE_BACKWARD, MOVE_END };
private:
	CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer(const CPlayer& rhs);
	virtual ~CPlayer() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual HRESULT Render_ShadowDepth();

	void	Key_State(_float fTime);
	void	Walk_Sound(_float fTimeDelta);

private:
	CModel*			m_pModelCom = { nullptr };
	CShader*		m_pShaderCom = { nullptr };	
	CRenderer*		m_pRendererCom = { nullptr };
	CTransform*		m_pTransformCom = { nullptr };
	CCollider*		m_pColliderCom[COLLIDER_END] = { nullptr };
	CNavigation*	m_pNavigationCom = { nullptr };

	map<const wstring, CGameObject*>	m_Parts;

private:
	Movestate m_MoveState;
	_vector	m_vPos = { XMVectorSet(0.f,0.f,0.f,1.f) };
	_bool m_bZoom = { false };
	_float	m_fShieldTimer = { 0.f };
	_float	m_fBulletTimer = { 0.f };
	_float	m_fShieldRecoveryTimer = { 0.f };

	_bool	m_bShieldRecovery = { false };
	_bool	m_bShieldZero = { false };
	_bool	m_bRun = { false };

	_bool	m_fDestroyUI = { 0.f };

public:
	_bool Get_Zoom() { return m_bZoom; };

private:
	HRESULT Add_Components();
	HRESULT Add_PlayerParts();
	HRESULT Bind_ShaderResources();
	HRESULT Collision_Object();

public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg)	override;
	virtual void Free() override;
};

END