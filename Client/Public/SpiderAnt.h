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
class CSpiderAnt_State;

class CSpiderAnt  : public CLandObject
{
public:
	enum STATE { STATE_DESCEND, STATE_IDLE, STATE_WALK, STATE_ATTACK, 
		STATE_SHOOT, STATE_SHOOT2, STATE_DIG, STATE_APPEAR, STATE_ATTACKED, STATE_DEAD, STATE_END };

private: 
	CSpiderAnt(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSpiderAnt(const CSpiderAnt& rhs);
	virtual ~CSpiderAnt() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Change_State(STATE state);

public:
	_bool Get_Collided() { return m_bCollided; };
	void	Set_Collided(_bool bCollided) { m_bCollided = bCollided; };
	void	Set_Dissolve(_bool bTrue) { m_isDissolve = bTrue; };
private:
	CModel* m_pModelCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CTransform* m_pTransformCom = { nullptr };
	CNavigation* m_pNavigationCom = { nullptr };
	CTransform*	m_pPlayerTransformCom = { nullptr };
	CCollider* m_pColliderCom[COLLIDER_END] = { nullptr };
	CTexture* m_pTextureCom = { nullptr };

	map<const wstring, CGameObject*>	m_Parts;

private:
	CSpiderAnt_State* m_pSpiderAnt_State = { nullptr };

private:
	_float3 m_vPos = { 0.f,0.f,0.f };
	_float3 m_vPlayerPos = { 0.f,0.f,0.f };
	_vector m_Look = { XMVectorSet(0.f,0.f,0.f,0.f) };
	_bool	m_bAttackMode = { false };
	_bool	m_bCollided = { false };
	_bool	m_isDissolve = { false };
private:
	HRESULT Add_Components();
	HRESULT Add_Parts();
	HRESULT Collision_Object();
	HRESULT Bind_ShaderResources();

public:
	static CSpiderAnt* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg)	override;
	virtual void Free() override;
};

END