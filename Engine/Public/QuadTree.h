#pragma once

#include "Base.h"

/* 첫번째 객체화는 전체 지형에 대한 정보를 담는다. */
/* 자식을 네개씩 가질거야. */

BEGIN(Engine)

class CQuadTree final : public CBase
{
public:
	enum CHILD { CHILD_LT, CHILD_RT, CHILD_RB, CHILD_LB, CHILD_END };
	enum CORNER { CORNER_LT, CORNER_RT, CORNER_RB, CORNER_LB, CORNER_END };
	enum NEIGHBOR { NEIGHBOR_LEFT, NEIGHBOR_TOP, NEIGHBOR_RIGHT, NEIGHBOR_BOTTOM, NEIGHBOR_END };
private:
	CQuadTree();
	virtual ~CQuadTree() = default;

public:
	HRESULT Initialize(_uint iLT, _uint iRT, _uint iRB, _uint iLB);
	void Culling(class CFrustum* pFrustum, const _float3* pVerticesPos, _ulong* pIndices, _uint* pNumIndices, _fvector vCamPosition);
	void SetUp_Neighbors();

private:
	/* 내 쿼드트리의 중점에 존재하는 정점의 인덱스*/
	_uint							m_iCenter = { 0 };
	_uint							m_iCorners[CORNER_END] = { 0 };
	_float							m_fRadius = { 0.0f };

	class CQuadTree* m_pChilds[CHILD_END] = { nullptr };
	class CQuadTree* m_pNeighbors[NEIGHBOR_END] = { nullptr };

private:
	_bool isDraw(const _float3* pVerticesPos, _fvector vCamPosition);
public:
	static CQuadTree* Create(_uint iLT, _uint iRT, _uint iRB, _uint iLB);
	virtual void Free() override;
};

END