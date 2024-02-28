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
class CThresher_State;

class CThresher : public CLandObject
{
public:
	enum STATES { STATE_FIRSTAPPEAR, STATE_SECONDAPPEAR, STATE_IDLE, STATE_APPEAR, STATE_DIG, STATE_BURY, STATE_ATTACK, STATE_SUMMON, STATE_DEAD, STATE_END };

private: 
	CThresher(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CThresher(const CThresher& rhs);
	virtual ~CThresher() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void	ChangeState(STATES states);

private:
	map<const wstring, CGameObject*>	m_Parts;
	STATES	m_States;

private:
	CModel* m_pModelCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CTransform* m_pTransformCom = { nullptr };
	CTransform*	m_pPlayerTransformCom = { nullptr };
	CNavigation* m_pNavigationCom = { nullptr };
	CCollider* m_pColliderCom[COLLIDER_END] = { nullptr };

private:
	CThresher_State* m_pThresher_State = { nullptr };

private:
	_bool m_bArmDead = { false };
	_bool m_bArm2Dead = { false };
	_bool m_bMeleeDead = { false };

public:
	void Set_ArmDead(_bool bDead) { m_bArmDead = bDead; };
	void Set_Arm2Dead(_bool bDead) { m_bArm2Dead = bDead; };
	void Set_MeleeDead(_bool bDead) { m_bMeleeDead = bDead; };

	_bool Get_ArmDead() { return m_bArmDead; };
	_bool Get_Arm2Dead() { return m_bArm2Dead; };
	_bool Get_MeleeDead() { return m_bMeleeDead; };

private:
	HRESULT Add_Components();
	HRESULT Add_Parts();
	HRESULT Bind_ShaderResources();

public:
	static CThresher* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg)	override;
	virtual void Free() override;
};

END