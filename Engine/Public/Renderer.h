#pragma once

#include "Component.h"

/* 화면에 그려지는 객체들을 그리는 순서대로 모아놓는다. */
/* 보관하고 있는 순서대로 객체들의 렌더함수를 호출해준다. */

BEGIN(Engine)

class ENGINE_DLL CRenderer final : public CComponent
{
public:
	enum RENDERGROUP { RENDER_PRIORITY, RENDER_SHADOWDEPTH, RENDER_NONLIGHT, RENDER_NONBLEND, RENDER_BLEND, RENDER_UI, RENDER_END };
private:
	CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CRenderer() = default;

public:
	virtual HRESULT Initialize_Prototype() override;	

public:
	HRESULT Add_RenderGroup(RENDERGROUP eRenderGroup, class CGameObject* pGameObject);
	HRESULT Draw_RenderGroup();

#ifdef _DEBUG
public:
	HRESULT Add_DebugComponent(class CComponent* pDebugCom);

#endif

private:
	list<class CGameObject*>			m_RenderGroup[RENDER_END];
	list<class CComponent*>				m_DebugComponents;
	class CTarget_Manager* m_pTarget_Manager = { nullptr };
	class CLight_Manager* m_pLight_Manager = { nullptr };

	/* 1. 렌더타겟들을 디버그용도로 그릴 때. */
	/* 2. 셰이드 타겟에 빛 연산의 결과를 그릴 때 */
	/* 3. 백버퍼에 최종적인 연산 결과를 그릴 때 */

	class CVIBuffer_Rect* m_pVIBuffer = { nullptr };
	class CShader* m_pShader = { nullptr };

private:
	_float4x4							m_WorldMatrix_Deferred;
	_float4x4							m_ViewMatrix_Deferred;
	_float4x4							m_ProjMatrix_Deferred;

private:
	HRESULT Render_Priority();
	HRESULT Render_ShadowDepth();

	HRESULT Render_NonBlend();
	HRESULT Render_LightAcc();
	HRESULT Render_Deferred();
	HRESULT Render_NonLight();

	HRESULT Render_Blend();
	HRESULT Render_UI();

	
#ifdef _DEBUG
private:
	HRESULT Render_Debug();
#endif

public:
	static CRenderer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END