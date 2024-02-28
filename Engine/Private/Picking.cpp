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

	/* 뷰포트 상의 마우스 위치좌표를 구한다. */
	/* 위치 : 로컬위치 * 월드 * 뷰 * 투영 /W ->(-1, 1 -> 0, 0  1, -1 -> wsx, wsy) 뷰포트변환 */
	ScreenToClient(m_hWnd, &ptMouse);

	/* 투영스페이스 상의 마우스 좌표를 구한다. */
	/* 위치 : 로컬위치 * 월드 * 뷰 * 투영 /W  */
	_float3		vMouseNearPos;
	/* x : 0 -> -1, WINSIZEX -> 1 */
	/* y : 0 -> 1, WINSIZEY -> -1 */
	RECT		rcClient;
	GetClientRect(m_hWnd, &rcClient);

	/* 투영공간상의 위치라는건 2d좌표. 깊이테스트를 위해 깊이를 0 ~ 1로 보간하여 저장하낟. */
	vMouseNearPos.x = ptMouse.x / (rcClient.right * 0.5f) - 1.f;
	vMouseNearPos.y = -ptMouse.y / (rcClient.bottom * 0.5f) + 1.f;
	vMouseNearPos.z = 0.f; /* near평면을 클릭하였기 때문에. */

	_vector vecMouseNearPos;
	vecMouseNearPos = XMLoadFloat3(&vMouseNearPos);

	/* 뷰스페이스상의 마우스 Ray, 마우스 RayPos을 구한다. */
	/* 위치 : 로컬위치 * 월드 * 뷰 * 투영 /W * 투영의 역 */
	//m_pGraphic_Device->GetTransform(D3DTS_PROJECTION, &ProjMatrix);
	// 파이프라인에 있는 역변환된 뷰행렬이랑 투영행렬 
	//InvProMatrix = CPipline::GetINv
	_matrix		InvProjMatrix = CGameInstance::Get_Instance()->Get_Transform_Matrix_Inverse(CPipeLine::D3DTS_PROJ);
	_matrix     InvViewMatrix = CGameInstance::Get_Instance()->Get_Transform_Matrix_Inverse(CPipeLine::D3DTS_VIEW);

	/* 뷰스페이스 상의 마우스 위치. */
	vecMouseNearPos = XMVector3TransformCoord(vecMouseNearPos, InvProjMatrix);

	_vector MousePos = XMVectorSet(0.f, 0.f, 0.f, 1.f);
	_vector MouseRayDir = vecMouseNearPos - MousePos;

	MousePos = XMVector4Transform(MousePos, InvViewMatrix);
	MouseRayDir = XMVector4Transform(MouseRayDir, InvViewMatrix);

	XMStoreFloat3(&m_vMouseRay, MouseRayDir);
	XMStoreFloat3(&m_vMouseRayPos, MousePos);
	//return;

	// 월드상에 있는 마우스 위치 나옴 

	/* 마우스 Ray, RayPos를 구한 이유는 삼각형과의 충돌을 위해 필요한 데이터이기때문에.  */
	/* D3DXIntersectTri(점1, 점2, 점3, RayPos, Ray); */
	//방향연산끝
	/* 월드 스페이스 상의 마우스 Ray, RayPos */
	/* 위치 : 로컬위치 * 월드 * 뷰 * 뷰의 역 */
	// 방향은 나중에 카톡으로 
	/*m_vMouseRayPos = _float3(0.f, 0.f, 0.f);
	m_vMouseRay = vMousePos - m_vMouseRayPos;*/

}

void CPicking::Transform_ToLocalSpace(const _float4x4& WorldMatrix)
{
	_matrix WorldMatrixInv = XMLoadFloat4x4(&WorldMatrix);
	WorldMatrixInv = XMMatrixInverse(nullptr, WorldMatrixInv); // 역변환해서
	// _XmLoad-- > _float3 을 _vector 바꾼 다음

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
