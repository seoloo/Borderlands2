#pragma once
#include "Thresher_State.h"

BEGIN(Engine)
class CTransform;
class CModel;
END

BEGIN(Client)
class CThresher;

class CThresher_Idle final : public CThresher_State
{
    virtual HRESULT Enter(CThresher* thresher);
    virtual void Tick(_float fTimeDelta);
    virtual void Exit(CThresher* thresher);

private:
    CThresher* m_pThresher = { nullptr };
    CTransform* m_pTransformCom = { nullptr };
    CTransform* m_pPlayerTransformCom = { nullptr };
    CModel* m_pModelCom = { nullptr };
};

END