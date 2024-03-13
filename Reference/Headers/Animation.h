#pragma once

#include "Model.h"

/* 1. Ư�� �ִϸ��̼� ������ ó������ ������ �����ϱ����� ������ ������ �ִ´�. */
/* 2. �����ϱ����� ���� : */
/* 2-1. �� �ִϸ��̼��� ǥ���ϱ����� ������ ����. ������ ����. �� : ä�� */
/* 2-2. �ִϸ��̼��� �ӵ�(TickPerSecond), �ִϸ��̼��� ��ü �����ϴµ� �ɸ��� �ð�(Duration) */

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
	/* �ִϸ��̼��� �����ϱ����� ���� ���� .*/
	_uint						m_iNumChannels = { 0 };
	/* �ִϸ��̼��� �����ϱ����� ���� .*/
	vector<class CChannel*>		m_Channels;
	vector<_uint>				m_CurrentKeyFrames;
	vector<_uint>				m_NextAnimation;
public:
	static CAnimation* Create(const aiAnimation* pAIAnimation, class CModel* pModel);
	CAnimation* Clone();
	virtual void Free() override;
};

END