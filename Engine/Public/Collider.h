#pragma once

/* 충돌체를 생성하고 보여준다. */
/* 충돌체의 정보를 활용하여 실제 충돌을 구현한다. */

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
	PrimitiveBatch<VertexPositionColor>*		m_pBatch = { nullptr };			/* 기하 도형을 그리는데 사용*//* 정점의 위치와 색상 정보를 담음*/
	BasicEffect*								m_pBaseEffect = { nullptr };	/* 모델과 텍스처 처리를 위한 기능을 제공 */
	ID3D11InputLayout* m_pInputLayout = { nullptr };							/* 정점 데이터를 그래픽스 파이프라인으로 보낼 떄 정점의 구조와 레이아웃을 정의하는 역할 */
#endif

public:
	static CCollider* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eType);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END