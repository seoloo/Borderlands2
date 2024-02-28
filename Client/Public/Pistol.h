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

class CPistol final : public CGameObject
{
public:
	typedef struct tagWeaponDesc
	{
		const _float4x4* pBoneMatrix = nullptr;
		_float4x4				PivotMatrix;
		const _float4x4* pParentMatrix = nullptr;
	}WEAPONDESC;

private:
	CPistol(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPistol(const CPistol& rhs);
	virtual ~CPistol() = default;

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

	map<const wstring, CGameObject*>	m_Parts;

private:
	_float4x4						m_WorldMatrix;
	const _float4x4* m_pBoneMatrix = { nullptr };
	_float4x4						m_PivotMatrix;

	const _float4x4* m_pParentMatrix = { nullptr };
	_float4 m_vPlayerPos = { _float4(0.f,0.f,0.f,0.f) };
	_bool	m_bFire = { false };

private:
	HRESULT Add_Components();
	HRESULT Add_Parts();
	HRESULT Bind_ShaderResources();

public:
	static CPistol* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg)	override;
	virtual void Free() override;
};

END