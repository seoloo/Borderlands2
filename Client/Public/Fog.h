#pragma once

#include "Client_Defines.h"
#include "BlendObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CFog final : public CBlendObject
{
private:
	CFog(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFog(const CFog& rhs);
	virtual ~CFog() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	CShader*						m_pShaderCom = { nullptr };
	CTexture*						m_pTextureCom = { nullptr };
	CRenderer*						m_pRendererCom = { nullptr };
	CTransform*						m_pTransformCom = { nullptr };
	CVIBuffer_Rect*					m_pVIBufferCom = { nullptr };

private:
	_float							m_fFrame = { 0.0f };

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();


public:
	static CFog* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg)	override;
	virtual void Free() override;
};

END