#pragma once
#include "SpiderAnt_State.h"

BEGIN(Engine)
class CTransform;
class CModel;
END

BEGIN(Client)
class CSpiderAnt;

class CSpiderAnt_Appear final :
    public CSpiderAnt_State
{
public:
    virtual HRESULT Enter(CSpiderAnt* pSpiderAnt) override;
    virtual void	Tick(_float fTimeDelta) override;
    virtual void	Exit(CSpiderAnt* pSpiderAnt) override;

private:
	CSpiderAnt* m_pSpiderAnt = { nullptr };
	CGameObject* m_pObject = { nullptr };
	CTransform* m_pTransformCom = { nullptr };
	CTransform* m_pPlayerTransformCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CNavigation* m_pNavigationCom = { nullptr };

private:
    _bool m_bLastAttack = { false };
	_bool m_bAnimDone = { false };
	_int	m_iSoundCount = { 0 };
};

END