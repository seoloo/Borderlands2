#include "Light.h"
#include "Shader.h"
#include "VIBuffer_Rect.h"

CLight::CLight(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CLight::Initialize(const LIGHTDESC& LightDesc)
{
	memmove(&m_LightDesc, &LightDesc, sizeof m_LightDesc);

	return S_OK;
}

HRESULT CLight::Render(CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
	_uint			iPassIndex = 0;

	if (LIGHTDESC::TYPE_DIRECTIONAL == m_LightDesc.eType)
	{
		iPassIndex = 1;

		if (FAILED(pShader->Bind_RawValue("g_vLightDir", &m_LightDesc.vDirection, sizeof(_float4))))
			return E_FAIL;

		if (FAILED(pShader->Bind_RawValue("g_vLightDiffuse", &m_LightDesc.vDiffuse, sizeof(_float4))))
			return E_FAIL;
		if (FAILED(pShader->Bind_RawValue("g_vLightAmbient", &m_LightDesc.vAmbient, sizeof(_float4))))
			return E_FAIL;
		if (FAILED(pShader->Bind_RawValue("g_vLightSpecular", &m_LightDesc.vSpecular, sizeof(_float4))))
			return E_FAIL;
	}
	else if(LIGHTDESC::TYPE_POINT == m_LightDesc.eType)
	{
		iPassIndex = 2;

		if (!m_LightDesc.bTurnOff)
		{
			if (FAILED(pShader->Bind_RawValue("g_vLightPos", &m_LightDesc.vPosition, sizeof(_float4))))
				return E_FAIL;
			if (FAILED(pShader->Bind_RawValue("g_fLightRange", &m_LightDesc.fRange, sizeof(_float))))
				return E_FAIL;
			if (FAILED(pShader->Bind_RawValue("g_vLightDiffuse", &m_LightDesc.vDiffuse, sizeof(_float4))))
				return E_FAIL;
			if (FAILED(pShader->Bind_RawValue("g_vLightAmbient", &m_LightDesc.vAmbient, sizeof(_float4))))
				return E_FAIL;
			if (FAILED(pShader->Bind_RawValue("g_vLightSpecular", &m_LightDesc.vSpecular, sizeof(_float4))))
				return E_FAIL;
		}


	}
	else if (LIGHTDESC::TYPE_SPOTLIGHT == m_LightDesc.eType)
	{
		iPassIndex = 5;

		if (FAILED(pShader->Bind_RawValue("g_fLightRange", &m_LightDesc.fRange, sizeof(_float))))
			return E_FAIL;
		if (FAILED(pShader->Bind_RawValue("g_vLightPos", &m_LightDesc.vPosition, sizeof(_float4))))
			return E_FAIL;
		if (FAILED(pShader->Bind_RawValue("g_vSpotLightPos", &m_LightDesc.vSpotlightPosition, sizeof(_float4))))
			return E_FAIL;
		if (FAILED(pShader->Bind_RawValue("g_vLightDiffuse", &m_LightDesc.vDiffuse, sizeof(_float4))))
			return E_FAIL;
		if (FAILED(pShader->Bind_RawValue("g_vLightAmbient", &m_LightDesc.vAmbient, sizeof(_float4))))
			return E_FAIL;
		if (FAILED(pShader->Bind_RawValue("g_vLightSpecular", &m_LightDesc.vSpecular, sizeof(_float4))))
			return E_FAIL;
	}

	pShader->Begin(iPassIndex);

	pVIBuffer->Render();

	return S_OK;
}

CLight* CLight::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const LIGHTDESC& LightDesc)
{
	CLight* pInstance = new CLight(pDevice, pContext);

	if (FAILED(pInstance->Initialize(LightDesc)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Created : CLight");
	}

	return pInstance;
}

void CLight::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
