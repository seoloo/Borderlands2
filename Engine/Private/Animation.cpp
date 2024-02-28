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

	/* �� �ִϸ��̼��� ��� ���� �����Ͽ� ǥ���ϴ°� .*/
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
	/* ���� �ִϸ��̼��� ����ؾ��� ��ġ�� ���Ѵ�. */
	m_fTrackPosition += m_fTickPerSecond * fTimeDelta;

	if (bReset)
		m_fTrackPosition = 0.f;

	if (m_fTrackPosition >= m_fDuration)
	{
		m_fTrackPosition = 0.f;
		m_bAnimEnd = true;
	}

	_uint		iIndex = 0;

	/* �� �ִϸ��̼ǿ��� ����ϴ� ��� ������ ��ȸ�ϸ鼭 �ð��� �´� ���¸� ���� �� �ֵ��� �Ѵ�. */

	for (auto& pChannel : m_Channels)
	{
		pChannel->Update_TransformationMatrices(&m_CurrentKeyFrames[iIndex++], m_fTrackPosition, Bones);
	}

	return S_OK;
}

HRESULT CAnimation::Dead_TransformationMatrices(_float fTimeDelta, const CModel::BONES& Bones)
{
	/* ���� �ִϸ��̼��� ����ؾ��� ��ġ�� ���Ѵ�. */
	m_fTrackPosition += m_fTickPerSecond * fTimeDelta;

	_uint		iIndex = 0;

	/* �� �ִϸ��̼ǿ��� ����ϴ� ��� ������ ��ȸ�ϸ鼭 �ð��� �´� ���¸� ���� �� �ֵ��� �Ѵ�. */
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
