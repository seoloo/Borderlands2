#pragma once

#include "Tool_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CNavigation;
class CVIBuffer_Terrain;
END

BEGIN(Tool)

class CTool_Terrain final : public CGameObject
{
public:
	enum TEXTURETYPE { TYPE_DIFFUSE, TYPE_MASK, TYPE_BRUSH, TYPE_END };

private:
	CTool_Terrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTool_Terrain(const CTool_Terrain& rhs);
	virtual ~CTool_Terrain() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();
	void	Change_Terrain();

private:
	CShader*			m_pShaderCom = { nullptr };
	CTexture*			m_pTextureCom_Origin[TYPE_END] = { nullptr };
	CTexture*			m_pTextureCom_Snow[TYPE_END] = { nullptr };
	CTexture*			m_pTextureCom_Snow2[TYPE_END] = { nullptr };
	CTexture*			m_pTextureCom_Rock[TYPE_END] = { nullptr };
	CTexture*			m_pTextureCom_Lava[TYPE_END] = { nullptr };
	CTexture*			m_pTextureCom_Grassland[TYPE_END] = { nullptr };
	CTexture*			m_pTextureCom_FrozenLake[TYPE_END] = { nullptr };
	CTexture*			m_pTextureCom_Ground[TYPE_END] = { nullptr };
	CRenderer*			m_pRendererCom = { nullptr };
	CTransform*			m_pTransformCom = { nullptr };
	CVIBuffer_Terrain*	m_pVIBufferCom = { nullptr };
	CNavigation* m_pNavigationCom = { nullptr };

private:
	class CImgui_Manager* m_pImgui_Manager = { nullptr };

private:
	_vector		m_vRight = { 0.f, 0.f, 0.f };
	_vector		m_vUp = { 0.f, 0.f, 0.f };
	_vector		m_vPos = { 0.f, 0.f, 0.f };
	_float2		m_fRadian = { 0.f, 0.f };

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CTool_Terrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg)	override;
	virtual void Free() override;
};

END