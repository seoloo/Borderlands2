#pragma once
#include "GameObject.h"
#include "Transform.h"

/* Ŭ���̾�Ʈ���� �����ϰ� ���ϴ� ī�޶���� �θ� �ȴ�. */
/* ��� ī�޶󿡰� �������� ����� �ʿ��ϱ⶧���� .*/

BEGIN(Engine)
class ENGINE_DLL CCamera abstract : public CGameObject
{
public:
	typedef struct tagCameraDesc
	{
		_uint		iLevelIndex;
		wstring		strTranformTag;
		_float4		vEye, vAt;
		_float		fFovy, fAspect, fNear, fFar;
		CTransform::TRANSFORMDESC	TransformDesc;
	}CAMERADESC;

protected:
	CCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera(const CCamera& rhs);
	virtual ~CCamera() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;

protected:
	_float		m_fFovy, m_fAspect, m_fNear, m_fFar;
	class CTransform* m_pTransform = { nullptr };
	class CPipeLine* m_pPipeLine = { nullptr };
	CAMERADESC m_CameraDesc;

protected:
	HRESULT Set_Transform();

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END