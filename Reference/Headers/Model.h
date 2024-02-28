#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CModel final : public CComponent
{
public:
	enum TYPE { TYPE_NONANIM, TYPE_ANIM, TYPE_END };
private:
	CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CModel(const CModel& rhs);
	virtual ~CModel() = default;

public:
	_uint Get_NumMeshes() const {
		return m_iNumMeshes;
	}

	_uint Get_BoneIndex(const char* pBoneName);

	const _float4x4* Get_BoneCombinedMatrix(const char* pBoneName);
	_float4x4 Get_PivotMatrix() const {
		return m_PivotMatrix;
	}

	vector<class CAnimation*> Get_Animation() { return m_Animations; };

	_bool Get_AnimEnd() { return m_bAnimEnd; };
	void Set_AnimEnd(_bool bAnimEnd) { m_bAnimEnd = bAnimEnd; };

public:
	virtual HRESULT Initialize_Prototype(TYPE eModelType, _fmatrix PivotMatrix, const char* pModelFilePath);
	virtual HRESULT Initialize(void* pArg);
	HRESULT Render(_uint iMeshIndex);

public:
	HRESULT Bind_ShaderResourceView(class CShader* pShader, const char* pConstantName, _uint iMeshIndex, aiTextureType eTextureType);
	HRESULT Bind_BoneMatrices(class CShader* pShader, const char* pContantName, _uint iMeshIndex);
	void Play_Animation(_float fTimeDelta, _bool bDead = 0);
	void SetUp_Animation(_uint iIndex, _bool bContinue = 0);
	void Dead_Animation(_uint iIndex, _float fTimeDelta);
	void Stop_Animation(_float fTimeDelta, _bool bStop);
	 
private:
	const aiScene* m_pAIScene = { nullptr };
	Assimp::Importer		m_Importer;

private:
	TYPE					m_eType = { TYPE_END };
	_float4x4				m_PivotMatrix;
	_uint					m_iNumMeshes = { 0 };
	vector<class CMesh*>	m_Meshes;
	_bool		m_bAnimEnd = { false };
	_bool		m_bFinished = { false };
private:
	_uint					m_iNumMaterials = { 0 };
	vector<MODEL_MATERIAL>	m_Materials;

private:
	vector<class CBone*>			m_Bones;
public:
	typedef vector<class CBone*>	BONES;

private:
	_uint							m_iCurrentAnimIndex = 0;
	_uint							m_iPreviousAnimIndex = 0;
	_uint							m_iNumAnimations = 0;
	vector<class CAnimation*>		m_Animations;

private:
	HRESULT Ready_Meshes(_fmatrix PivotMatrix);
	HRESULT Ready_Materials(const char* pModelFilePath);
	HRESULT Ready_Bones(const aiNode* pAINode, _int iParentIndex);
	HRESULT Ready_Animations();

public:
	_bool Intersect_Polygons(const _float4x4& WorldMatrix, _float3* pOut);

public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eModelType, const char* pModelFilePath, _fmatrix PivotMatrix = XMMatrixIdentity());
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END