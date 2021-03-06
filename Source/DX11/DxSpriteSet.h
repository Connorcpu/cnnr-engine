#pragma once

#include <Common/Platform.h>
#include <Renderer/SpriteSet.h>
#include <Renderer/RenderHelpers.h>
#include <Renderer/Texture.h>
#include <vector>

#include "DxInstance.h"
#include "DxPlatform.h"

struct SpriteEntry;
class DxDevice;

class DxSpriteSet : public ImplRenderBase<ISpriteSet, DxInstance>
{
public:
    DxSpriteSet(
        DxInstance *instance, DxDevice *device,
        bool streaming, uint32_t spriteCount,
        uint32_t spriteWidth, uint32_t spriteHeight,
        const uint8_t *const *buffers,
        bool pixel_art
    );

    // Get the width and height in pixels of each image in this set
    virtual void GetSpriteSize(uint32_t *width, uint32_t *height) override;
    // Get the number of images in this set
    virtual uint32_t GetSpriteCount() override;
    // Get whether images can be streamed
    virtual bool IsStreaming() override;
    virtual void SetPixelArt(bool pa) override;

    // Get the regular non-streaming sprite pointer for image number `index`. This
    // function will fail and return false if index is out of bounds.
    virtual bool GetSprite(uint32_t index, ITexture **texture) override;
    // Get a pointer to a sprite that lets you update its data. This function will
    // fail and return false if index is out of bounds or this sprite set is not
    // set up for streaming.
    virtual bool GetStreamingSprite(uint32_t index, IStreamingTexture **texture) override;
    
    ID3D11ShaderResourceView *const &GetView() const;

    bool is_pixel_art = false;

private:
    std::vector<SpriteEntry> entries;
    RPtr<DxDevice> device;
    ComPtr<ID3D11Texture2D> backing_store;
    ComPtr<ID3D11ShaderResourceView> sr_view;
    uint32_t sprite_width, sprite_height;
    bool is_streaming;
    friend struct SpriteEntry;
};

struct SpriteEntry : public IStreamingTexture
{
public:
    SpriteEntry(DxSpriteSet *owner, ID3D11Texture2D *backing, uint32_t i)
        : owner(owner), backing_store(backing), index(i)
    {
    }

    virtual IDevice *GetDevice() override;
    virtual void GetSize(uint32_t *width, uint32_t *height) override;
    virtual void Update(const uint8_t *data, size_t len) override;

    DxSpriteSet *owner;
    ID3D11Texture2D *backing_store;
    uint32_t index;
};
