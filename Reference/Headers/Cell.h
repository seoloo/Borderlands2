#pragma once

#include "Base.h"

/* 네비게이션을 구성하는 삼각형 하나. */

BEGIN(Engine)
class CNavigation;

class CCell final : public CBase
{
public:
	enum POINT { POINT_A, POINT_B, POINT_C, POINT_END };
	enum LINE { LINE_AB, LINE_BC, LINE_CA, LINE_END };

private:
	CCell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CCell() = default;

public:
	const _float3& Get_Point(POINT ePoint) const {
		return m_vPoints[ePoint];
	}

public:
	void SetUp_Neighbor(LINE eLine, const CCell* pNeighborCell) {
		m_iNeighborIndices[eLine] = pNeighborCell->m_iIndex;
	}

public:
	HRESULT Initialize(const _float3* pPoints, _int iIndex);
	_bool Compare_Points(const _float3& vSourPoint, const _float3& vDestPoint);
	_bool is_In(_fvector vPosition, _int* pNeighborIndex);

#ifdef _DEBUG
public:
	HRESULT Render();

#endif
public:
	_float3 Get_FirstPos() { return m_vFirstPos; };
	_float3 Get_SecondPos() { return m_vSecondPos; };

private:
	class CNavigation* m_pNavigation = { nullptr };


private:
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };

private:
	_float3				m_vPoints[POINT_END];	
	_int				m_iNeighborIndices[LINE_END] = { -1, -1, -1 };
	_int				m_iIndex = { 0 };

	_float3				m_vFirstPos = { 0.f,0.f,0.f };
	_float3				m_vSecondPos = { 0.f,0.f,0.f };

#ifdef _DEBUG
private:
	class CVIBuffer_Cell* m_pVIBuffer = { nullptr };
#endif

public:
	static CCell* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* pPoints, _int iIndex);
	virtual void Free() override;
};

END