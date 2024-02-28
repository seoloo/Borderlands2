#pragma once

#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CBlendObject abstract : public CGameObject
{
protected:
	CBlendObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBlendObject(const CBlendObject& rhs);
	virtual ~CBlendObject() = default;

public:
	_float Get_CamDistance() const {
		return m_fCamDistance;
	}

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

protected:
	_float			m_fCamDistance = { 0.0f };

protected:
	void Compute_CamDistance(class CTransform* pTransform);

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END