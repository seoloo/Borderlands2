#include "Transform.h"
#include "Shader.h"

#include "Navigation.h"

CTransform::CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

CTransform::CTransform(const CTransform& rhs)
	: CComponent(rhs)
	, m_WorldMatrix(rhs.m_WorldMatrix)
{
}

_vector CTransform::Get_State(STATE eState)
{
	/* float2, 3, 4, 4x4 -> vector or matrix : Load */
	/* vector or matrix -> float2344x4   : Store */

	/* XMLoadFloat4x4 : 4x4 행렬을 로드하여 XMMATRIX 형식으로 반환하는 역할*/
	_matrix		WorldMatrix = XMLoadFloat4x4(&m_WorldMatrix);

	/* WorldMatrix.r : 행렬의 각 행을 나타내는 4개의 SIMD 레지스터(벡터)(연산)를 의미 */
	return WorldMatrix.r[eState];
}

_float3 CTransform::Get_Scaled()
{
	/* XMVectorGetX : XMVECTOR 타입의 벡터에서 X요소 값을 가져오는 역할*/
	return _float3(XMVectorGetX(XMVector3Length(Get_State(STATE_RIGHT))),
		XMVectorGetX(XMVector3Length(Get_State(STATE_UP))),
		XMVectorGetX(XMVector3Length(Get_State(STATE_LOOK))));
}

void CTransform::Set_State(STATE eState, _fvector vState)
{
	/* float2, 3, 4, 4x4 -> vector or matrix : Load */
	/* vector or matrix -> float2344x4   : Store */

	_matrix		WorldMatrix = XMLoadFloat4x4(&m_WorldMatrix);

	WorldMatrix.r[eState] = vState;

	XMStoreFloat4x4(&m_WorldMatrix, WorldMatrix);
}

HRESULT CTransform::Initialize_Prototype()
{
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

	return S_OK;
}

HRESULT CTransform::Initialize(void* pArg)
{
	if (nullptr != pArg)
		memmove(&m_TransformDesc, pArg, sizeof m_TransformDesc);

	return S_OK;
}

HRESULT CTransform::Bind_ShaderResource(CShader* pShader, const char* pConstantName)
{
	return pShader->Bind_Matrix(pConstantName, &m_WorldMatrix);
}

void CTransform::Go_Look(_vector Look, _float fTimeDelta, CNavigation* pNavigation)
{
	_vector vPosition = Get_State(STATE_POSITION);

	if (nullptr == pNavigation || pNavigation->is_Move(vPosition))
	{
		vPosition += XMVector3Normalize(Look) * m_TransformDesc.fSpeedPerSec * fTimeDelta;

		Set_State(STATE_POSITION, vPosition);
	}
}

void CTransform::Go_Straight(_float fTimeDelta, CNavigation* pNavigation)
{
	_vector		vPosition = Get_State(STATE_POSITION);
	_vector		vLook = Get_State(STATE_LOOK);
	_vector		vDir = XMVectorSet(0.f, 0.f, 0.f, 0.f);
	_vector		vNormal = XMVectorSet(0.f, 0.f, 0.f, 0.f);
	_vector		vSliding = XMVectorSet(0.f, 0.f, 0.f, 0.f);
	_float		fdotProduct = XMVectorGetX(XMVector3Dot(vLook, vNormal));

	_float3		vLine = { 0.f,0.f,0.f };

	if (nullptr == pNavigation || !pNavigation->is_Move(vPosition))
	{
		m_vFirstPos = pNavigation->Get_FirstPos();
		m_vSecondPos = pNavigation->Get_SecondPos();

		_vector		vFirstPos = XMLoadFloat3(&m_vFirstPos);
		_vector		vSecondPos = XMLoadFloat3(&m_vSecondPos);

		_vector		vBoundaryMax = XMVectorMax(vFirstPos, vSecondPos);
		_vector		vBoundaryMin = XMVectorMin(vFirstPos, vSecondPos);

		vLine = _float3((m_vFirstPos.x - m_vSecondPos.x),
			(m_vFirstPos.y - m_vSecondPos.y), (m_vFirstPos.z - m_vSecondPos.z));

		vDir = XMVector3Normalize(XMLoadFloat3(&m_vFirstPos) - XMLoadFloat3(&m_vSecondPos));
		vNormal = XMVectorSet(-XMVectorGetY(vDir), XMVectorGetX(vDir), 0, 0);
		vSliding = vLook - vNormal * fdotProduct;

		vPosition = XMVectorClamp(vPosition, vBoundaryMin, vBoundaryMax);

		vPosition += XMVector3Normalize(vSliding) * m_TransformDesc.fSpeedPerSec * fTimeDelta;

		Set_State(STATE_POSITION, vPosition);
	}

	if (nullptr == pNavigation || pNavigation->is_Move(vPosition))
	{
		m_fAcceleration += m_fMaxAcceleration * fTimeDelta;

		m_fAcceleration = std::min(m_fAcceleration, m_fMaxAcceleration);

		_vector vLastVelocity = XMVectorZero();

		_vector currentVelocity = XMVector3Normalize(vLook) * m_TransformDesc.fSpeedPerSec;

		_vector acceleratedVelocity = vLastVelocity +
			(currentVelocity - vLastVelocity) * m_fAcceleration;

		vPosition += acceleratedVelocity * fTimeDelta;

		Set_State(STATE_POSITION, vPosition);

		vLastVelocity = acceleratedVelocity;
	}
}

void CTransform::Go_Backward(_float fTimeDelta, CNavigation* pNavigation)
{
	_vector		vPosition = Get_State(STATE_POSITION);
	_vector		vLook = Get_State(STATE_LOOK);
	_vector		vDir = XMVectorSet(0.f, 0.f, 0.f, 0.f);
	_vector		vNormal = XMVectorSet(0.f, 0.f, 0.f, 0.f);
	_vector		vSliding = XMVectorSet(0.f, 0.f, 0.f, 0.f);
	_float		fdotProduct = XMVectorGetX(XMVector3Dot(vLook, vNormal));


	_float3		vLine = { 0.f,0.f,0.f };

	if (nullptr == pNavigation || !pNavigation->is_Move(vPosition))
	{
		m_vFirstPos = pNavigation->Get_FirstPos();
		m_vSecondPos = pNavigation->Get_SecondPos();

		_vector		vFirstPos = XMLoadFloat3(&m_vFirstPos);
		_vector		vSecondPos = XMLoadFloat3(&m_vSecondPos);

		_vector vBoundaryMax = XMVectorMax(vFirstPos, vSecondPos);
		_vector vBoundaryMin = XMVectorMin(vFirstPos, vSecondPos);

		vLine = _float3((m_vFirstPos.x - m_vSecondPos.x), (m_vFirstPos.y - m_vSecondPos.y), (m_vFirstPos.z - m_vSecondPos.z));
		vDir = XMVector3Normalize(XMLoadFloat3(&m_vFirstPos) - XMLoadFloat3(&m_vSecondPos));
		vNormal = XMVectorSet(-XMVectorGetY(vDir), XMVectorGetX(vDir), 0, 0);
		vSliding = vLook - vNormal * fdotProduct;

		vPosition = XMVectorClamp(vPosition, vBoundaryMin, vBoundaryMax);

		vPosition -= XMVector3Normalize(vSliding) * m_TransformDesc.fSpeedPerSec * fTimeDelta;

		Set_State(STATE_POSITION, vPosition);
	}

	if (nullptr == pNavigation || pNavigation->is_Move(vPosition))
	{
		vPosition -= XMVector3Normalize(vLook) * m_TransformDesc.fSpeedPerSec * fTimeDelta;

		Set_State(STATE_POSITION, vPosition);
	}
}

void CTransform::Go_Left(_float fTimeDelta, CNavigation* pNavigation)
{
	_vector		vPosition = Get_State(STATE_POSITION);
	_vector		vRight = Get_State(STATE_RIGHT);

	_vector		vDir = XMVectorSet(0.f, 0.f, 0.f, 0.f);
	_vector		vNormal = XMVectorSet(0.f, 0.f, 0.f, 0.f);
	_vector		vSliding = XMVectorSet(0.f, 0.f, 0.f, 0.f);
	_float		fdotProduct = XMVectorGetX(XMVector3Dot(vRight, vNormal));


	_float3		vLine = { 0.f,0.f,0.f };

	if (nullptr == pNavigation || !pNavigation->is_Move(vPosition))
	{
		m_vFirstPos = pNavigation->Get_FirstPos();
		m_vSecondPos = pNavigation->Get_SecondPos();

		_vector		vFirstPos = XMLoadFloat3(&m_vFirstPos);
		_vector		vSecondPos = XMLoadFloat3(&m_vSecondPos);

		_vector vBoundaryMax = XMVectorMax(vFirstPos, vSecondPos);
		_vector vBoundaryMin = XMVectorMin(vFirstPos, vSecondPos);

		vLine = _float3((m_vFirstPos.x - m_vSecondPos.x), (m_vFirstPos.y - m_vSecondPos.y), (m_vFirstPos.z - m_vSecondPos.z));
		vDir = XMVector3Normalize(XMLoadFloat3(&m_vFirstPos) - XMLoadFloat3(&m_vSecondPos));
		vNormal = XMVectorSet(-XMVectorGetY(vDir), XMVectorGetX(vDir), 0, 0);
		vSliding = vRight - vNormal * fdotProduct;

		vPosition = XMVectorClamp(vPosition, vBoundaryMin, vBoundaryMax);

		vPosition -= XMVector3Normalize(vSliding) * m_TransformDesc.fSpeedPerSec * fTimeDelta;

		Set_State(STATE_POSITION, vPosition);
	}

	if (nullptr == pNavigation || pNavigation->is_Move(vPosition))
	{
		vPosition -= XMVector3Normalize(vRight) * m_TransformDesc.fSpeedPerSec * fTimeDelta;

		Set_State(STATE_POSITION, vPosition);
	}
}

void CTransform::Go_Right(_float fTimeDelta, CNavigation* pNavigation)
{
	_vector		vPosition = Get_State(STATE_POSITION);
	_vector		vRight = Get_State(STATE_RIGHT);

	_vector		vDir = XMVectorSet(0.f, 0.f, 0.f, 0.f);
	_vector		vNormal = XMVectorSet(0.f, 0.f, 0.f, 0.f);
	_vector		vSliding = XMVectorSet(0.f, 0.f, 0.f, 0.f);
	_float		fdotProduct = XMVectorGetX(XMVector3Dot(vRight, vNormal));


	_float3		vLine = { 0.f,0.f,0.f };

	if (nullptr == pNavigation || !pNavigation->is_Move(vPosition))
	{
		m_vFirstPos = pNavigation->Get_FirstPos();
		m_vSecondPos = pNavigation->Get_SecondPos();

		_vector		vFirstPos = XMLoadFloat3(&m_vFirstPos);
		_vector		vSecondPos = XMLoadFloat3(&m_vSecondPos);

		_vector vBoundaryMax = XMVectorMax(vFirstPos, vSecondPos);
		_vector vBoundaryMin = XMVectorMin(vFirstPos, vSecondPos);

		vLine = _float3((m_vFirstPos.x - m_vSecondPos.x), (m_vFirstPos.y - m_vSecondPos.y), (m_vFirstPos.z - m_vSecondPos.z));
		vDir = XMVector3Normalize(XMLoadFloat3(&m_vFirstPos) - XMLoadFloat3(&m_vSecondPos));
		vNormal = XMVectorSet(-XMVectorGetY(vDir), XMVectorGetX(vDir), 0, 0);
		vSliding = vRight - vNormal * fdotProduct;

		vPosition = XMVectorClamp(vPosition, vBoundaryMin, vBoundaryMax);

		vPosition += XMVector3Normalize(vSliding) * m_TransformDesc.fSpeedPerSec * fTimeDelta;

		Set_State(STATE_POSITION, vPosition);
	}

	if (nullptr == pNavigation || pNavigation->is_Move(vPosition))
	{
		vPosition += XMVector3Normalize(vRight) * m_TransformDesc.fSpeedPerSec * fTimeDelta;

		Set_State(STATE_POSITION, vPosition);
	}
}

void CTransform::Rotation(_fvector vAxis, _float fRadian)
{
	_matrix		RotationMatrix = XMMatrixRotationAxis(vAxis, fRadian);
	_float3		vScaled = Get_Scaled();

	_vector		vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * vScaled.x;
	_vector		vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * vScaled.y;
	_vector		vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * vScaled.z;

	/* 어차피 w가 0이기 때문에 3나 4나 같다 */
	Set_State(STATE_RIGHT, XMVector4Transform(vRight, RotationMatrix));
	Set_State(STATE_UP, XMVector3TransformNormal(vUp, RotationMatrix));
	Set_State(STATE_LOOK, XMVector3TransformNormal(vLook, RotationMatrix));
}

void CTransform::Turn(_fvector vAxis, _float fTimeDelta)
{
	_matrix		RotationMatrix = XMMatrixRotationAxis(vAxis, m_TransformDesc.fRotationPerSec * fTimeDelta);

	_vector		vRight = Get_State(STATE_RIGHT);
	_vector		vUp = Get_State(STATE_UP);
	_vector		vLook = Get_State(STATE_LOOK);

	Set_State(STATE_RIGHT, XMVector4Transform(vRight, RotationMatrix));
	Set_State(STATE_UP, XMVector3TransformNormal(vUp, RotationMatrix));
	Set_State(STATE_LOOK, XMVector3TransformNormal(vLook, RotationMatrix));
}

void CTransform::Scaled(const _float3& vScale)
{
	Set_State(STATE_RIGHT, XMVector3Normalize(Get_State(STATE_RIGHT)) * vScale.x);
	Set_State(STATE_UP, XMVector3Normalize(Get_State(STATE_UP)) * vScale.y);
	Set_State(STATE_LOOK, XMVector3Normalize(Get_State(STATE_LOOK)) * vScale.z);
}

void CTransform::Chase(_fvector vTargetPos, _float fTimeDelta, _float fLimit)
{
	_vector		vPosition = Get_State(STATE_POSITION);

	_vector		vLook = vTargetPos - vPosition;

	_float		fDistance = XMVectorGetX(XMVector3Length(vLook));

	/* 일정 거리로 제한 */
	if (fDistance >= fLimit)
		vPosition += XMVector3Normalize(vLook) * m_TransformDesc.fSpeedPerSec * fTimeDelta;
}

void CTransform::LookAt(_fvector vTargetPos)
{
	_vector		vPosition = Get_State(STATE_POSITION);

	_vector		vLook = vTargetPos - vPosition;

	_vector		vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);

	_vector		vUp = XMVector3Cross(vLook, vRight);

	_float3		vScaled = Get_Scaled();

	Set_State(STATE_RIGHT, vRight);
	Set_State(STATE_UP, vUp);
	Set_State(STATE_LOOK, vLook);

	Scaled(vScaled);
}

void CTransform::ToolGo_Straight(_float fTimeDelta, CNavigation* pNavigation)
{
	_vector		vPosition = Get_State(STATE_POSITION);
	_vector		vLook = Get_State(STATE_LOOK);

	if (nullptr == pNavigation || pNavigation->is_Move(vPosition))
	{
		vPosition += XMVector3Normalize(vLook) * m_TransformDesc.fSpeedPerSec * fTimeDelta;

		Set_State(STATE_POSITION, vPosition);
	}
}

void CTransform::ToolGo_Backward(_float fTimeDelta, CNavigation* pNavigation)
{
	_vector		vPosition = Get_State(STATE_POSITION);
	_vector		vLook = Get_State(STATE_LOOK);

	if (nullptr == pNavigation || pNavigation->is_Move(vPosition))
	{
		vPosition -= XMVector3Normalize(vLook) * m_TransformDesc.fSpeedPerSec * fTimeDelta;

		Set_State(STATE_POSITION, vPosition);
	}
}

void CTransform::ToolGo_Left(_float fTimeDelta, CNavigation* pNavigation)
{
	_vector		vPosition = Get_State(STATE_POSITION);
	_vector		vRight = Get_State(STATE_RIGHT);

	if (nullptr == pNavigation || pNavigation->is_Move(vPosition))
	{
		vPosition -= XMVector3Normalize(vRight) * m_TransformDesc.fSpeedPerSec * fTimeDelta;

		Set_State(STATE_POSITION, vPosition);
	}
}

void CTransform::ToolGo_Right(_float fTimeDelta, CNavigation* pNavigation)
{
	_vector		vPosition = Get_State(STATE_POSITION);
	_vector		vRight = Get_State(STATE_RIGHT);

	if (nullptr == pNavigation || pNavigation->is_Move(vPosition))
	{
		vPosition += XMVector3Normalize(vRight) * m_TransformDesc.fSpeedPerSec * fTimeDelta;

		Set_State(STATE_POSITION, vPosition);
	}
}

CTransform* CTransform::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTransform* pInstance = new CTransform(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Created : CTransform");
	}

	return pInstance;
}

CComponent* CTransform::Clone(void* pArg)
{
	CTransform* pInstance = new CTransform(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Cloned : CTransform");
	}

	return pInstance;
}

void CTransform::Free()
{
	__super::Free();
}
