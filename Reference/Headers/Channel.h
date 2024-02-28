#pragma once

/* Ư�� �ִϸ��̼��� �����ϱ����� ���� �� �ϳ��� ��.  */
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

	

	/* ä���� ������ �� �̸��� ���� ���� �ε����� ��� �ְ� �ϰڴ�. */
	_uint					m_iBoneIndex;


public:
	static CChannel* Create(const aiNodeAnim* pAIChannel, CModel* pModel);
	virtual void Free() override;
};

END