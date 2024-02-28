#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Point_Instance;
END

BEGIN(Client)

class CSnowing final : public CGameObject
{
private:
	CSnowing(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSnowing(const CSnowing& rhs);
	virtual ~CSnowing() = default;

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
	CVIBuffer_Point_Instance*		m_pVIBufferCom = { nullptr };
	

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CSnowing* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg)	override;
	virtual void Free() override;
};

END