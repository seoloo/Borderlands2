#pragma once

#include "Bounding.h"

BEGIN(Engine)

class CBounding_Sphere final : public CBounding
{
private:
	CBounding_Sphere();
	virtual ~CBounding_Sphere() = default;

public:
	virtual void* Get_BoundingDesc() override {
		return m_pSphere;
	}

public:
	virtual HRESULT Initialize(const _float3& vPosition, _float fRadius);
	virtual void Tick(_fmatrix TransformMatrix);
	virtual _bool Intersect(CCollider::TYPE eType, void* pBoundingDesc) override;
	virtual _bool Intersect_Mouse(_vector Origin, _vector Direction, _float& Dist) override;
#ifdef _DEBUG
public:
	HRESULT	Render(PrimitiveBatch<VertexPositionColor>* pBatch, _fvector vColor) override;

#endif

private:
	BoundingSphere* m_pSphere_Original = { nullptr };
	BoundingSphere*				m_pSphere = { nullptr };

public:
	static CBounding_Sphere* Create(const _float3& vPosition, _float fRadius);
	virtual void Free() override;
};

END