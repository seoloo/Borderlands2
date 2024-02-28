#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CGameObject abstract : public CBase
{
public:
	enum COLLIDER { COLLIDER_AABB, COLLIDER_OBB, COLLIDER_SPHERE, COLLIDER_END };
protected:
	CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject(const CGameObject& rhs);
	virtual ~CGameObject() = default;

public:
	class CComponent* Find_Component(const wstring& strComTag);

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual HRESULT Render_Shadow() { return S_OK; };

protected:
	ID3D11Device*			m_pDevice = { nullptr }; 
	ID3D11DeviceContext*	m_pContext = { nullptr };

protected:
	map<wstring, class CComponent*>			m_Components;

protected:
	HRESULT Add_Component(_uint iLevelIndex, const wstring& pPrototypeTag, const wstring& strComTag, CComponent** ppOut, void* pArg = nullptr);

protected:
	CGameObject* m_pObject = { nullptr };
	CGameObject* m_pPlayer = { nullptr };
	class  CTransform* m_pPlayerTransform = { nullptr };

	_bool	m_bDead = { false };		/* ��� ���� */
	_bool	m_bDeathAnim = { false };	/* ��� �ִϸ��̼�*/
	_bool	m_bCollision = { false };	/* �浹 ���� */
	_bool	m_bAttacked = { false };	/* �ǰ� ���� */
	_float	m_fTimer = { 0.f };			/* Ÿ�̸� */
	_float	m_fDeadTimer = { 0.f };		/* ���� Ÿ�̸� */
	_float	m_fDissolveTimer = { 0.f };	/* ������ Ÿ�̸� */
	_float	m_fSoundTimer = { 0.f };	/* ���� Ÿ�̸� */
	_int	m_iMaxHp = { 0 };			/* �ִ� ü��*/
	_int	m_iHp = { 0 };				/* ü�� */
	_int	m_iShield = { 0 };			/* ����*/
	_int	m_iObjectHp = { 0 };		/* ������Ʈ ü�� */
	_int	m_iAtt = { 0 };				/* ���ݷ� */
	_int	m_iHealthVial = { 100 };
	_int	m_iMonsterAtt = { 0 };
	_float	m_fDissolveValue = { 0.f }; /* ������ �� */
	_int	m_iCounter = { 0 };

public:
	void Set_Dead() { m_bDead = true; };
	void Set_DeathAnim(_bool bDeathAnim) { m_bDeathAnim = bDeathAnim; };
	void Set_Hp(_int iHp) { m_iHp = iHp; };
	void Set_Shield(_int iShield) { m_iShield = iShield; };
	void Decrease_Hp(_int iAttack) { m_iHp -= iAttack; };
	void Decrease_Shield(_int iAttack) { m_iShield -= iAttack; };
	void Set_Attacked(_bool bHeadAttack) { m_bAttacked = bHeadAttack; };
	void Set_MonsterAtt(_int iAtt) { m_iMonsterAtt = iAtt; };
	void Increase_Hp(_int iHp) { m_iHp += iHp; };
	void Increase_Shield(_int iShield) { m_iShield += iShield; };

public:
	_int Get_Hp() { return m_iHp; }; 
	_int Get_Shield() { return m_iShield; };
	_int Get_MaxHp() { return m_iMaxHp; }; 
	_int Get_Demage() { return m_iAtt; };
	_bool Get_Dead() { return m_bDead; };
	_int Get_HealthVial() { return m_iHealthVial; };
	_bool Get_Attacked(){ return m_bAttacked; };
	_bool Get_DeathAnim() { return m_bDeathAnim; };
public:	
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END