#include <Renderer/Renderer.h>
#include <Common/Platform.h>
#include <AssetPipeline/SpriteLoader.h>
#include <Common/MathLib.h>
#include <Common/optional.h>
#include <Renderer/StringHelpers.h>
#include <AssetPipeline/SpritePack.h>
#include <iostream>
#include <iomanip>

static const wchar_t RENDERER[] = L"DX11.dll";

const float *NextClear()
{
    static ColorF color;
    static float hue = 0;

    hue = std::fmod(hue + 0.001f, 1.f);
    color = Math::HslToRgb(Math::ColorHsl(hue, 1.0f, 1.0f));
    return &color.r;
}

int main(int, const char *)
{
    while (!fs::exists(fs::current_path() / "Assets"))
        fs::current_path(fs::current_path().parent_path());

    HMODULE renderlib = LoadLibraryW(RENDERER);
    PCreateRenderInstance CreateRenderInstance =
        (PCreateRenderInstance)GetProcAddress(renderlib, "CreateRenderInstance");

    CreateInstParams instparams;
    instparams.instance_name = "Test Renderer";
    instparams.adapter_num = 0;
    RPtr<IRenderInstance> inst;
    CreateRenderInstance(&instparams, &inst);

    DeviceParams devparams;
    devparams.debug = true;
    RPtr<IDevice> dev;
    inst->CreateDevice(&devparams, &dev);

    SpriteLoader loader{ dev.p };
    loader.Load("Test");

    auto gif = (GifPack *)loader.Load("TestGif");
    auto gif_time = std::chrono::system_clock::now();
    ImageLoad::duration gif_dur;
    gif->LoadFrame(0, &gif_dur);
    gif->CacheNextThreaded(1);

    DisplayParams disparams;
    disparams.device = dev.p;
    disparams.window_title = "Hi there :D";
    RPtr<IDisplay> display;
    inst->CreateDisplay(&disparams, &display);

    bool quit = false;
    uint32_t gif_frame = 0;
    while (!quit)
    {
        EventStorage event;
        while (display->PollEvent(event))
        {
            switch (event.base.type)
            {
                case EventType::Resized:
                {
                    break;
                }

                case EventType::Moved:
                {
                    break;
                }

                case EventType::DroppedFile:
                {
                    std::cout << "Dropped a file: " << event.dropped_file.path << std::endl;
                    break;
                }

                case EventType::ReceivedCharacter:
                {
                    uint32_t code = event.received_character.codepoint;
                    std::cout << "Unicode char: U+"
                              << std::setfill('0') << std::setw(6) << std::hex
                              << code << std::endl;
                    break;
                }

                case EventType::Focused:
                {
                    break;
                }

                case EventType::MouseMoved:
                {
                    break;
                }

                case EventType::MouseInput:
                {
                    if (event.mouse_input.state == ElementState::Pressed)
                    {
                        std::cout << "Mouse down" << std::endl;
                    }
                    else
                    {
                        std::cout << "Mouse up" << std::endl;
                    }
                    break;
                }

                case EventType::Closed:
                {
                    quit = true;
                    break;
                }

                case EventType::Touch:
                {
                    if (event.touch.phase != TouchPhase::Moved)
                    {
                        std::cout << "Touch " << event.touch.id << " " << TouchPhaseName(event.touch.phase) << std::endl;
                    }
                    break;
                }

                default:
                {
                    std::cout << "Unknown event " << EventTypeName(event.base.type) << std::endl;
                    break;
                }
            }
        }

        auto now = std::chrono::system_clock::now();
        if (gif_time + gif_dur >= now)
        {
            gif_time = now;
            gif_frame++;
            if (!gif->LoadFrame(gif_frame, &gif_dur))
                gif_frame = 0;
            gif->CacheNextThreaded(gif_frame + 1);
        }

        display->Clear(NextClear());
        display->Present();
        Sleep(16);
    }

    return 0;
}
