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
class CSkag  : public CLandObject
{
public:
	typedef struct MonsterDesc {
		_vector CreatePos = { XMVectorSet(0.f,0.f,0.f,0.f) };
	}MONSTERDESC;

	enum STATE { STATE_IDLE, STATE_APPEAR, STATE_SHOUT, STATE_WALK, STATE_ATTACK, STATE_BITE, STATE_SHOOT, STATE_CLAW, STATE_ATTACKED, STATE_SUMMON, STATE_DEAD, STATE_END };

private: 
	CSkag(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSkag(const CSkag& rhs);
	virtual ~CSkag() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Idle_Mode(_float fTimeDelta);
	void Appear_Mode(_float fTimeDelta);
	void Shout_Mode(_float fTimeDelta);
	void Walk_Mode(_float fTimeDelta);
	void Attack_Mode(_float fTimeDelta);
	void Bite_Mode(_float fTimeDelta);
	void Claw_Mode(_float fTimeDelta);
	void Shoot_Mode(_float fTimeDelta);
	void Attacked_Mode(_float fTimeDelta);
	void Summon_Mode(_float fTimeDelta);
	void Dead_Mode(_float fTimeDelta);

private:
	CModel* m_pModelCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CTransform* m_pTransformCom = { nullptr };
	CTransform*	m_pPlayerTransformCom = { nullptr };
	CNavigation* m_pNavigationCom = { nullptr };
	CCollider* m_pColliderCom[COLLIDER_END] = { nullptr };
	CTexture* m_pTextureCom = { nullptr };

	map<const wstring, CGameObject*>	m_Parts;
private:
	_float3 m_vPos = { 0.f,0.f,0.f };
	_float3 m_vPlayerPos = { 0.f,0.f,0.f };

	_float4 m_vSkagPos = { 0.f,0.f,0.f,0.f };

	_vector m_Look = { XMVectorSet(0.f,0.f,0.f,0.f) };

	_bool	m_bAttackMode = { false };
	_bool	m_bAppearDone = { false };
	_bool	m_bShootModeDone = { false };

	_uint	m_iRandomAttack = { 0 };
	_uint	m_iAttacked = { 0 };

	_float	m_fShootTimer = { 0.f };
	_float	m_fAppearTimer = { 0.f };

	STATE	m_state;

	_int	m_iNum = { 0 };
	_int	m_iCount = { 0 };
	_int	m_iAttackedCount = { 0 };

private:
	HRESULT Add_Components();
	HRESULT Add_Parts();
	HRESULT Bind_ShaderResources();
	HRESULT Collision_Object();

public:
	static CSkag* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg)	override;
	virtual void Free() override;
};

END