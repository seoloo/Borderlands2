#pragma once

#include "Base.h"

/* 뷰랑, 투영행렬을 가지고 있는다. */
/* 요청하면 언제든 뷰, 투영을 리턴한다. */
/* 역행렬도 구해놔서 언제든 리턴한다. */
/* 카메라 위치까지 구해놔서 언제든 리턴. */

BEGIN(Engine)

class CPipeLine final : public CBase
{
	DECLARE_SINGLETON(CPipeLine)

public:
	enum TRANSFORMSTATE { D3DTS_VIEW, D3DTS_PROJ, D3DTS_END };

private:
	CPipeLine();
	virtual ~CPipeLine() = default;

public:
	void Set_Transform(TRANSFORMSTATE eState, _fmatrix TransformMatrix) {
		XMStoreFloat4x4(&m_TransformMatrix[eState], TransformMatrix);
	}

public:
	_matrix Get_Transform_Matrix(TRANSFORMSTATE eState) {
		return XMLoadFloat4x4(&m_TransformMatrix[eState]);
	}
	_float4x4 Get_Transform_Float4x4(TRANSFORMSTATE eState) {
		return m_TransformMatrix[eState];
	}

	_matrix Get_Transform_Matrix_Inverse(TRANSFORMSTATE eState) {
		return XMLoadFloat4x4(&m_TransformMatrixInv[eState]);
	}
	_float4x4 Get_Transform_Float4x4_Inverse(TRANSFORMSTATE eState) {
		return m_TransformMatrixInv[eState];
	}

	_float4	Get_CamPosition() {
		return m_vCamPosition;
	}

public:
	HRESULT Initialize();
	void Tick();

private:
	_float4x4			m_TransformMatrix[D3DTS_END];
	_float4x4			m_TransformMatrixInv[D3DTS_END];
	_float4				m_vCamPosition;

public:
	virtual void Free() override;
};

END