#include "Picking.h"

#include "GameInstance.h"
#include "PipeLine.h"

#include "Bounding_Sphere.h"
#include "Bounding_AABB.h"
#include "Bounding_OBB.h"

IMPLEMENT_SINGLETON(CPicking)

CPicking::CPicking()
{
}

HRESULT CPicking::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, HWND hWnd)
{
    m_pDeivce = pDevice;
    m_pContext = pContext;

    m_hWnd = hWnd;

    return S_OK;
}

void CPicking::Tick()
{
	POINT		ptMouse;

	GetCursorPos(&ptMouse);

	/* ����Ʈ ���� ���콺 ��ġ��ǥ�� ���Ѵ�. */
	/* ��ġ : ������ġ * ���� * �� * ���� /W ->(-1, 1 -> 0, 0  1, -1 -> wsx, wsy) ����Ʈ��ȯ */
	ScreenToClient(m_hWnd, &ptMouse);

	/* ���������̽� ���� ���콺 ��ǥ�� ���Ѵ�. */
	/* ��ġ : ������ġ * ���� * �� * ���� /W  */
	_float3		vMouseNearPos;
	/* x : 0 -> -1, WINSIZEX -> 1 */
	/* y : 0 -> 1, WINSIZEY -> -1 */
	RECT		rcClient;
	GetClientRect(m_hWnd, &rcClient);

	/* ������������ ��ġ��°� 2d��ǥ. �����׽�Ʈ�� ���� ���̸� 0 ~ 1�� �����Ͽ� �����ϳ�. */
	vMouseNearPos.x = ptMouse.x / (rcClient.right * 0.5f) - 1.f;
	vMouseNearPos.y = -ptMouse.y / (rcClient.bottom * 0.5f) + 1.f;
	vMouseNearPos.z = 0.f; /* near����� Ŭ���Ͽ��� ������. */

	_vector vecMouseNearPos;
	vecMouseNearPos = XMLoadFloat3(&vMouseNearPos);

	/* �佺���̽����� ���콺 Ray, ���콺 RayPos�� ���Ѵ�. */
	/* ��ġ : ������ġ * ���� * �� * ���� /W * ������ �� */
	//m_pGraphic_Device->GetTransform(D3DTS_PROJECTION, &ProjMatrix);
	// ���������ο� �ִ� ����ȯ�� ������̶� ������� 
	//InvProMatrix = CPipline::GetINv
	_matrix		InvProjMatrix = CGameInstance::Get_Instance()->Get_Transform_Matrix_Inverse(CPipeLine::D3DTS_PROJ);
	_matrix     InvViewMatrix = CGameInstance::Get_Instance()->Get_Transform_Matrix_Inverse(CPipeLine::D3DTS_VIEW);

	/* �佺���̽� ���� ���콺 ��ġ. */
	vecMouseNearPos = XMVector3TransformCoord(vecMouseNearPos, InvProjMatrix);

	_vector MousePos = XMVectorSet(0.f, 0.f, 0.f, 1.f);
	_vector MouseRayDir = vecMouseNearPos - MousePos;

	MousePos = XMVector4Transform(MousePos, InvViewMatrix);
	MouseRayDir = XMVector4Transform(MouseRayDir, InvViewMatrix);

	XMStoreFloat3(&m_vMouseRay, MouseRayDir);
	XMStoreFloat3(&m_vMouseRayPos, MousePos);
	//return;

	// ����� �ִ� ���콺 ��ġ ���� 

	/* ���콺 Ray, RayPos�� ���� ������ �ﰢ������ �浹�� ���� �ʿ��� �������̱⶧����.  */
	/* D3DXIntersectTri(��1, ��2, ��3, RayPos, Ray); */
	//���⿬�곡
	/* ���� �����̽� ���� ���콺 Ray, RayPos */
	/* ��ġ : ������ġ * ���� * �� * ���� �� */
	// ������ ���߿� ī������ 
	/*m_vMouseRayPos = _float3(0.f, 0.f, 0.f);
	m_vMouseRay = vMousePos - m_vMouseRayPos;*/

}

void CPicking::Transform_ToLocalSpace(const _float4x4& WorldMatrix)
{
	_matrix WorldMatrixInv = XMLoadFloat4x4(&WorldMatrix);
	WorldMatrixInv = XMMatrixInverse(nullptr, WorldMatrixInv); // ����ȯ�ؼ�
	// _XmLoad-- > _float3 �� _vector �ٲ� ����

	_vector vMouseRayPos_Local;
	_vector vMouseRay_Local;

	vMouseRayPos_Local = XMLoadFloat3(&m_vMouseRayPos);
	vMouseRay_Local = XMLoadFloat3(&m_vMouseRay);

	XMVector3TransformCoord(vMouseRayPos_Local, WorldMatrixInv);
	XMVector3TransformNormal(vMouseRay_Local, WorldMatrixInv);

	vMouseRay_Local = XMVector3Normalize(vMouseRay_Local);

	XMStoreFloat3(&m_vMouseRayPos_Local, vMouseRayPos_Local);
	XMStoreFloat3(&m_vMouseRay_Local, vMouseRay_Local);
}


_bool CPicking::Intersect_Triangle_InLocal(const _vector pPointA, const _vector pPointB, const _vector pPointC, _float3* pOut)
{
	_float		fDist = 0.f;
	_bool		isColl = { false };

	_vector    vMouseRayPos;
	_vector    vMouseRay;

	_vector		vMouseRay_Local = XMLoadFloat3(&m_vMouseRay_Local);
	_vector		vMouseRayPos_Local = XMLoadFloat3(&m_vMouseRayPos_Local);

	vMouseRayPos = XMLoadFloat3(&m_vMouseRayPos_Local);
	vMouseRay = XMLoadFloat3(&m_vMouseRay_Local);

	isColl = DirectX::TriangleTests::Intersects(vMouseRayPos, vMouseRay, pPointA, pPointB, pPointC, fDist);

	if (true == isColl)
	{
		_vector vMouseIntersection = vMouseRay_Local * fDist + vMouseRayPos_Local;

		XMStoreFloat3(pOut, vMouseIntersection);
	}

	return isColl;
}

_bool CPicking::Intersect_Collider(CBounding* pCollider)
{
	_float		fDist = 0.f;
	_bool		isColl = { false };

	_vector    vMouseRayPos = {XMVectorSet(0.f,0.f,0.f,0.f)};
	_vector    vMouseRay = { XMVectorSet(0.f,0.f,0.f,0.f) };

	_vector		vMouseRay_Local = XMLoadFloat3(&m_vMouseRay_Local);
	_vector		vMouseRayPos_Local = XMLoadFloat3(&m_vMouseRayPos_Local);

	vMouseRayPos = XMLoadFloat3(&m_vMouseRayPos);
	vMouseRay = XMLoadFloat3(&m_vMouseRay);

	vMouseRay = XMVector3Normalize(vMouseRay);
	
	if (pCollider->Intersect_Mouse(vMouseRayPos, vMouseRay, fDist))
		isColl = true;

	return isColl;
}

void CPicking::Free()
{
    Safe_Release(m_pDeivce);
    Safe_Release(m_pContext);
}
