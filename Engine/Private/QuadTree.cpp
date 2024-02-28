#include "QuadTree.h"
#include "Frustum.h"

CQuadTree::CQuadTree()
{
}

HRESULT CQuadTree::Initialize(_uint iLT, _uint iRT, _uint iRB, _uint iLB)
{
	m_iCorners[CORNER_LT] = iLT;
	m_iCorners[CORNER_RT] = iRT;
	m_iCorners[CORNER_RB] = iRB;
	m_iCorners[CORNER_RT] = iLB;

	if (1 == iRT - iLT)
		return S_OK;

	m_iCenter = (iLT + iRB) >> 1;

	_uint iLC, iTC, iRC, iBC;

	iLC = (iLT + iLB) >> 1;
	iTC = (iLT + iRT) >> 1;
	iRC = (iRT + iRB) >> 1;
	iBC = (iLB + iRB) >> 1;

	m_pChilds[CHILD_LT] = CQuadTree::Create(m_iCorners[CORNER_LT], iTC, m_iCenter, iLC);
	m_pChilds[CHILD_RT] = CQuadTree::Create(iTC, m_iCorners[CORNER_RT], iRC, m_iCenter);
	m_pChilds[CHILD_RB] = CQuadTree::Create(m_iCenter, iRC, m_iCorners[CORNER_RB], iBC);
	m_pChilds[CHILD_LB] = CQuadTree::Create(iLC, m_iCenter, iBC, m_iCorners[CORNER_LB]);

	return S_OK;
}

void CQuadTree::Culling(CFrustum* pFrustum, const _float3* pVerticesPos, _ulong* pIndices, _uint* pNumIndices, _fvector vCamPosition)
{
	/* 더 이상 분열될 수 없다면. */
	/* 카메라랑 충분히 멀어져 있다면. */
	if (nullptr == m_pChilds[CHILD_LT] ||
		true == isDraw(pVerticesPos, vCamPosition))
	{
		_bool		isIn[4] = {
			pFrustum->isInFrustum_InLocal(XMLoadFloat3(&pVerticesPos[m_iCorners[0]])),
			pFrustum->isInFrustum_InLocal(XMLoadFloat3(&pVerticesPos[m_iCorners[1]])),
			pFrustum->isInFrustum_InLocal(XMLoadFloat3(&pVerticesPos[m_iCorners[2]])),
			pFrustum->isInFrustum_InLocal(XMLoadFloat3(&pVerticesPos[m_iCorners[3]])),
		};

		_bool		isDraw[NEIGHBOR_END] = { true, true, true, true };

		for (size_t i = 0; i < NEIGHBOR_END; i++)
		{
			if (nullptr != m_pNeighbors[i])
				isDraw[i] = m_pNeighbors[i]->isDraw(pVerticesPos, vCamPosition);
		}

		if (true == isDraw[NEIGHBOR_LEFT] &&
			true == isDraw[NEIGHBOR_TOP] &&
			true == isDraw[NEIGHBOR_RIGHT] &&
			true == isDraw[NEIGHBOR_BOTTOM])
		{
			if (true == isIn[0] ||
				true == isIn[1] ||
				true == isIn[2])
			{
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_LT];
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_RT];
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_RB];
			}

			if (true == isIn[0] ||
				true == isIn[2] ||
				true == isIn[3])
			{
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_LT];
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_RB];
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_LB];
			}
			return;
		}

		_uint		iLC, iTC, iRC, iBC;

		iLC = (m_iCorners[CORNER_LT] + m_iCorners[CORNER_LB]) >> 1;
		iTC = (m_iCorners[CORNER_LT] + m_iCorners[CORNER_RT]) >> 1;
		iRC = (m_iCorners[CORNER_RT] + m_iCorners[CORNER_RB]) >> 1;
		iBC = (m_iCorners[CORNER_LB] + m_iCorners[CORNER_RB]) >> 1;

		if (true == isIn[0] ||
			true == isIn[2] ||
			true == isIn[3])
		{
			if (false == isDraw[NEIGHBOR_LEFT])
			{
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_LT];
				pIndices[(*pNumIndices)++] = m_iCenter;
				pIndices[(*pNumIndices)++] = iLC;

				pIndices[(*pNumIndices)++] = iLC;
				pIndices[(*pNumIndices)++] = m_iCenter;
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_LB];
			}
			else
			{
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_LT];
				pIndices[(*pNumIndices)++] = m_iCenter;
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_LB];
			}

			if (false == isDraw[NEIGHBOR_BOTTOM])
			{
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_LB];
				pIndices[(*pNumIndices)++] = m_iCenter;
				pIndices[(*pNumIndices)++] = iBC;

				pIndices[(*pNumIndices)++] = iBC;
				pIndices[(*pNumIndices)++] = m_iCenter;
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_RB];
			}
			else
			{
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_LB];
				pIndices[(*pNumIndices)++] = m_iCenter;
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_RB];
			}
		}
		if (true == isIn[0] ||
			true == isIn[1] ||
			true == isIn[2])
		{
			if (false == isDraw[NEIGHBOR_RIGHT])
			{
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_RT];
				pIndices[(*pNumIndices)++] = iRC;
				pIndices[(*pNumIndices)++] = m_iCenter;

				pIndices[(*pNumIndices)++] = m_iCenter;
				pIndices[(*pNumIndices)++] = iRC;
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_RB];
			}
			else
			{
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_RT];
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_RB];
				pIndices[(*pNumIndices)++] = m_iCenter;
			}

			if (false == isDraw[NEIGHBOR_TOP])
			{
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_LT];
				pIndices[(*pNumIndices)++] = iTC;
				pIndices[(*pNumIndices)++] = m_iCenter;

				pIndices[(*pNumIndices)++] = m_iCenter;
				pIndices[(*pNumIndices)++] = iTC;
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_RT];
			}
			else
			{
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_LT];
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_RT];
				pIndices[(*pNumIndices)++] = m_iCenter;
			}
		}

		return;
	}
	/* 자식으로 더 분열되야하냐? */
	_float		fRadius = XMVectorGetX(XMVector3Length(XMLoadFloat3(&pVerticesPos[m_iCorners[CORNER_LT]]) - XMLoadFloat3(&pVerticesPos[m_iCenter])));

	if (true == pFrustum->isInFrustum_InLocal(XMLoadFloat3(&pVerticesPos[m_iCenter]), fRadius))
	{
		for (size_t i = 0; i < CHILD_END; i++)
		{
			if (nullptr != m_pChilds[i])
				m_pChilds[i]->Culling(pFrustum, pVerticesPos, pIndices, pNumIndices, vCamPosition);
		}
	}

}

void CQuadTree::SetUp_Neighbors()
{
	if (nullptr == m_pChilds[CHILD_LT]->m_pChilds[CHILD_LT])
		return;

	m_pChilds[CHILD_LT]->m_pNeighbors[NEIGHBOR_RIGHT] = m_pChilds[CHILD_RT];
	m_pChilds[CHILD_LT]->m_pNeighbors[NEIGHBOR_BOTTOM] = m_pChilds[CHILD_LB];

	m_pChilds[CHILD_RT]->m_pNeighbors[NEIGHBOR_LEFT] = m_pChilds[CHILD_LT];
	m_pChilds[CHILD_RT]->m_pNeighbors[NEIGHBOR_BOTTOM] = m_pChilds[CHILD_RB];

	m_pChilds[CHILD_RB]->m_pNeighbors[NEIGHBOR_LEFT] = m_pChilds[CHILD_LB];
	m_pChilds[CHILD_RB]->m_pNeighbors[NEIGHBOR_TOP] = m_pChilds[CHILD_RT];

	m_pChilds[CHILD_LB]->m_pNeighbors[NEIGHBOR_RIGHT] = m_pChilds[CHILD_RB];
	m_pChilds[CHILD_LB]->m_pNeighbors[NEIGHBOR_TOP] = m_pChilds[CHILD_LT];

	if (nullptr != m_pNeighbors[NEIGHBOR_LEFT])
	{
		m_pChilds[CHILD_LT]->m_pNeighbors[NEIGHBOR_LEFT] = m_pNeighbors[NEIGHBOR_LEFT]->m_pChilds[CHILD_RT];
		m_pChilds[CHILD_LB]->m_pNeighbors[NEIGHBOR_LEFT] = m_pNeighbors[NEIGHBOR_LEFT]->m_pChilds[CHILD_RB];
	}

	if (nullptr != m_pNeighbors[NEIGHBOR_TOP])
	{
		m_pChilds[CHILD_LT]->m_pNeighbors[NEIGHBOR_TOP] = m_pNeighbors[NEIGHBOR_TOP]->m_pChilds[CHILD_LB];
		m_pChilds[CHILD_RT]->m_pNeighbors[NEIGHBOR_TOP] = m_pNeighbors[NEIGHBOR_TOP]->m_pChilds[CHILD_RB];
	}

	if (nullptr != m_pNeighbors[NEIGHBOR_RIGHT])
	{
		m_pChilds[CHILD_RT]->m_pNeighbors[NEIGHBOR_RIGHT] = m_pNeighbors[NEIGHBOR_RIGHT]->m_pChilds[CHILD_LT];
		m_pChilds[CHILD_RB]->m_pNeighbors[NEIGHBOR_RIGHT] = m_pNeighbors[NEIGHBOR_RIGHT]->m_pChilds[CHILD_LB];
	}

	if (nullptr != m_pNeighbors[NEIGHBOR_BOTTOM])
	{
		m_pChilds[CHILD_LB]->m_pNeighbors[NEIGHBOR_BOTTOM] = m_pNeighbors[NEIGHBOR_BOTTOM]->m_pChilds[CHILD_LT];
		m_pChilds[CHILD_RB]->m_pNeighbors[NEIGHBOR_BOTTOM] = m_pNeighbors[NEIGHBOR_BOTTOM]->m_pChilds[CHILD_RT];
	}

	for (auto& pChild : m_pChilds)
	{
		if (nullptr != pChild)
			pChild->SetUp_Neighbors();
	}
}

_bool CQuadTree::isDraw(const _float3* pVerticesPos, _fvector vCamPosition)
{
	_float		fDistance = XMVectorGetX(XMVector3Length(vCamPosition - XMLoadFloat3(&pVerticesPos[m_iCenter])));

	_float		fWidth = XMVectorGetX(XMVector3Length(XMLoadFloat3(&pVerticesPos[m_iCorners[CORNER_RT]]) - XMLoadFloat3(&pVerticesPos[m_iCorners[CORNER_LT]])));

	if (fDistance * 0.3f > fWidth)
		return true;

	return false;
}

CQuadTree* CQuadTree::Create(_uint iLT, _uint iRT, _uint iRB, _uint iLB)
{
	CQuadTree* pInstance = new CQuadTree();

	if (FAILED(pInstance->Initialize(iLT, iRT, iRB, iLB)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Created : CQuadTree");
	}

	return pInstance;
}

void CQuadTree::Free()
{
	for (size_t i = 0; i < CHILD_END; i++)
		Safe_Release(m_pChilds[i]);
}
