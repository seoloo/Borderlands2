#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CModel;
class CShader;
class CCollider;
class CRenderer;
class CTransform;
class CTexture;

END

BEGIN(Client)

class CPsycho_Face final : public CGameObject
{
public:
	typedef struct tagWeaponDesc
	{
		const _float4x4* pBoneMatrix = nullptr;
		_float4x4				PivotMatrix;
		const _float4x4* pParentMatrix = nullptr;
		_int iNumber = { 0 };
	}WEAPONDESC;

private:
	CPsycho_Face(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPsycho_Face(const CPsycho_Face& rhs);
	virtual ~CPsycho_Face() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CModel* m_pModelCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CTransform* m_pTransformCom = { nullptr };
	CCollider* m_pColliderCom[COLLIDER_END] = { nullptr };
	CTexture* m_pTextureCom = { nullptr };

private:
	_float4x4						m_WorldMatrix;
	const _float4x4* m_pBoneMatrix = { nullptr };
	_float4x4						m_PivotMatrix;

	const _float4x4* m_pParentMatrix = { nullptr };

	_bool m_bDemage = { false };
	_int	m_iNumber = { 0 };

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CPsycho_Face* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg)	override;
	virtual void Free() override;
};

END