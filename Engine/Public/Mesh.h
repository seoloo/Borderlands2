#pragma once

/* 모델을 구성하는 하나의 파츠. */
/* 이 파츠는 정점과 인덱스버퍼로 구성되어있다. */

#include "Model.h"
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CMesh final : public CVIBuffer
{
protected:
	CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMesh(const CMesh& rhs);
	virtual ~CMesh() = default;

public:
	_uint Get_MaterialIndex() const {
		return m_iMaterialIndex;
	}

	void Get_BoneMatrices(_float4x4* pBoneMatrices, const CModel::BONES& Bones, _fmatrix PivotMatrix);

public:
	virtual HRESULT Initialize_Prototype(CModel::TYPE eModelType, const aiMesh* pAIMesh, class CModel* pModel, _fmatrix PivotMatrix);
	virtual HRESULT Initialize(void* pArg) override;

private:
	char						m_szName[MAX_PATH] = "";
	_uint						m_iMaterialIndex = { 0 };

	_uint						m_iNumBones = { 0 };
	vector<_uint>				m_BoneIndices; /* 모델이 가지고 있는 전체뼈의 인덱스 */
	vector<_float4x4>			m_OffsetMatrices;

private:
	HRESULT Ready_VertexBuffer_For_NonAnim(const aiMesh* pAIMesh, _fmatrix PivotMatrix);
	HRESULT Ready_VertexBuffer_For_Anim(const aiMesh* pAIMesh, class CModel* pModel);

public:
	static CMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel::TYPE eModelType, const aiMesh* pAIMesh, class CModel* pModel, _fmatrix PivotMatrix);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END