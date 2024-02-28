#pragma once

#include "Base.h"

/* ���, ��������� ������ �ִ´�. */
/* ��û�ϸ� ������ ��, ������ �����Ѵ�. */
/* ����ĵ� ���س��� ������ �����Ѵ�. */
/* ī�޶� ��ġ���� ���س��� ������ ����. */

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