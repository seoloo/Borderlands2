#pragma once

#include "Tool_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CTransform;
class CNavigation;
class CVIBuffer_Terrain;
END

BEGIN(Tool)

class CNavi final : public CGameObject
{
private:
	CNavi(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNavi(const CNavi& rhs);
	virtual ~CNavi() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	CShader* m_pShaderCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CTransform* m_pTransformCom = { nullptr };
	CNavigation* m_pNavigationCom = { nullptr };
	CVIBuffer_Terrain* m_pVIBufferCom = { nullptr };

private:
	class CImgui_Manager* m_pImgui_Manager = { nullptr };

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CNavi* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg)	override;
	virtual void Free() override;
};

END
