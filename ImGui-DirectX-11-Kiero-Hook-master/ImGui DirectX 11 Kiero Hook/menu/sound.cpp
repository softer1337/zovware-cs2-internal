#include "sound.h"
#include <xaudio2.h>
#include "hit_sound1.h"
#include "hit_sound2.h"
#include "tung_sahur.h"
#pragma comment(lib,"xaudio2.lib")

IXAudio2* g_XAudio = nullptr;
IXAudio2MasteringVoice* g_Master = nullptr;

void Audio_Init()
{
    XAudio2Create(&g_XAudio);
    g_XAudio->CreateMasteringVoice(&g_Master);
}
struct WavData
{
    WAVEFORMATEX format{};
    XAUDIO2_BUFFER buffer{};
    BYTE* data = nullptr;
};

bool LoadWavFromMemory(const unsigned char* src, size_t size, WavData& out)
{
    const BYTE* p = src;

    if (*(DWORD*)p != 'FFIR') return false; p += 8;
    if (*(DWORD*)p != 'EVAW') return false; p += 4;

    while (p < src + size)
    {
        DWORD id = *(DWORD*)p; p += 4;
        DWORD sz = *(DWORD*)p; p += 4;

        if (id == ' tmf')
        {
            memcpy(&out.format, p, sizeof(WAVEFORMATEX));
        }
        else if (id == 'atad')
        {
            out.data = new BYTE[sz];
            memcpy(out.data, p, sz);

            out.buffer.AudioBytes = sz;
            out.buffer.pAudioData = out.data;
            out.buffer.Flags = XAUDIO2_END_OF_STREAM;
        }
        p += sz;
    }
    return true;
}

WavData g_HitSound_mine;
WavData g_HitSound_coin;
WavData g_MemeSound_sahur;

void initAudioSystem()
{
    Audio_Init();
    LoadWavFromMemory(hit_wav1, hit_wav_size1, g_HitSound_mine);
    LoadWavFromMemory(hit_wav2, hit_wav_size2, g_HitSound_coin);
    LoadWavFromMemory(tung_wav, tung_wav_size, g_MemeSound_sahur);

}



void PlayHitSound(float volume, HitSoundType type)
{
    IXAudio2SourceVoice* voice;
    switch (type)
    {
    case MINECRAFT:

        g_XAudio->CreateSourceVoice(&voice, &g_HitSound_mine.format);
        voice->SetVolume(volume);
        voice->SubmitSourceBuffer(&g_HitSound_mine.buffer);
        voice->Start();
        break;

    case COIN:

        g_XAudio->CreateSourceVoice(&voice, &g_HitSound_coin.format);
        voice->SetVolume(volume);
        voice->SubmitSourceBuffer(&g_HitSound_coin.buffer);
        voice->Start();
        break;

    case BRAINROT:
        g_XAudio->CreateSourceVoice(&voice, &g_MemeSound_sahur.format);
        voice->SetVolume(volume);
        voice->SubmitSourceBuffer(&g_MemeSound_sahur.buffer);
        voice->Start();
        break;

    }




}