#pragma once

#include "Client_Defines.h"
#include "LandObject.h"

BEGIN(Engine)
class CModel;
class CShader;
class CRenderer;
class CTransform;
END

BEGIN(Client)
class CThresher_Arm : public CLandObject
{
public:
	typedef struct ArmDesc {
		_vector CreatePos = { XMVectorSet(0.f,0.f,0.f,1.f) };
		_uint Numbering = { 0 };
	}ARMDESC;

	enum STATE { STATE_APPEAR, STATE_ATTACK, STATE_BURY, STATE_END };

private: 
	CThresher_Arm(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CThresher_Arm(const CThresher_Arm& rhs);
	virtual ~CThresher_Arm() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void State_Appear(_float fTimeDelta);
	void State_Attack(_float fTimeDelta);
	void State_Bury(_float fTimeDelta);

public:
	_uint Get_Numbering() { return m_iNumbering; };
	void Set_ArmsDead(_bool bDead) { m_bArmsDead = bDead; };
private:
	map<const wstring, CGameObject*>	m_Parts;
	STATE m_state;
	_uint m_iRandomAttack = { 0 };
	_uint m_iRandomBury = { 0 };
	_uint m_iNumbering = { 0 };
	_bool m_bArmsDead = { false };

private:
	CModel* m_pModelCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CTransform* m_pTransformCom = { nullptr };
	CTransform*	m_pPlayerTransformCom = { nullptr };

private:
	HRESULT Add_Components();
	HRESULT Add_Parts();
	HRESULT Bind_ShaderResources();

public:
	static CThresher_Arm* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg)	override;
	virtual void Free() override;
};

END