#include "..\Public\Mesh.h"
#include "Bone.h"

CMesh::CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CMesh::CMesh(const CMesh& rhs)
	: CVIBuffer(rhs)
{
}

void CMesh::Get_BoneMatrices(_float4x4* pBoneMatrices, const CModel::BONES& Bones, _fmatrix PivotMatrix)
{
	_uint		iIndex = 0;

	/* 이 메시에 영향을 주는 ㅃㅕ의 갯수만큼 순회하자. */
	for (auto& iBoneIndex : m_BoneIndices)
	{
		XMStoreFloat4x4(&pBoneMatrices[iIndex],
			XMLoadFloat4x4(&m_OffsetMatrices[iIndex]) * Bones[iBoneIndex]->Get_CombinedTransformationMatrix() * PivotMatrix);

		++iIndex;
	}

}

HRESULT CMesh::Initialize_Prototype(CModel::TYPE eModelType, const aiMesh* pAIMesh, CModel* pModel, _fmatrix PivotMatrix)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	strcpy_s(m_szName, pAIMesh->mName.data);

	m_iMaterialIndex = pAIMesh->mMaterialIndex;
	m_iNumVertexBuffers = 1;
	m_iNumVertices = pAIMesh->mNumVertices;
	m_iNumIndices = pAIMesh->mNumFaces * 3;
	m_iIndexStride = sizeof(_ulong);
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_iNumPrimitive = pAIMesh->mNumFaces;

#pragma region VERTEX_BUFFER

	HRESULT		hr = eModelType == CModel::TYPE_NONANIM ?
		Ready_VertexBuffer_For_NonAnim(pAIMesh, PivotMatrix) : Ready_VertexBuffer_For_Anim(pAIMesh, pModel);
	if (FAILED(hr))
		return E_FAIL;


#pragma endregion

#pragma region INDEX_BUFFER
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iNumIndices * m_iIndexStride;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = 0;

	_ulong* pIndices = new _ulong[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_ulong) * m_iNumIndices);

	_uint		iIndex = { 0 };

	for (size_t i = 0; i < pAIMesh->mNumFaces; i++)
	{
		pIndices[iIndex++] = pAIMesh->mFaces[i].mIndices[0];
		pIndices[iIndex++] = pAIMesh->mFaces[i].mIndices[1];
		pIndices[iIndex++] = pAIMesh->mFaces[i].mIndices[2];
	}

	ZeroMemory(&m_InitializeData, sizeof m_InitializeData);
	m_InitializeData.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	m_pIndices = pIndices;

	//Safe_Delete_Array(pIndices);

#pragma endregion

	return S_OK;
}

HRESULT CMesh::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CMesh::Ready_VertexBuffer_For_NonAnim(const aiMesh* pAIMesh, _fmatrix PivotMatrix)
{
	m_iStride = sizeof(VTXMESH);

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iNumVertices * m_iStride;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iStride;


	VTXMESH* pVertices = new VTXMESH[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXMESH) * m_iNumVertices);

	m_pVerticesPos = new _float3[m_iNumVertices];
	ZeroMemory(m_pVerticesPos, sizeof(m_iNumVertices));

	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		memcpy(&pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vPosition,
			XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vPosition), PivotMatrix));

		memcpy(&pVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vNormal,
			XMVector3Normalize(XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vNormal), PivotMatrix)));

		memcpy(&pVertices[i].vTexcoord, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));
		memcpy(&pVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));

		XMStoreFloat3(&pVertices[i].vTangent,
			XMVector3Normalize(XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vTangent), PivotMatrix)));
		
		m_pVerticesPos[i] = pVertices[i].vPosition;
		int a = 0;
	}

	ZeroMemory(&m_InitializeData, sizeof m_InitializeData);
	m_InitializeData.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	return S_OK;
}

HRESULT CMesh::Ready_VertexBuffer_For_Anim(const aiMesh* pAIMesh, CModel* pModel)
{
	m_iStride = sizeof(VTXANIMMESH);

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iNumVertices * m_iStride;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iStride;

	VTXANIMMESH* pVertices = new VTXANIMMESH[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXANIMMESH) * m_iNumVertices);

	m_pVerticesPos = new _float3[m_iNumVertices];
	ZeroMemory(m_pVerticesPos, sizeof(m_iNumVertices));

	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		memcpy(&pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
		memcpy(&pVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
		memcpy(&pVertices[i].vTexcoord, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));
		memcpy(&pVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));

		memcpy(&m_pVerticesPos[i], &pAIMesh->mVertices[i], sizeof(_float3));

		//m_pVerticesPos[i] = pVertices[i].vPosition;
	}

	/* 모델 = 메시의 집합.  */
	/* 메시가 사용하는 뼈의 갯수. */
	m_iNumBones = pAIMesh->mNumBones;

	/* 이 메시에게 영향을 주는 뼈들을 순회한다. */
	for (size_t i = 0; i < m_iNumBones; ++i)
	{
		/* 각 뼈정보에 접근한다. */
		aiBone* pAIBone = pAIMesh->mBones[i];

		_float4x4		OffsetMatrix;

		memcpy(&OffsetMatrix, &pAIBone->mOffsetMatrix, sizeof(_float4x4));
		XMStoreFloat4x4(&OffsetMatrix, XMMatrixTranspose(XMLoadFloat4x4(&OffsetMatrix)));

		m_OffsetMatrices.push_back(OffsetMatrix);

		m_BoneIndices.push_back(pModel->Get_BoneIndex(pAIBone->mName.data));

		/* pAIBone->mNumWeights : 특정 뼈는 몇개의 정점에게 영향을 주는가? */
		for (size_t j = 0; j < pAIBone->mNumWeights; ++j)
		{
			if (0.0f == pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.x)
			{
				/* pVertices[pAIBone->mWeights[j].mVertexId].vBlendIndices */
				/* 이 정점이 사용하고 있는 뼈(모델전체의뼈(x), 이 메시에게영향을 주는 뼈(o))의 인덱스다. */
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendIndices.x = i;
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.x = pAIBone->mWeights[j].mWeight;
			}
			else if (0.0f == pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.y)
			{
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendIndices.y = i;
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.y = pAIBone->mWeights[j].mWeight;
			}
			else if (0.0f == pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.z)
			{
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendIndices.z = i;
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.z = pAIBone->mWeights[j].mWeight;
			}
			else
			{
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendIndices.w = i;
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.w = pAIBone->mWeights[j].mWeight;
			}
		}
	}

	if (0 == m_iNumBones)
	{
		m_iNumBones = 1;

		_float4x4		OffsetMatrix;

		XMStoreFloat4x4(&OffsetMatrix, XMMatrixIdentity());

		m_OffsetMatrices.push_back(OffsetMatrix);

		m_BoneIndices.push_back(pModel->Get_BoneIndex(m_szName));
	}

	ZeroMemory(&m_InitializeData, sizeof m_InitializeData);
	m_InitializeData.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	return S_OK;
}

CMesh* CMesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel::TYPE eModelType, const aiMesh* pAIMesh, CModel* pModel, _fmatrix PivotMatrix)
{
	CMesh* pInstance = new CMesh(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eModelType, pAIMesh, pModel, PivotMatrix)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Created : CMesh");
	}

	return pInstance;
}

CComponent* CMesh::Clone(void* pArg)
{
	CMesh* pInstance = new CMesh(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Cloned : CMesh");
	}

	return pInstance;
}

void CMesh::Free()
{
	__super::Free();

}
