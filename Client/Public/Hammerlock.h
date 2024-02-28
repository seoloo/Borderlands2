#pragma once

#include "Client_Defines.h"
#include "LandObject.h"

BEGIN(Engine)
class CModel;
class CShader;
class CCollider;
class CRenderer;
class CTransform;
class CNavigation;
class CTexture;
END

BEGIN(Client)

class CHammerlock final : public CLandObject
{
public:
	enum COLLIDER { COLLIDER_AABB, COLLIDER_OBB, COLLIDER_SPHERE, COLLIDER_END };


private:
	CHammerlock(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CHammerlock(const CHammerlock& rhs);
	virtual ~CHammerlock() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual HRESULT Render_ShadowDepth();

private:
	CModel*							m_pModelCom = { nullptr };
	CShader*						m_pShaderCom = { nullptr };	
	CRenderer*						m_pRendererCom = { nullptr };
	CTransform*						m_pTransformCom = { nullptr };
	CTransform*						m_pPlayerTransformCom = { nullptr };
	CNavigation*					m_pNavigationCom = { nullptr };
	CCollider*						m_pColliderCom[COLLIDER_END] = { nullptr };
	CTexture* m_pTextureCom = { nullptr };

	_bool m_bCheck = { false };
	_float m_fValue = { 0 };
private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();
	HRESULT Collision_Object();

public:
	static CHammerlock* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg)	override;
	virtual void Free() override;
};

END