#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CCollider;
class CRenderer;
class CTransform;
class CGameObject;
END

BEGIN(Client)

class CSpiderAnt_LegCollider final : public CGameObject
{
public:
	typedef struct tagColliderDesc
	{
		const _float4x4* pBoneMatrix = nullptr;
		_float4x4				PivotMatrix;
		const _float4x4* pParentMatrix = nullptr;
	}COLLIDERDESC;

private:
	CSpiderAnt_LegCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSpiderAnt_LegCollider(const CSpiderAnt_LegCollider& rhs);
	virtual ~CSpiderAnt_LegCollider() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader* m_pShaderCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CTransform* m_pTransformCom = { nullptr };
	CCollider* m_pColliderCom[COLLIDER_END] = { nullptr };
	CGameObject* m_pPlayer = { nullptr };
private:
	_float4x4						m_WorldMatrix;
	const _float4x4* m_pBoneMatrix = { nullptr };
	_float4x4						m_PivotMatrix;
	const _float4x4* m_pParentMatrix = { nullptr };

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();
	HRESULT Collision_Object();

public:
	static CSpiderAnt_LegCollider* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg)	override;
	virtual void Free() override;
};

END