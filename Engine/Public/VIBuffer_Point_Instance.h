#pragma once

#include "VIBuffer_Instance.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Point_Instance final : public CVIBuffer_Instance
{
public:
	typedef struct tagPointInstanceDesc
	{
		_int		iNumInstance = { 0 };
		_float4		vPivotPos = { 0.f,0.f,0.f,0.f };
		_float3		vRange = { 0.f,0.f,0.f };
		_float		fLifeTime = { 0.f };
		_float		fMinSpeed, fMaxSpeed = (0.f, 0.f);
		_bool		bFalling = { false };
		_bool		bRising = { false };
		_bool		bExplosion = { false };
		_bool		bBleeding = { false };
		_bool		bDig = { false };
		_bool		bCatridge = { false };
		_bool		bGreen = { false };
		_bool		bFog = { false };
		_bool		bGray = { false };
		_bool		bSmoke = { false };
		_bool		bBlue = { false };
		_bool		bRed = { false };
		_bool		bBlack = { false };
		_bool		bViolet = { false };
		/* 데미지 폰트 */
		_int		iNum = { 0 };
		_int		iDamage = { 0 };
	}POINTINSTANCEDESC;

private:
	CVIBuffer_Point_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Point_Instance(const CVIBuffer_Point_Instance& rhs);
	virtual ~CVIBuffer_Point_Instance() = default;

public:
	virtual HRESULT Initialize_Prototype(_uint iNumInstance);
	virtual HRESULT Initialize(void* pArg) override;

public:
	void Tick(_float fTimeDelta);

private:
	POINTINSTANCEDESC			m_InstanceData;
	_float*						m_pInstanceSpeed = { nullptr };

	mt19937_64					m_RandomNumber;
	uniform_real_distribution<float>		m_RandomX;
	uniform_real_distribution<float>		m_RandomY;
	uniform_real_distribution<float>		m_RandomZ;
	uniform_real_distribution<float>		m_RandomLookX;
	uniform_real_distribution<float>		m_RandomLookY;
	uniform_real_distribution<float>		m_RandomLookZ;

public:
	static CVIBuffer_Point_Instance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iNumInstance);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END