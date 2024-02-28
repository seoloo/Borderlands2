#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CModel;
class CShader;
class CCollider;
class CRenderer;
class CTransform;
END

BEGIN(Client)

class CMissile :
    public CGameObject
{
public:
	typedef struct BulletDesc {
		_vector CreatePos = { XMVectorSet(0.f,0.f,0.f,1.f) };
		_vector LookDir = { XMVectorSet(0.f,0.f,0.f,1.f) };
		_vector UpDir = { XMVectorSet(0.f,0.f,0.f,1.f) };
	}BULLETDESC;

private:
	CMissile(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMissile(const CMissile& rhs);
	virtual ~CMissile() = default;

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
	CTransform* m_pPlayerTransformCom = { nullptr };
	CCollider* m_pColliderCom[COLLIDER_END] = { nullptr };

private:
	_vector m_PlayrLook = { XMVectorSet(0.f,0.f,0.f,1.f) };

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();
	HRESULT Collision_Object();

public:
	static CMissile* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg)	override;
	virtual void Free() override;
};

END