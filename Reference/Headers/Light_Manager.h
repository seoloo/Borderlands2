#pragma once
#include "Base.h"

BEGIN(Engine)

class CLight_Manager final : public CBase
{
	DECLARE_SINGLETON(CLight_Manager)
private:
	CLight_Manager();
	virtual ~CLight_Manager() = default;

public:
	const LIGHTDESC* Get_LightDesc(_uint iIndex);

public:
	HRESULT Add_Lights(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const LIGHTDESC& LightDesc);
	HRESULT Render_Lights(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
	void	Update_LightsPos(_float4 Position = _float4(0.f,0.f,0.f,0.f));
	void	Lights_TurnOff(_bool TurnOff = false);
	HRESULT	Remove_PointLight();

private:
	list<class CLight*>			m_Lights;
	LIGHTDESC	m_LightDesc;
	class CLight* m_pLights;

private:
	virtual void Free() override;
};

END