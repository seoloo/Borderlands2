#pragma once

#include "Base.h"

BEGIN(Engine)

class CFrustum final : public CBase
{
	DECLARE_SINGLETON(CFrustum)
private:
	CFrustum();
	virtual ~CFrustum() = default;

public:
	HRESULT Initialize();
	void Tick();

	void Transform_ToLocalSpace(_fmatrix WorldMatrix);
	_bool isInFrustum_InWorld(_fvector vWorldPos, _float fRange = 0.f);
	_bool isInFrustum_InLocal(_fvector vLocalPos, _float fRange = 0.f);

private:
	_float3				m_vPoints[8];	
	_float3				m_vPoints_InWorld[8];
	_float4				m_vPlanes_InWorld[6];
	_float4				m_vPlanes_InLocal[6];

private:
	HRESULT Make_Plane(const _float3* pPoints, _Inout_ _float4* pPlanes);

public:
	virtual void Free() override;
};

END