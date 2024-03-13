#pragma once

#include "Model.h"

/* 1. 특정 애니메이션 동작을 처음부터 끝까지 진행하기위한 정보를 가지고 있는다. */
/* 2. 진행하기위한 정보 : */
/* 2-1. 이 애니메이션을 표현하기위한 뼈들의 종류. 뼈들의 상태. 뼈 : 채널 */
/* 2-2. 애니메이션의 속도(TickPerSecond), 애니메이션을 전체 구동하는데 걸리는 시간(Duration) */

BEGIN(Engine)

class ENGINE_DLL CAnimation final : public CBase
{
private:
	CAnimation();
	CAnimation(const CAnimation& rhs);
	virtual ~CAnimation() = default;

public:
	HRESULT Initialize(const aiAnimation* pAIAnimation, class CModel* pModel);
	HRESULT Update_TransformationMatrices(_float fTimeDelta, const CModel::BONES& Bones, _bool bReset = 0);
	HRESULT Dead_TransformationMatrices(_float fTimeDelta, const CModel::BONES& Bones);

public:
	_float	Get_TrackPosition() { return m_fTrackPosition; };
	_bool	Get_AnimEnd() { return m_bAnimEnd; };
	void	Set_AnimEnd(_bool bEnd) { m_bAnimEnd = bEnd; };
private:
	_float				m_fDuration = { 0.0f };
	_float				m_fTickPerSecond = { 0.0f };
	_float				m_fTrackPosition = { 0.0f };

	_bool				m_bAnimEnd = { false };

private:
	/* 애니메이션을 구동하기위한 뼈의 갯수 .*/
	_uint						m_iNumChannels = { 0 };
	/* 애니메이션을 구동하기위한 뼈들 .*/
	vector<class CChannel*>		m_Channels;
	vector<_uint>				m_CurrentKeyFrames;
	vector<_uint>				m_NextAnimation;
public:
	static CAnimation* Create(const aiAnimation* pAIAnimation, class CModel* pModel);
	CAnimation* Clone();
	virtual void Free() override;
};

END