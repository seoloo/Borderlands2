#pragma once

#include "Client_Defines.h"
#include "Camera.h"

BEGIN(Engine)
class CTransform;
END

BEGIN(Client)
class CCamera_OnBone final : public CCamera
{
public:
	typedef struct tagCameraFreeDesc
	{
		const _float4x4* pBoneMatrix = nullptr;
		_float4x4			PivotMatrix;
		const _float4x4* pParentMatrix = nullptr;

		CCamera::CAMERADESC			CameraDesc;
	}CAMERAONBONEDESC;
private:
	CCamera_OnBone(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera_OnBone(const CCamera_OnBone& rhs);
	virtual ~CCamera_OnBone() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;

public:
	void	Moving_Camera(_float fTimeDelta);
	void	Cut_Scene(_float fTimeDelta);
	
private:
	CTransform* m_pSpiderAntTransformCom = { nullptr };
	CTransform* m_pSkagTransformCom = { nullptr };
	CTransform* m_pObjectTransformCom = { nullptr };

private:
	_float4x4						m_WorldMatrix;
	const _float4x4* m_pBoneMatrix = { nullptr };
	_float4x4						m_PivotMatrix;

	const _float4x4* m_pParentMatrix = { nullptr };

private:
	_vector m_ObjectPos;
	_vector m_ObjectUp;
	_vector m_ObjectRight;
	_vector m_ObjectLook;

	_vector m_PlayerPos;
	_vector m_PlayerUp;
	_vector m_PlayerRight;
	_vector m_PlayerLook;

	_vector m_vCameraPos;

	mt19937_64					m_RandomNumber;
	uniform_real_distribution<float>		m_RandomFovy;
	uniform_real_distribution<float>		m_RandomFovyForShake;
	_bool	m_bPistolCam = { true };

	_int	m_iBulletCount = { 0 };

public:
	static CCamera_OnBone* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END