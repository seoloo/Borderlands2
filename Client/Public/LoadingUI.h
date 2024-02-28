#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CLoadingUI final : public CGameObject
{
private:
	CLoadingUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLoadingUI(const CLoadingUI& rhs);
	virtual ~CLoadingUI() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	CShader*		m_pShaderCom = { nullptr };
	CTexture*		m_pTextureCom = { nullptr };
	CTexture*		m_pTextureGamePlayCom = { nullptr };
	CTexture*		m_pTextureTownCom = { nullptr };
	CTexture*		m_pTextureEndingCom = { nullptr };
	CRenderer*		m_pRendererCom = { nullptr };
	CTransform*		m_pTransformCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

private:
	_float							m_fX, m_fY, m_fSizeX, m_fSizeY;
	_float4x4						m_WorldMatrix, m_ViewMatrix, m_ProjMatrix;

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CLoadingUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg)	override;
	virtual void Free() override;
};

END