#pragma once

#include "Client_Defines.h"
#include "Camera.h"

BEGIN(Client)
class CCamera_Free final : public CCamera
{
public:
	typedef struct tagCameraFreeDesc
	{
		CCamera::CAMERADESC			CameraDesc;
	}CAMERAFREEDESC;
private:
	CCamera_Free(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera_Free(const CCamera_Free& rhs);
	virtual ~CCamera_Free() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;

private:
	CTransform* m_pPlayerTransformCom = { nullptr };

private:
	_vector m_PlayerPos;
	CAMERAFREEDESC* m_CameraFreeDesc = { nullptr };

public:
	static CCamera_Free* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END