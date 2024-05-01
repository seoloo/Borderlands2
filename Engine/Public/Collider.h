#pragma once

/* �浹ü�� �����ϰ� �����ش�. */
/* �浹ü�� ������ Ȱ���Ͽ� ���� �浹�� �����Ѵ�. */

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CCollider final : public CComponent
{
public:
	typedef struct tagColliderDesc
	{
		_float3		vPosition;
		_float3		vSize;
		_float3		vRotation;
		_float		fRadius;
	} COLLIDERDESC;

public:
	enum TYPE { TYPE_AABB, TYPE_OBB, TYPE_SPHERE, TYPE_END };
private:
	CCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCollider(const CCollider& rhs);
	virtual ~CCollider() = default;

public:
	virtual HRESULT Initialize_Prototype(TYPE eType);
	virtual HRESULT Initialize(void* pArg);
	void Tick(_fmatrix WorldMatrix);

public:
	_bool Collision(class CCollider* pTargetCollider);
	_bool Collision_Mouse();

#ifdef _DEBUG
public:
	virtual HRESULT Render() override;
#endif

private:
	TYPE							m_eType = TYPE_END;
	class CBounding* m_pBounding = { nullptr };
	_bool							m_bCollision = { false };

#ifdef _DEBUG
private:
	PrimitiveBatch<VertexPositionColor>*		m_pBatch = { nullptr };			/* ���� ������ �׸��µ� ���*//* ������ ��ġ�� ���� ������ ����*/
	BasicEffect*								m_pBaseEffect = { nullptr };	/* �𵨰� �ؽ�ó ó���� ���� ����� ���� */
	ID3D11InputLayout* m_pInputLayout = { nullptr };							/* ���� �����͸� �׷��Ƚ� �������������� ���� �� ������ ������ ���̾ƿ��� �����ϴ� ���� */
#endif

public:
	static CCollider* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eType);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END