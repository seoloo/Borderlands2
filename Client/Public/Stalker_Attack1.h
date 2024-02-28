#pragma once
#include "Stalker_State.h"

BEGIN(Engine)
class CTransform;
class CModel;
END

BEGIN(Client)
class CStalker;

class CStalker_Attack1 final :
    public CStalker_State
{
    virtual HRESULT Enter(CStalker* stalker);
    virtual void Tick(_float fTimeDelta);
    virtual void Exit(CStalker* stalker);

private:
    CStalker* m_pStalker = { nullptr };
    CTransform* m_pTransformCom = { nullptr };
    CTransform* m_pPlayerTransformCom = { nullptr };
    CModel* m_pModelCom = { nullptr };

private:
    _float3 m_vPos = { 0.f,0.f,0.f };
    _float3 m_vPlayerPos = { 0.f,0.f,0.f };
    _vector m_Look = { XMVectorSet(0.f,0.f,0.f,0.f) };
};

END