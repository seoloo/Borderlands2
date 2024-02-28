#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CModel;
class CShader;
class CRenderer;
class CTransform;
END

BEGIN(Client)
class CCrateSet final : public CGameObject
{
public:
	typedef struct tagObjectInfo {
		wstring strName;
		_float3	ObjectSize = { 1.f, 1.f, 1.f };
		_float	fRadian = { 0.f };
		_vector	CreatePos = { XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f) };
	}OBJECTINFO;
private:
	CCrateSet(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCrateSet(const CCrateSet& rhs);
	virtual ~CCrateSet() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	CModel* m_pModelCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CTransform* m_pTransformCom = { nullptr };

private:
	_vector		m_vRight = { 0.f, 0.f, 0.f };
	_vector		m_vUp = { 0.f, 0.f, 0.f };
	_vector		m_vPos = { 0.f, 0.f, 0.f };
	_vector		m_vLook = { 0.f, 0.f, 0.f };
	wstring		m_strName;

	_float3		m_fPos = { 0.f, 0.f, 0.f };
	_float3		m_fSize = { 0.f, 0.f, 0.f };
	_float		m_fRadian = { 0.f };

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CCrateSet* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg)	override;
	virtual void Free() override;
};

END
