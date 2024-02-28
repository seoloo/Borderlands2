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

class CCutScene final : public CGameObject
{
private:
	CCutScene(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCutScene(const CCutScene& rhs);
	virtual ~CCutScene() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader*		m_pShaderCom = { nullptr };	
	CRenderer*		m_pRendererCom = { nullptr };
	CTransform*		m_pTransformCom = { nullptr };
	CTexture*		m_pTextureCom = { nullptr };
	CTexture*		m_pThresherTextureCom = { nullptr };
	CTexture*		m_pSpiderAntTextureCom = { nullptr };
	CTexture*		m_pSkagTextureCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

private:
	_float							m_fX, m_fY, m_fSizeX, m_fSizeY;
	_float4x4						m_WorldMatrix, m_ViewMatrix, m_ProjMatrix;

public:
	static CCutScene* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg)	override;
	virtual void Free() override;
};

END