#include "..\Public\Navigation.h"
#include "Cell.h"

#ifdef _DEBUG
#include "Shader.h"
#include "PipeLine.h"
#endif

CNavigation::CNavigation(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext)
{
}

CNavigation::CNavigation(const CNavigation & rhs)
	: CComponent(rhs)
	, m_Cells(rhs.m_Cells)
#ifdef _DEBUG
	, m_pShader(rhs.m_pShader)
#endif
{
	for (auto& pCell : m_Cells)
		Safe_AddRef(pCell);


#ifdef _DEBUG
	Safe_AddRef(m_pShader);
#endif
}

HRESULT CNavigation::Initialize_Prototype(const _tchar * pNavigationDataFiles, _int iLevel)
{
	_ulong		dwByte = 0;
	HANDLE		hFile = nullptr;

	if(iLevel == 3)
		hFile = CreateFile(TEXT("../../Client/Bin/Resources/Map/NaviTownTest.txt"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	else if(iLevel == 4)
		hFile = CreateFile(TEXT("../../Client/Bin/Resources/Map/NaviTest.txt"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	else if (iLevel == 5)
		hFile = CreateFile(TEXT("../../Client/Bin/Resources/Map/NaviEnding.txt"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	size_t		dataLength;
	wstring		Name;
	_float3		Pos[3];

	if (0 == hFile)
		return E_FAIL;

	while (true)
	{
		if (!ReadFile(hFile, &dataLength, sizeof(size_t), &dwByte, nullptr))
			break;

		if (dataLength == 0)
			break;

		if (0 == dwByte)
			break;

		Name.resize(dataLength);
		if (!ReadFile(hFile, &Name[0], sizeof(wchar_t) * dataLength, &dwByte, nullptr))
			break;

		if (!ReadFile(hFile, &Pos[0], sizeof(_float3), &dwByte, nullptr))
			break;

		if (!ReadFile(hFile, &Pos[1], sizeof(_float3), &dwByte, nullptr))
			break;

		if (!ReadFile(hFile, &Pos[2], sizeof(_float3), &dwByte, nullptr))
			break;


		if (0 == dwByte)
			break;

		CCell*		pCell = CCell::Create(m_pDevice, m_pContext, Pos, m_Cells.size());
		if (nullptr == pCell)
			return E_FAIL;

		m_Cells.push_back(pCell);
	}

#ifdef _DEBUG
	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Navigation.hlsl"), VTXPOS::Elements, VTXPOS::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;
#endif

	if (FAILED(SetUp_Neighbors()))
		return E_FAIL;
	return S_OK;
}

HRESULT CNavigation::Initialize(void * pArg)
{
	NAVIDESC* pNaviDesc = (NAVIDESC*)pArg;

	m_pWorldMatrix = pNaviDesc->pWorldMatrix;
	m_iCurrentIndex = pNaviDesc->iCurrentIndex;

	return S_OK;
}

_bool CNavigation::is_Move(_fvector vPosition)
{
	_int		iNeighborIndex = { -1 };

	if (true == m_Cells[m_iCurrentIndex]->is_In(vPosition, &iNeighborIndex))
	{
		return true;
	}
	else /* ÇöÀç ¼¿À» ¹þ¾î³ª³®. */
	{
		/* ÀÌ¿ôÀÌ ÀÖ¾ú´Ù. */
		if (-1 != iNeighborIndex)
		{
			while (true)
			{
				if (true == m_Cells[iNeighborIndex]->is_In(vPosition, &iNeighborIndex))
					break;

				if (-1 == iNeighborIndex)
					return false;
			}
			m_iCurrentIndex = iNeighborIndex;
			return true;
		}
		/* ÀÌ¿ôÀÌ ¾ø¾ú´Ù. */
		else
		{
			m_vFirstPos = m_Cells[m_iCurrentIndex]->Get_FirstPos();
			m_vSecondPos = m_Cells[m_iCurrentIndex]->Get_SecondPos();
			return false;
		}
	}
}

#ifdef _DEBUG
HRESULT CNavigation::Render()
{
	if (nullptr == m_pWorldMatrix)
		return E_FAIL;

	CPipeLine* pPipeLine = CPipeLine::Get_Instance();
	Safe_AddRef(pPipeLine);

	m_pShader->Bind_Matrix("g_ViewMatrix", &pPipeLine->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW));
	m_pShader->Bind_Matrix("g_ProjMatrix", &pPipeLine->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ));

	Safe_Release(pPipeLine);

	_float4 colorGreen(0.f, 1.f, 0.f, 1.f);
	_float4 colorRed(1.f, 0.f, 0.f, 1.f);

	if (-1 == m_iCurrentIndex)
	{
		if (FAILED(m_pShader->Bind_RawValue("g_vColor", &colorGreen, sizeof(_float4))))
			return E_FAIL;
		 
		m_pShader->Bind_Matrix("g_WorldMatrix", m_pWorldMatrix);

		m_pShader->Begin(0);

		for (auto& pCell : m_Cells)
		{
			if (nullptr != pCell)
				pCell->Render();
		}
	}
	else
	{
		if (FAILED(m_pShader->Bind_RawValue("g_vColor", &colorRed, sizeof(_float4))))
			return E_FAIL;

		_float4x4	WorldMatrix = *m_pWorldMatrix;

		WorldMatrix._42 += 0.1f;

		m_pShader->Bind_Matrix("g_WorldMatrix", &WorldMatrix);

		m_pShader->Begin(0);

 		m_Cells[m_iCurrentIndex]->Render();
	}

	return S_OK;
}
#endif

HRESULT CNavigation::SetUp_Neighbors()
{
	for (auto& pSourCell : m_Cells)
	{
		for (auto& pDestCell : m_Cells)
		{
			if (pSourCell == pDestCell)
				continue;

			if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_A), pSourCell->Get_Point(CCell::POINT_B)))
			{
				pSourCell->SetUp_Neighbor(CCell::LINE_AB, pDestCell);
			}

			else if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_B), pSourCell->Get_Point(CCell::POINT_C)))
			{
				pSourCell->SetUp_Neighbor(CCell::LINE_BC, pDestCell);
			}

			else if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_C), pSourCell->Get_Point(CCell::POINT_A)))
			{
				pSourCell->SetUp_Neighbor(CCell::LINE_CA, pDestCell);
			}
		}
	}


	return S_OK;
}

CNavigation * CNavigation::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar * pNavigationDataFiles, _int iLevel)
{
	CNavigation*	pInstance = new CNavigation(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pNavigationDataFiles, iLevel)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Created : CNavigation");
	}

	return pInstance;
}

CComponent * CNavigation::Clone(void * pArg)
{
	CNavigation*	pInstance = new CNavigation(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Cloned : CNavigation");
	}

	return pInstance;
}

void CNavigation::Free()
{
	__super::Free();

	for (auto& pCell : m_Cells)
		Safe_Release(pCell);

	m_Cells.clear();

#ifdef _DEBUG
	Safe_Release(m_pShader);
#endif
}
