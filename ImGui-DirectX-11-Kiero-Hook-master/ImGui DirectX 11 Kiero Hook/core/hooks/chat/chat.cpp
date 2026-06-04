#define NOMINMAX
#include "chat.h"
#include <algorithm>
#include <cmath>
#include "../../mem.hpp"
#include "../hooks.h"


std::string MessageBuilder::buildText(const std::string& str, const ImColor& col)
{
    static constexpr const char hexChars[] = "0123456789ABCDEF";

    auto r = static_cast<unsigned char>(std::min(255, static_cast<int>(std::round(col.Value.x * 255.f))));
    auto g = static_cast<unsigned char>(std::min(255, static_cast<int>(std::round(col.Value.y * 255.f))));
    auto b = static_cast<unsigned char>(std::min(255, static_cast<int>(std::round(col.Value.z * 255.f))));

    char color_buffer[7];
    color_buffer[0] = hexChars[(r >> 4) & 0x0F];
    color_buffer[1] = hexChars[r & 0x0F];

    color_buffer[2] = hexChars[(g >> 4) & 0x0F];
    color_buffer[3] = hexChars[g & 0x0F];

    color_buffer[4] = hexChars[(b >> 4) & 0x0F];
    color_buffer[5] = hexChars[b & 0x0F];
    color_buffer[6] = '\0';

    static constexpr char color_text_start[] = R"(<font color="#)";
    static constexpr char color_text_end[] = R"(">)";
    static constexpr char text_end[] = R"(</font>)";

    std::string ret;
    ret.reserve(str.length() + sizeof(color_text_start) + sizeof(color_buffer) + sizeof(color_text_end) + sizeof(text_end) - 4);
    ret.append(color_text_start);
    ret.append(color_buffer);
    ret.append(color_text_end);
    ret.append(str);
    ret.append(text_end);

    return ret;
}

void ChatMessage::send() {
    if (!MEM::IsInGame()) return;

    uint64_t hudElement = (uint64_t)oFindHudElement("CCSGO_HudVoiceStatus");

    if (hudElement && hudElement > 32)
    {
        std::string text = GetText();

        uint64_t v163 = 1;
        oSendMessageClient((void*)(hudElement - 32), text.c_str(), 0xFFFFFFFF, &v163);
    }
}

void Init_ChatMessage()
{
	oFindHudElement = (FindHudElement_t)PatternScan("client.dll", FINDHUDELEMENT_PATTERN);
    oSendMessageClient = (SendChatMessage_t)MEM::GetAbsoluteAddress<uint8_t>(reinterpret_cast<uint8_t*>(PatternScan("client.dll", "E8 ? ? ? ? 33 D2 48 8D 4D ? FF 15 ? ? ? ? 4C 39 65") + 0x1));
}