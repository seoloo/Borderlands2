#include "..\Public\Channel.h"
#include "Bone.h"

CChannel::CChannel()
{

}

HRESULT CChannel::Initialize(const aiNodeAnim * pAIChannel, CModel* pModel)
{
	/* �� ä���� �ǹ��ϴ� ���� �̸�. */
	m_iBoneIndex = pModel->Get_BoneIndex(pAIChannel->mNodeName.data);

	/* �� ���� �� �ִϸ��̼��� �����ϱ����� ��� ���¸� ������ �ִ°�?! */
	m_iNumKeyFrames = max(pAIChannel->mNumScalingKeys, pAIChannel->mNumRotationKeys);
	m_iNumKeyFrames = max(m_iNumKeyFrames, pAIChannel->mNumPositionKeys);

	_float3			vScale;
	_float4			vRotation;
	_float3			vPosition;

	for (size_t i = 0; i < m_iNumKeyFrames; i++)
	{
		KEYFRAME		KeyFrame;
		ZeroMemory(&KeyFrame, sizeof KeyFrame);

		if (i < pAIChannel->mNumScalingKeys)		
		{
			memcpy(&vScale, &pAIChannel->mScalingKeys[i].mValue, sizeof(_float3));
			KeyFrame.fTime = pAIChannel->mScalingKeys[i].mTime;
		}

		if (i < pAIChannel->mNumRotationKeys)
		{
			vRotation.x = pAIChannel->mRotationKeys[i].mValue.x;
			vRotation.y = pAIChannel->mRotationKeys[i].mValue.y;
			vRotation.z = pAIChannel->mRotationKeys[i].mValue.z;
			vRotation.w = pAIChannel->mRotationKeys[i].mValue.w;			
			KeyFrame.fTime = pAIChannel->mRotationKeys[i].mTime;
		}

		if (i < pAIChannel->mNumPositionKeys)
		{
			memcpy(&vPosition, &pAIChannel->mPositionKeys[i].mValue, sizeof(_float3));
			KeyFrame.fTime = pAIChannel->mPositionKeys[i].mTime;
		}

		memcpy(&KeyFrame.vScale, &vScale, sizeof(_float3));
		memcpy(&KeyFrame.vRotation, &vRotation, sizeof(_float4));
		memcpy(&KeyFrame.vTranslation, &vPosition, sizeof(_float3));
		KeyFrame.vTranslation.w = 1.f;

		m_KeyFrames.push_back(KeyFrame);
	}

	return S_OK;
}

void CChannel::Update_TransformationMatrices(_uint* pCurrentKeyFrameIndex, _float fTrackPosition, const CModel::BONES& Bones)
{
	if (0.0f == fTrackPosition)
		(*pCurrentKeyFrameIndex) = 0;

	/* ���� �ð��� �´� �� ��(Channel)�� ��������� �����. */
	_vector			vScale;
	_vector			vRotation;
	_vector			vPosition;

	/* ���� �ð��� ������ ������ �ʾ����� ������ Ű������ �ð��� �Ѿ��. ������ Ű�������� ���¸� ���Ѵ�.
	�����ִ� �κ��� �ִٸ� �� �κ��� �����ǵ���*/
	if (fTrackPosition >= m_KeyFrames.back().fTime)
	{
		vScale = XMLoadFloat4(&m_KeyFrames.back().vScale);
		vRotation = XMLoadFloat4(&m_KeyFrames.back().vRotation);
		vPosition = XMLoadFloat4(&m_KeyFrames.back().vTranslation);
	}

	else /* Ư�� Ű�����Ӱ� Ű������ ���̿� �־��ٸ�. */
	{
		while (fTrackPosition > m_KeyFrames[(*pCurrentKeyFrameIndex) + 1].fTime)
			++(*pCurrentKeyFrameIndex);

		_float		fRatio = (fTrackPosition - m_KeyFrames[(*pCurrentKeyFrameIndex)].fTime) /
			(m_KeyFrames[(*pCurrentKeyFrameIndex) + 1].fTime - m_KeyFrames[(*pCurrentKeyFrameIndex)].fTime);

		_vector		vSourScale, vDestScale;
		_vector		vSourRotation, vDestRotation;
		_vector		vSourTranslation, vDestTranslation;

		vSourScale = XMLoadFloat4(&m_KeyFrames[(*pCurrentKeyFrameIndex)].vScale);
		vSourRotation = XMLoadFloat4(&m_KeyFrames[(*pCurrentKeyFrameIndex)].vRotation);
		vSourTranslation = XMLoadFloat4(&m_KeyFrames[(*pCurrentKeyFrameIndex)].vTranslation);

		vDestScale = XMLoadFloat4(&m_KeyFrames[(*pCurrentKeyFrameIndex) + 1].vScale);
		vDestRotation = XMLoadFloat4(&m_KeyFrames[(*pCurrentKeyFrameIndex) + 1].vRotation);
		vDestTranslation = XMLoadFloat4(&m_KeyFrames[(*pCurrentKeyFrameIndex) + 1].vTranslation);

		vScale = XMVectorLerp(vSourScale, vDestScale, fRatio);
		vRotation = XMQuaternionSlerp(vSourRotation, vDestRotation, fRatio);
		vPosition = XMVectorLerp(vSourTranslation, vDestTranslation, fRatio);

		if (fRatio >= 1.0f)
		{
			vScale = XMLoadFloat4(&m_KeyFrames.back().vScale);
			vRotation = XMLoadFloat4(&m_KeyFrames.back().vRotation);
			vPosition = XMLoadFloat4(&m_KeyFrames.back().vTranslation);
		}
	}

	_matrix			TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);

	Bones[m_iBoneIndex]->Set_TransformationMatrix(TransformationMatrix);
}

CChannel * CChannel::Create(const aiNodeAnim * pAIChannel, CModel* pModel)
{
	CChannel*	pInstance = new CChannel();

	if (FAILED(pInstance->Initialize(pAIChannel, pModel)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Created : CChannel");
	}

	return pInstance;
}

void CChannel::Free()
{
	m_KeyFrames.clear();
}
