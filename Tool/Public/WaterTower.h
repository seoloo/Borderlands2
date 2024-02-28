#pragma once
#include "Tool_Object.h"
#include "Tool_Defines.h"

BEGIN(Engine)
class CModel;
class CShader;
class CRenderer;
class CTransform;
class CVIBuffer;
END

BEGIN(Tool)
class CWaterTower final: public CTool_Object
{
private:
	CWaterTower(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWaterTower(const CWaterTower& rhs);
	virtual ~CWaterTower() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void	Apply_Tool();
	void	Key_Input();

private:
	CModel* m_pModelCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CTransform* m_pTransformCom = { nullptr };
	CVIBuffer* m_pVIBufferCom = { nullptr };

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
	HRESULT		Add_Components();
	HRESULT		Bind_ShaderResources();

public:
	static CWaterTower* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CWaterTower* Clone(void* pArg);
	virtual void Free() override;
};

END