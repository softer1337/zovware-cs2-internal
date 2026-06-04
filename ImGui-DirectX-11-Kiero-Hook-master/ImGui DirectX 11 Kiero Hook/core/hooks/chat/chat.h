#pragma once
#include <string>
#include "../../../imgui/imgui.h"

class MessageBuilder
{
public:
	MessageBuilder& operator<<(const std::string& s)
	{
		text_ += buildText(s, current_);
		return *this;
	}

	MessageBuilder& operator<<(const char* s)
	{
		return *this << std::string(s);
	}

	MessageBuilder& operator<<(int n)
	{
		return *this << std::to_string(n);
	}

	MessageBuilder& operator<<(const ImColor& c)
	{
		current_ = c;
		return *this;
	}

	inline std::string get() { return text_; }
private:
	std::string text_;
	ImColor current_ = { 1.f, 1.f, 1.f };
	std::string buildText(const std::string& str, const ImColor& col);
};

class ChatMessage
{
public:
    template<typename T>
    ChatMessage& operator<<(const T& value)
    {
        builder_ << value;
        return *this;
    }


	void send();

private:
	MessageBuilder builder_;

	std::string GetText()
	{
		return builder_.get();
	}
};

using FindHudElement_t = __int64(__fastcall*)(const char*);
inline FindHudElement_t oFindHudElement = nullptr;

using SendChatMessage_t = __int64(*)(void*, const char*, unsigned int, uint64_t*);
inline SendChatMessage_t oSendMessageClient = nullptr;


void Init_ChatMessage();