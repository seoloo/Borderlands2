#pragma once

/* AABB, OBB, SPHERE의 부모가 되는 클래스.*/
#include "Base.h"
#include "Collider.h"

BEGIN(Engine)

class CBounding abstract : public CBase
{
protected:
	CBounding();
	virtual ~CBounding() = default;
public:
	virtual void* Get_BoundingDesc() = 0;

public:
	virtual HRESULT Initialize();
	virtual void Tick(_fmatrix TransformMatrix) = 0;

	virtual _bool Intersect(CCollider::TYPE eType, void* pBoundingDesc) = 0;
	virtual _bool Intersect_Mouse(_vector Origin, _vector Direction, _float& Dist) = 0;

#ifdef _DEBUG
public:
	virtual HRESULT Render(PrimitiveBatch<VertexPositionColor>* pBatch, _fvector vColor) = 0;
#endif

public:
	virtual void Free() override;
};

END