#pragma once

#include "Client_Defines.h"
#include "LandObject.h"

BEGIN(Engine)
class CModel;
class CShader;
class CRenderer;
class CTransform;
class CNavigation;
class CCollider;
class CTexture;

END

BEGIN(Client)
class CGoliath final : public CLandObject
{
public:
	typedef struct MonsterDesc {
		_vector CreatePos = { XMVectorSet(0.f,0.f,0.f,0.f) };
		_int	iNumber = { 0 };
	}MONSTERDESC;

	enum STATE{ STATE_APPEAR, STATE_IDLE, STATE_WALK, STATE_ATTACK, STATE_ATTACKED, STATE_DEAD, STATE_END};
	enum MOVEDIR { MOVE_FORWARD, MOVE_BACKWARD, MOVE_RIGHT, MOVE_LEFT, MOVE_END };

private: 
	CGoliath(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGoliath(const CGoliath& rhs);
	virtual ~CGoliath() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Appear_Mode(_float fTimeDelta);
	void Idle_Mode(_float fTimeDelta);
	void Walk_Mode(_float fTimeDelta);
	void Attack_Mode(_float fTimeDelta);
	void Attacked_Mode(_float fTimeDelta);
	void Dead_Mode(_float fTimeDelta);

private:
	CModel* m_pModelCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CTransform* m_pTransformCom = { nullptr };
	CTransform*						m_pPlayerTransformCom = { nullptr };
	CNavigation* m_pNavigationCom = { nullptr };
	CCollider* m_pColliderCom[COLLIDER_END] = { nullptr };
	CTexture* m_pTextureCom = { nullptr };

private:
	map<const wstring, CGameObject*>	m_Parts;

private:
	_float3 m_vPos = { 0.f,0.f,0.f };
	_float3 m_vPlayerPos = { 0.f,0.f,0.f };
	_vector m_Look = { XMVectorSet(0.f,0.f,0.f,0.f) };
	_bool	m_bAttackMode = { false };
	STATE	m_state;

	_int	m_iCount = { 0 };

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();
	HRESULT Collision_Object();
	HRESULT Add_Parts();

public:
	static CGoliath* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg)	override;
	virtual void Free() override;
};

END