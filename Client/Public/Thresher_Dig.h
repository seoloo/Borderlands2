#pragma once
#include "Thresher_State.h"
BEGIN(Engine)
class CTransform;
class CNavigation;
class CModel;
class CGameObject;
END

BEGIN(Client)
class CThresher;

class CThresher_Dig : public CThresher_State
{
    virtual HRESULT Enter(CThresher* thresher);
    virtual void Tick(_float fTimeDelta);
    virtual void Exit(CThresher* thresher);

private:
    CThresher* m_pThresher = { nullptr };
    CModel* m_pModelCom = { nullptr };
    CShader* m_pShaderCom = { nullptr };
    CTransform* m_pTransformCom = { nullptr };
    CTransform* m_pPlayerTransformCom = { nullptr };
    CNavigation* m_pNavigationCom = { nullptr };
    CGameObject* m_pObject = { nullptr };

private:
    _uint m_iCount = { 0 };
    _bool m_bChange = { false };
};

END