#pragma once
#include "Base.h"

BEGIN(Engine)

class CPicking final: public CBase
{
	DECLARE_SINGLETON(CPicking)

private:
	CPicking();
	virtual ~CPicking() = default;

public:
	HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, HWND hWnd);

public: /* 뷰포트상에 마우스위치를 구하고. 투영까지 변환하고, 뷰까지 변환한다, 월드까지 가자. */
	void Tick();
	void Transform_ToLocalSpace(const _float4x4& WorldMatrix);

public:
	_bool Intersect_Triangle_InLocal(const _vector pPointA, const _vector pPointB, const _vector pPointC, _float3* pOut);
	_bool Intersect_Collider(class CBounding* Collider);
private:
	ID3D11Device*			m_pDeivce;
	ID3D11DeviceContext*	m_pContext;
	HWND				m_hWnd;

private:
	_float3				m_vMouseRay, m_vMouseRayPos;
	_float3				m_vMouseRay_Local, m_vMouseRayPos_Local;

public:
	virtual void Free() override;
};

END