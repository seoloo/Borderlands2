#pragma once

#include "EffectTool_Defines.h"
#include "Camera.h"

BEGIN(EffectTool)
class CEffectTool_Camera final : public CCamera
{
public:
	typedef struct tagCameraFreeDesc
	{
		/* 니네들이 하고싶은거. */

		CCamera::CAMERADESC			CameraDesc;
	}CAMERAFREEDESC;
private:
	CEffectTool_Camera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffectTool_Camera(const CEffectTool_Camera& rhs);
	virtual ~CEffectTool_Camera() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;

private:
	/* 니네들이 하고싶은거. */


public:
	static CEffectTool_Camera* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END