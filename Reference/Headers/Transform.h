#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CTransform final : public CComponent
{
public:
	enum STATE { STATE_RIGHT, STATE_UP, STATE_LOOK, STATE_POSITION, STATE_END };

public:
	typedef struct tagTransformDesc
	{
		_float			fSpeedPerSec;
		_float			fRotationPerSec;

	}TRANSFORMDESC;

private:
	CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTransform(const CTransform& rhs);
	virtual ~CTransform() = default;

public:
	_vector Get_State(STATE eState);
	_float3 Get_Scaled();
	_matrix Get_WorldMatrix_Inverse() {
		return XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix));
	}
	_float4x4 Get_WorldMatrix() { return m_WorldMatrix; };
	_float4x4 Get_PartsWorldMatrix() { return m_PartsWorldMatrix; };
	_matrix Get_WorldMatrix_Float4x4() {
		return XMLoadFloat4x4(&m_WorldMatrix);
	}
	const _float4x4* Get_WorldMatrixPtr() {
		return &m_WorldMatrix;
	}
	_float Get_Speed() { return m_TransformDesc.fSpeedPerSec; };

public:
	void Set_Speed(_float fSpeed) { m_TransformDesc.fSpeedPerSec = fSpeed; };
	void Set_Acceleration(_float fSpeed) { m_fAcceleration = fSpeed; };
	void Set_State(STATE eState, _fvector vState);
	void Set_PartsWorldMatrix(_float4x4 WorldMat) { m_PartsWorldMatrix = WorldMat; };
	//void Set_vLastVelocity(_vector vVelocity) { m_vLastVelocity = vVelocity; };

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);

public:
	HRESULT Bind_ShaderResource(class CShader* pShader, const char* pConstantName);

public:
	void Go_Look(_vector Look, _float fTimeDelta, class CNavigation* pNavigation = nullptr);
	void Go_Straight(_float fTimeDelta, class CNavigation* pNavigation = nullptr);
	void Go_Backward(_float fTimeDelta, class CNavigation* pNavigation = nullptr);
	void Go_Left(_float fTimeDelta, class CNavigation* pNavigation = nullptr);
	void Go_Right(_float fTimeDelta, class CNavigation* pNavigation = nullptr);
	void Rotation(_fvector vAxis, _float fRadian);
	void Turn(_fvector vAxis, _float fTimeDelta);
	void Scaled(const _float3& vScale);
	void Chase(_fvector vTargetPos, _float fTimeDelta, _float fLimit = 0.f);
	void LookAt(_fvector vTargetPos);
	void ToolGo_Straight(_float fTimeDelta, class CNavigation* pNavigation = nullptr);
	void ToolGo_Backward(_float fTimeDelta, class CNavigation* pNavigation = nullptr);
	void ToolGo_Left(_float fTimeDelta, class CNavigation* pNavigation = nullptr);
	void ToolGo_Right(_float fTimeDelta, class CNavigation* pNavigation = nullptr);

private:
	TRANSFORMDESC				m_TransformDesc;

	// _vector XMVECTOR : SIMD������ ���� ��ȵ� Ÿ��. 
	// float4�� ��ŭ�� ������ ������. ���� ����� �׹��� ����(x) �ѹ��� ����(o)
	// Stack�޸𸮿� ����Ǿ���Ѵ�. 
	// �ּҸ� ���� ����(x)
	// _matrix XMMATRIX : SIMD������ ���� ��ȵ� Ÿ��.
	// XMVECTOR�� 4�� ��ŭ(float16��)�� ������ ������.Stack�޸𸮿� ����Ǿ���Ѵ�. 
	// �ּҸ� ���� ����(x)
	// _float4x4 XMFLOAT4X4 : ������ ���� ���. �ּҸ� ���� ����(o)

	_float4x4					m_WorldMatrix;
	_float4x4					m_PartsWorldMatrix;

	_float3				m_vFirstPos = { 0.f,0.f,0.f };
	_float3				m_vSecondPos = { 0.f,0.f,0.f };

	_float m_fAcceleration = 0.01f;
	_float m_fMaxAcceleration = 5.0f;
	//_vector m_vLastVelocity = XMVectorZero();

public:
	static CTransform* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END