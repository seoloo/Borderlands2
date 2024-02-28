#pragma once

#include "Client_Defines.h"
#include "Camera.h"

BEGIN(Engine)
class CTransform;
END

BEGIN(Client)
class CCamera_Pistol final : public CCamera
{
public:
	typedef struct tagCameraFreeDesc
	{
		const _float4x4* pBoneMatrix = nullptr;
		_float4x4			PivotMatrix;
		const _float4x4* pParentMatrix = nullptr;

		CCamera::CAMERADESC			CameraDesc;
	}CAMERAPISTOLDESC;
private:
	CCamera_Pistol(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera_Pistol(const CCamera_Pistol& rhs);
	virtual ~CCamera_Pistol() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;

private:
	CTransform* m_pTransformCom = { nullptr };
	CTransform* m_pPlayerTransformCom = { nullptr };

private:
	_vector m_PlayerPos;
	_vector m_PlayerUp;
	_vector m_PlayerRight;
	_vector m_PlayerLook;
	_float4x4						m_WorldMatrix;
	const _float4x4* m_pBoneMatrix = { nullptr };
	_float4x4						m_PivotMatrix;

	const _float4x4* m_pParentMatrix = { nullptr };

private:
	HRESULT Add_Components();

public:
	static CCamera_Pistol* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END