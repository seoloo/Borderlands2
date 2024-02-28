#include "PipeLine.h"

IMPLEMENT_SINGLETON(CPipeLine)

CPipeLine::CPipeLine()
{
}

HRESULT CPipeLine::Initialize()
{
	return S_OK;
}

void CPipeLine::Tick()
{
	for (size_t i = 0; i < D3DTS_END; i++)
	{
		XMStoreFloat4x4(&m_TransformMatrixInv[i],
			XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_TransformMatrix[i])));
	}

	memcpy(&m_vCamPosition, &m_TransformMatrixInv[D3DTS_VIEW].m[3][0], sizeof(_float4));
}

void CPipeLine::Free()
{
}
