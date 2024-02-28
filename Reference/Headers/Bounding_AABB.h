#pragma once

#include "Bounding.h"

BEGIN(Engine)

class CBounding_AABB final : public CBounding
{
public:
	typedef struct tagAABBDesc
	{
		_float3		vSize;
	}AABBDESC;

private:
	CBounding_AABB();
	virtual ~CBounding_AABB() = default;

public:
	virtual void* Get_BoundingDesc() override {
		return m_pAABB;
	};

public:
	virtual HRESULT Initialize(const _float3& vPosition, const _float3& vSize);
	virtual void Tick(_fmatrix TransformMatrix);
	virtual _bool Intersect(CCollider::TYPE eType, void* pBoundingDesc) override;
	virtual _bool Intersect_Mouse(_vector Origin, _vector Direction, _float& Dist) override;

#ifdef _DEBUG
public:
	HRESULT	Render(PrimitiveBatch<VertexPositionColor>* pBatch, _fvector vColor) override;
#endif

private:
	BoundingBox*				m_pAABB_Original = { nullptr };
	BoundingBox*				m_pAABB = { nullptr };
public:
	static CBounding_AABB* Create(const _float3& vPosition, const _float3& vSize);
	virtual void Free() override;
};

END