#include "..\Public\Animation.h"
#include "Channel.h"

CAnimation::CAnimation()
{

}

CAnimation::CAnimation(const CAnimation & rhs)
	: m_fDuration(rhs.m_fDuration)
	, m_fTickPerSecond(rhs.m_fTickPerSecond)
	, m_fTrackPosition(rhs.m_fTrackPosition)
	, m_Channels(rhs.m_Channels)
	, m_CurrentKeyFrames(rhs.m_CurrentKeyFrames)
{
	for (auto& pChannel : m_Channels)
		Safe_AddRef(pChannel);
}

HRESULT CAnimation::Initialize(const aiAnimation * pAIAnimation, CModel* pModel)
{
	m_fDuration = (_float)pAIAnimation->mDuration;
	m_fTickPerSecond = (_float)pAIAnimation->mTicksPerSecond;

	/* 이 애니메이션은 몇개의 뼈를 구동하여 표현하는가 .*/
	m_iNumChannels = pAIAnimation->mNumChannels;
	m_CurrentKeyFrames.resize(m_iNumChannels);

	for (size_t i = 0; i < m_iNumChannels; i++)
	{
		CChannel*		pChannel = CChannel::Create(pAIAnimation->mChannels[i], pModel);
		if (nullptr == pChannel)
			return E_FAIL;

		m_Channels.push_back(pChannel);
	}

	return S_OK;
}

HRESULT CAnimation::Update_TransformationMatrices(_float fTimeDelta, const CModel::BONES& Bones, _bool bReset)
{
	/* 현재 애니메이션이 재생해야할 위치를 구한다. */
	m_fTrackPosition += m_fTickPerSecond * fTimeDelta;

	if (bReset)
		m_fTrackPosition = 0.f;

	if (m_fTrackPosition >= m_fDuration)
	{
		m_fTrackPosition = 0.f;
		m_bAnimEnd = true;
	}

	_uint		iIndex = 0;

	/* 이 애니메이션에서 사용하는 모든 뼈들을 순회하면서 시간에 맞는 상태를 가질 수 있도록 한다. */

	for (auto& pChannel : m_Channels)
	{
		pChannel->Update_TransformationMatrices(&m_CurrentKeyFrames[iIndex++], m_fTrackPosition, Bones);
	}

	return S_OK;
}

HRESULT CAnimation::Dead_TransformationMatrices(_float fTimeDelta, const CModel::BONES& Bones)
{
	/* 현재 애니메이션이 재생해야할 위치를 구한다. */
	m_fTrackPosition += m_fTickPerSecond * fTimeDelta;

	_uint		iIndex = 0;

	/* 이 애니메이션에서 사용하는 모든 뼈들을 순회하면서 시간에 맞는 상태를 가질 수 있도록 한다. */
	for (auto& pChannel : m_Channels)
	{
		pChannel->Update_TransformationMatrices(&m_CurrentKeyFrames[iIndex++], m_fTrackPosition, Bones);
	}

	return S_OK;
}

CAnimation * CAnimation::Create(const aiAnimation * pAIAnimation, class CModel* pModel)
{
	CAnimation*	pInstance = new CAnimation();

	if (FAILED(pInstance->Initialize(pAIAnimation, pModel)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Created : CAnimation");
	}

	return pInstance;
}

CAnimation * CAnimation::Clone()
{
	return new CAnimation(*this);	
}

void CAnimation::Free()
{
	for (auto& pChannel : m_Channels)
		Safe_Release(pChannel);

	m_Channels.clear();
}
