#include "..\Public\LandObject.h"
#include "VIBuffer_Terrain.h"
#include "Transform.h"


CLandObject::CLandObject(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CLandObject::CLandObject(const CLandObject & rhs)
	: CGameObject(rhs)
{
}


HRESULT CLandObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CLandObject::Initialize(void * pArg)
{
	return S_OK;
}

void CLandObject::Tick(_float fTimeDelta)
{
}

void CLandObject::Late_Tick(_float fTimeDelta)
{
}

HRESULT CLandObject::Render()
{
	return S_OK;
}

_vector CLandObject::SetUp_OnTerrain(_fvector vWorldPos, class CVIBuffer_Terrain* pTerrainVIBuffer, class CTransform* pTerrainTransform )
{
	_vector		vLocalPos = XMVector3TransformCoord(vWorldPos, pTerrainTransform->Get_WorldMatrix_Inverse());	

	vLocalPos = XMVectorSetY(vLocalPos, pTerrainVIBuffer->Compute_Height(vLocalPos));

	return XMVector3TransformCoord(vLocalPos, pTerrainTransform->Get_WorldMatrix_Float4x4());	
}


void CLandObject::Free()
{
	__super::Free();
}
