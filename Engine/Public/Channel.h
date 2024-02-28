#pragma once

/* 특정 애니메이션을 구동하기위한 뼈들 중 하나의 뼈.  */
#include "Model.h"

BEGIN(Engine)

class CChannel final : public CBase
{
private:
	CChannel();
	virtual ~CChannel() = default;

public:
	HRESULT Initialize(const aiNodeAnim* pAIChannel, class CModel* pModel);
	void Update_TransformationMatrices(_uint* pCurrentKeyFrameIndex, _float fTrackPosition, const CModel::BONES& Bones);

private:
	_uint					m_iNumKeyFrames = { 0 };
	vector<KEYFRAME>		m_KeyFrames;

	

	/* 채널이 생성될 때 이름이 같은 뼈의 인덱스를 들고 있게 하겠다. */
	_uint					m_iBoneIndex;


public:
	static CChannel* Create(const aiNodeAnim* pAIChannel, CModel* pModel);
	virtual void Free() override;
};

END