#pragma once

#include "EffectTool_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CNavigation;
class CVIBuffer_Terrain;
END

BEGIN(EffectTool)

class CEffect_Terrain final : public CGameObject
{
public:
	enum TEXTURETYPE { TYPE_DIFFUSE, TYPE_MASK, TYPE_BRUSH, TYPE_END };

private:
	CEffect_Terrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect_Terrain(const CEffect_Terrain& rhs);
	virtual ~CEffect_Terrain() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	CShader*			m_pShaderCom = { nullptr };
	CTexture*			m_pTextureCom[TYPE_END] = { nullptr };
	CRenderer*			m_pRendererCom = { nullptr };
	CTransform*			m_pTransformCom = { nullptr };
	CVIBuffer_Terrain*	m_pVIBufferCom = { nullptr };
	CNavigation* m_pNavigationCom = { nullptr };

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CEffect_Terrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg)	override;
	virtual void Free() override;
};

END