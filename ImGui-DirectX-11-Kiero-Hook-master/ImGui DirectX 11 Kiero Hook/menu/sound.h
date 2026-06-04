#pragma once


enum HitSoundType {
	MINECRAFT,
	COIN,
	BRAINROT
};


void initAudioSystem();
void PlayHitSound(float volume, HitSoundType type);