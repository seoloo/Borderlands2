#include "Light_Manager.h"
#include "Light.h"

IMPLEMENT_SINGLETON(CLight_Manager)

CLight_Manager::CLight_Manager()
{
}

const LIGHTDESC* CLight_Manager::Get_LightDesc(_uint iIndex)
{
	if (iIndex >= m_Lights.size())
		return nullptr;

	auto	iter = m_Lights.begin();

	for (size_t i = 0; i < iIndex; i++)
		++iter;

	return (*iter)->Get_LightDesc();
}

HRESULT CLight_Manager::Add_Lights(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const LIGHTDESC& LightDesc)
{
	m_pLights = CLight::Create(pDevice, pContext, LightDesc);
	if (nullptr == m_pLights)
		return E_FAIL;

	m_Lights.push_back(m_pLights);

	m_LightDesc = LightDesc;

	return S_OK;
}

HRESULT CLight_Manager::Render_Lights(CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
	for (auto& pLight : m_Lights)
	{
		pLight->Render(pShader, pVIBuffer);
	}
	return S_OK;
}

void CLight_Manager::Update_LightsPos(_float4 Position)
{
	m_pLights->Set_LightPos(Position);
}

void CLight_Manager::Lights_TurnOff(_bool TurnOff)
{
	m_pLights->Set_LightTurnOff(TurnOff);
}

HRESULT CLight_Manager::Remove_PointLight()
{
	for (auto iter = m_Lights.begin(); iter != m_Lights.end();)
	{
		if (*iter != nullptr && (*iter)->Get_LightDesc()->eType == LIGHTDESC::TYPE_END)
			iter = m_Lights.erase(iter);
		else
			++iter;
	}

	return S_OK;
}

void CLight_Manager::Free()
{
	for (auto& pLight : m_Lights)
		Safe_Release(pLight);

	m_Lights.clear();
}
