#pragma once

#include "Bounding.h"

BEGIN(Engine)

class CBounding_OBB final : public CBounding
{
private:
	CBounding_OBB();
	virtual ~CBounding_OBB() = default;

public:
	virtual void* Get_BoundingDesc()  override {
		return m_pOBB;
	}

public:
	virtual HRESULT Initialize(const _float3& vPosition, const _float3& vSize, const _float3& vRotation);
	virtual void Tick(_fmatrix TransformMatrix);
	virtual _bool Intersect(CCollider::TYPE eType, void* pBoundingDesc) override;
	virtual _bool Intersect_Mouse(_vector Origin, _vector Direction, _float& Dist) override;

#ifdef _DEBUG
public:
	HRESULT	Render(PrimitiveBatch<VertexPositionColor>* pBatch, _fvector vColor) override;

#endif
private:
	BoundingOrientedBox* m_pOBB_Original = { nullptr };
	BoundingOrientedBox*				m_pOBB = { nullptr };
public:
	static CBounding_OBB* Create(const _float3& vPosition, const _float3& vSize, const _float3& vRotation);
	virtual void Free() override;
};

END