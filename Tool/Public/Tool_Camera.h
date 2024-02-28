#pragma once

#include "Tool_Defines.h"
#include "Camera.h"

BEGIN(Tool)
class CTool_Camera final : public CCamera
{
public:
	typedef struct tagCameraFreeDesc
	{
		/* �ϳ׵��� �ϰ������. */

		CCamera::CAMERADESC			CameraDesc;
	}CAMERAFREEDESC;
private:
	CTool_Camera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTool_Camera(const CTool_Camera& rhs);
	virtual ~CTool_Camera() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;

private:
	/* �ϳ׵��� �ϰ������. */


public:
	static CTool_Camera* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END