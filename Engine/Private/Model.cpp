#include "..\Public\Model.h"
#include "Mesh.h"
#include "Bone.h"
#include "Shader.h"
#include "Texture.h"
#include "Animation.h"


CModel::CModel(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext)
{

}

CModel::CModel(const CModel & rhs)
	: CComponent(rhs)
	, m_PivotMatrix(rhs.m_PivotMatrix)
	, m_iNumMeshes(rhs.m_iNumMeshes)
	, m_Meshes(rhs.m_Meshes)
	, m_iNumMaterials(rhs.m_iNumMaterials)
	, m_Materials(rhs.m_Materials)	
	, m_iNumAnimations(rhs.m_iNumAnimations)
	
{
	for (auto& pOriginalAnimation : rhs.m_Animations)
	{
		m_Animations.push_back(pOriginalAnimation->Clone());
	}

	for (auto& pOriginalBone : rhs.m_Bones)
	{
		m_Bones.push_back(pOriginalBone->Clone());
	}
	
	for (size_t i = 0; i < m_iNumMaterials; i++)
	{
		for (size_t j = 0; j < AI_TEXTURE_TYPE_MAX; j++)
		{
			Safe_AddRef(m_Materials[i].pMaterialTexture[j]);
		}		
	}

	for (size_t i = 0; i < m_iNumMeshes; i++)	
		Safe_AddRef(m_Meshes[i]);
	
}

_uint CModel::Get_BoneIndex(const char * pBoneName)
{
	_uint		iNumBones = m_Bones.size();
	size_t i = 0;
	for (; i < iNumBones; i++)
	{
		if (false == strcmp(m_Bones[i]->Get_Name(), pBoneName))
			break;
	}

	return i;

	//_uint		iIndex = 0;

	//auto	iter = find_if(m_Bones.begin(), m_Bones.end(), [&](CBone* pBone)->_bool
	//{		
	//	if (false == strcmp(pBone->Get_Name(), pBoneName))
	//	{
	//		return true;
	//	}

	//	++iIndex;
	//}); 

	//return iIndex;
}

const _float4x4* CModel::Get_BoneCombinedMatrix(const char * pBoneName)
{
	_float4x4	BoneMatrix;
	_uint		iNumBones = m_Bones.size();
	size_t i = 0;
	for (; i < iNumBones; i++)
	{
		if (false == strcmp(m_Bones[i]->Get_Name(), pBoneName))
		{			
			return m_Bones[i]->Get_CombinedTransformationFloat4x4();
		}			
	}
	return nullptr;
}

HRESULT CModel::Initialize_Prototype(TYPE eModelType, _fmatrix PivotMatrix, const char * pModelFilePath)
{
	/* m_Importer라는 객체를 통해서 파일의 정보를 읽어들여 m_pAIScene객체에 모든 데이터를 저장한다. */

	m_eType = eModelType;

	XMStoreFloat4x4(&m_PivotMatrix, PivotMatrix);

	_uint		iFlag = aiProcessPreset_TargetRealtime_Fast | aiProcess_ConvertToLeftHanded;

	if (TYPE_NONANIM == m_eType)
		iFlag |= aiProcess_PreTransformVertices;
	
	// aiProcess_GlobalScale
	/* aiProcess_PreTransformVertices : 모델안에 포함되어있는 뼈대의 정보를 메시에게 적용해준다. */

	m_pAIScene = m_Importer.ReadFile(pModelFilePath, iFlag);
	if (nullptr == m_pAIScene)
		return E_FAIL;

	/* m_pAIScene이라는 객체가 들고 있는 정보를 정리하여 내가 사용하기 좋게 만든다. */

	/* 3. 뼈를 로드하낟. (aiNode, aiBone, aiNodeAnim) */
	/* aiNode : 뼈의 상속관계를 표현한다. (자식과 부모의 정보, 행렬)*/
	/* aiBone : 메시기준으로 묶여있는 뼈의 정보. 이 뼈는 어떤 정점에게 영햐응ㄹ 주는가? 몇퍼나 영향르 주는가? */
	if (FAILED(Ready_Bones(m_pAIScene->mRootNode, -1)))
		return E_FAIL;


	/* 1. 메시의 정보를 생성한다. 
		메시 : 삼각형 집합. (정점버퍼, 인덱스버퍼를 통해서 만든다.)
		모델 : 메시의 집합. 추후 진행에서 메시의 파츠 교체가 용이하도록ㅎ하기위해서 .*/
	if (FAILED(Ready_Meshes(PivotMatrix)))
		return E_FAIL;

	/* 2. 메시의 머테리얼을 로드하낟.(텍스쳐를 로드한다.) */
	if (FAILED(Ready_Materials(pModelFilePath)))
		return E_FAIL;

	if (FAILED(Ready_Animations()))
		return E_FAIL;

	return S_OK;
}

HRESULT CModel::Initialize(void * pArg)
{
	return S_OK;
}

HRESULT CModel::Render(_uint iMeshIndex)
{
	if (nullptr != m_Meshes[iMeshIndex])
		m_Meshes[iMeshIndex]->Render();

	return S_OK;
}

HRESULT CModel::Bind_ShaderResourceView(CShader * pShader, const char * pConstantName, _uint iMeshIndex, aiTextureType eTextureType)
{
	if (iMeshIndex >= m_iNumMeshes)
		return E_FAIL;

	_uint		iMaterialIndex = m_Meshes[iMeshIndex]->Get_MaterialIndex();

	if (iMaterialIndex >= m_iNumMaterials)
		return E_FAIL;

	return m_Materials[iMaterialIndex].pMaterialTexture[eTextureType]->Bind_ShaderResourceView(pShader, pConstantName, 0);	
}

HRESULT CModel::Bind_BoneMatrices(CShader * pShader, const char * pContantName, _uint iMeshIndex)
{
	if (iMeshIndex >= m_iNumMeshes)
		return E_FAIL;

	_float4x4		BoneMatrices[256];

	m_Meshes[iMeshIndex]->Get_BoneMatrices(BoneMatrices, m_Bones, XMLoadFloat4x4(&m_PivotMatrix));

	return pShader->Bind_Matrices(pContantName, BoneMatrices, 256);	
}

void CModel::Play_Animation(_float fTimeDelta, _bool bDead)
{
	/* 현재 애니메이션이 사용하는 뼈들의 TransformationMatrix 를 갱신한다. */
	/* 1. 현재 애니메이션이 사용하는 뼈?! */
	/* 2. 뼈들의 상태는 어떤 값으로 갱신을해야하는지?! */

	/*if (!bDead)
		m_Animations[m_iCurrentAnimIndex]->Update_TransformationMatrices(fTimeDelta, m_Bones);
	else
		m_Animations[m_iCurrentAnimIndex]->Dead_TransformationMatrices(fTimeDelta, m_Bones);

	if (m_Animations[m_iCurrentAnimIndex]->Get_AnimEnd())
		m_bAnimEnd = m_Animations[m_iCurrentAnimIndex]->Get_AnimEnd();

	if (m_bAnimEnd)
		m_Animations[m_iCurrentAnimIndex]->Set_AnimEnd(m_bAnimEnd);*/

	m_Animations[m_iCurrentAnimIndex]->Update_TransformationMatrices(fTimeDelta, m_Bones);

	/* 특정 뼈들의 TransfokrmationMatrix가 변경되었기때문에. 부모로 부터 누적하여 만드는 CombinedTransfromationMatrix의 상태를 갱신해준다. */
	/* m_Bones에서 하나씩 꺼내서 CombinedTransformation을 갱신한다. */

	m_bAnimEnd = m_Animations[m_iCurrentAnimIndex]->Get_AnimEnd();

	for (auto& pBone : m_Bones)
	{
		pBone->Update_CombinedTransformationMatrix(m_Bones);
	}
}

void CModel::SetUp_Animation(_uint iIndex, _bool bContinue)
{
	if (iIndex >= m_iNumAnimations)
		return;

	if (bContinue)
	{
		m_iCurrentAnimIndex = iIndex;
		m_bFinished = bContinue;
	}
	else {
		if (m_bFinished && 0.f == m_Animations[m_iCurrentAnimIndex]->Get_TrackPosition())
		{
			m_iCurrentAnimIndex = iIndex;
			m_bFinished = false;
		}
		else if (!m_bFinished && 0.f != m_Animations[m_iCurrentAnimIndex]->Get_TrackPosition())
			m_iCurrentAnimIndex = iIndex;
	}
}

void CModel::Dead_Animation(_uint iIndex, _float fTimeDelta)
{
	if (iIndex >= m_iNumAnimations)
		return;

	m_iCurrentAnimIndex = iIndex;

	m_Animations[m_iCurrentAnimIndex]->Dead_TransformationMatrices(fTimeDelta, m_Bones);
}

void CModel::Stop_Animation(_float fTimeDelta, _bool bStop)
{
	m_Animations[m_iCurrentAnimIndex]->Update_TransformationMatrices(fTimeDelta, m_Bones, bStop);
}

HRESULT CModel::Ready_Meshes(_fmatrix PivotMatrix)
{
	if (nullptr == m_pAIScene)
		return E_FAIL;

	m_iNumMeshes = m_pAIScene->mNumMeshes;

	for (size_t i = 0; i < m_iNumMeshes; i++)
	{
		CMesh*	pMesh = CMesh::Create(m_pDevice, m_pContext, m_eType, m_pAIScene->mMeshes[i], this, PivotMatrix);
		if (nullptr == pMesh)
			return E_FAIL;

		m_Meshes.push_back(pMesh);
	}

	return S_OK;
}

HRESULT CModel::Ready_Materials(const char* pModelFilePath)
{
	m_iNumMaterials = m_pAIScene->mNumMaterials;

	for (size_t i = 0; i < m_iNumMaterials; i++)
	{
		MODEL_MATERIAL		ModelMaterial;
		ZeroMemory(&ModelMaterial, sizeof ModelMaterial);

		aiMaterial*	pAIMaterial = m_pAIScene->mMaterials[i];

		for (size_t j = 0; j < AI_TEXTURE_TYPE_MAX; j++)
		{
			aiString	strPath;

			if (FAILED(pAIMaterial->GetTexture(aiTextureType(j), 0, &strPath)))
				continue;

			char		szDrive[MAX_PATH] = "";
			char		szDirectory[MAX_PATH] = "";
			char		szFileName[MAX_PATH] = "";
			char		szExt[MAX_PATH] = "";
			char		szFullPath[MAX_PATH] = "";
			_tchar		szFinalPath[MAX_PATH] = L"";

			/* j번째(Diffuse, Specular, Normals ... ) 타입의 텍스쳐가 존재한다. */
			_splitpath_s(pModelFilePath, szDrive, MAX_PATH, szDirectory, MAX_PATH, nullptr, 0, nullptr, 0);
			_splitpath_s(strPath.data, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);
			
			strcpy_s(szFullPath, szDrive);
			strcat_s(szFullPath, szDirectory);
			strcat_s(szFullPath, szFileName);
			strcat_s(szFullPath, szExt);

			MultiByteToWideChar(CP_ACP, 0, szFullPath, strlen(szFullPath),
				szFinalPath, MAX_PATH);

			ModelMaterial.pMaterialTexture[j] = CTexture::Create(m_pDevice, m_pContext, szFinalPath);
			if (nullptr == ModelMaterial.pMaterialTexture[j])
				return E_FAIL;			
		}

		m_Materials.push_back(ModelMaterial);				
	}

	return S_OK;
}

HRESULT CModel::Ready_Bones(const aiNode * pAINode, _int iParentIndex)
{
	CBone*			pBone = CBone::Create(pAINode, iParentIndex);
	if (nullptr == pBone)
		return E_FAIL;

	m_Bones.push_back(pBone);	

	for (size_t i = 0; i < pAINode->mNumChildren; ++i)
	{
		if (FAILED(Ready_Bones(pAINode->mChildren[i], Get_BoneIndex(pBone->Get_Name()))))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CModel::Ready_Animations()
{
	m_iNumAnimations = m_pAIScene->mNumAnimations;

	for (size_t i = 0; i < m_iNumAnimations; ++i)
	{
		CAnimation*		pAnimation = CAnimation::Create(m_pAIScene->mAnimations[i], this);
		if (nullptr == pAnimation)
			return E_FAIL;

		m_Animations.push_back(pAnimation);
	}	

	return S_OK;
}

_bool CModel::Intersect_Polygons(const _float4x4& WorldMatrix, _float3* pOut)
{
	for (auto it = m_Meshes.begin(); it != m_Meshes.end(); ++it)
	{
		return (*it)->Intersect_Polygons(WorldMatrix, pOut);
	}
}

CModel * CModel::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, TYPE eModelType, const char * pModelFilePath, _fmatrix PivotMatrix)
{
	CModel*	pInstance = new CModel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eModelType, PivotMatrix, pModelFilePath)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Created : CModel");
	}

	return pInstance;
}

CComponent * CModel::Clone(void * pArg)
{
	CModel*	pInstance = new CModel(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Cloned : CModel");
	}

	return pInstance;
}

void CModel::Free()
{
	__super::Free();

	for (auto& pAnimation : m_Animations)
		Safe_Release(pAnimation);

	m_Animations.clear();

	for (auto& pBone : m_Bones)
		Safe_Release(pBone);

	m_Bones.clear();

	for (size_t i = 0; i < m_iNumMaterials; i++)
	{
		for (size_t j = 0; j < AI_TEXTURE_TYPE_MAX; j++)
		{
			Safe_Release(m_Materials[i].pMaterialTexture[j]);
		}
	}

	for (size_t i = 0; i < m_iNumMeshes; i++)
		Safe_Release(m_Meshes[i]);
}
