#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
class CTexture;
class CCollider;
class CRenderer;
class CTransform;
class CVIBuffer_Point_Instance;
END

BEGIN(Client)

class CMonster_Fire :
    public CGameObject
{
public:
	typedef struct BulletDesc {
		_vector CreatePos = { XMVectorSet(0.f,0.f,0.f,1.f) };
		_vector LookDir = { XMVectorSet(0.f,0.f,0.f,1.f) };
		_vector UpDir = { XMVectorSet(0.f,0.f,0.f,1.f) };
	}BULLETDESC;

private:
	CMonster_Fire(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonster_Fire(const CMonster_Fire& rhs);
	virtual ~CMonster_Fire() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	CModel* m_pModelCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CTransform* m_pTransformCom = { nullptr };
	CVIBuffer_Point_Instance* m_pVIBufferCom = { nullptr };
	CCollider* m_pColliderCom[COLLIDER_END] = { nullptr };

private:
	_vector m_Dir = { XMVectorSet(0.f,0.f,0.f,1.f) };
	_float	m_fFrame = { 0.0f };
	_uint	m_iCount = { 0 };
private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();
	HRESULT Collision_Object();

public:
	static CMonster_Fire* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg)	override;
	virtual void Free() override;
};

END