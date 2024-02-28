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

class CThresher_Summon : public CThresher_State
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
    _uint m_iRandom = { 0 };
    _uint m_iAnimIndex = { 0 };
    _bool m_bNextAnim = { false };
    _bool m_bAnimEnd = { false };
    _float  m_fBlastTimer = { 0.f };

    _float  m_fAttackTimer = { 0.f };
};

END