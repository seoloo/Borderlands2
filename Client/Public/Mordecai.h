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
END

BEGIN(Client)

class CMordecai final : public CLandObject
{
public:
	enum COLLIDER { COLLIDER_AABB, COLLIDER_OBB, COLLIDER_SPHERE, COLLIDER_END };


private:
	CMordecai(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMordecai(const CMordecai& rhs);
	virtual ~CMordecai() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	CModel*							m_pModelCom = { nullptr };
	CShader*						m_pShaderCom = { nullptr };	
	CRenderer*						m_pRendererCom = { nullptr };
	CTransform*						m_pTransformCom = { nullptr };
	CTransform* m_pPlayerTransformCom = { nullptr };
	CNavigation*					m_pNavigationCom = { nullptr };
	CCollider*						m_pColliderCom[COLLIDER_END] = { nullptr };

private:
	HRESULT Add_Components();

	HRESULT Bind_ShaderResources();


public:
	static CMordecai* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg)	override;
	virtual void Free() override;
};

END