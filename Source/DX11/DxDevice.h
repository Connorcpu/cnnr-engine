#pragma once

#include <Renderer/Device.h>
#include <Renderer/RenderHelpers.h>
#include "DxInstance.h"
#include "DxShader.h"

class DxDevice : public ImplRenderBase<IDevice, DxInstance>
{
public:
    DxDevice(DxInstance *inst, const DeviceParams *params);

    virtual void CreateSpriteSet(
        const SpriteSetParams *params,
        ISpriteSet **set
    ) override;

    virtual void CreateScene(
        IScene **scene
    ) override;

    virtual void CreateCamera(
        ICamera **camera
    ) override;


    virtual void Lock() override;
    virtual void Unlock() override;

    ComPtr<ID3D11Device> device;
    ComPtr<ID3D11DeviceContext> context;
    DxShader sprite_shader;

private:
    SRWLOCK lock;
};
