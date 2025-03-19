/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "common/file.h"
#include "common/system.h"
#include "common/textconsole.h"
#include "audio/audiostream.h"
#include "audio/decoders/raw.h"
#include "audio/decoders/wave.h"

#include "ack/ack.h"
#include "ack/sound.h"
#include "ack/sound_decoder.h"

namespace Ack {

Sound::Sound(AckEngine *vm) : _vm(vm), _musicPlaying(false), _currentMusic(-1) {
    _mixer = g_system->getMixer();
}

Sound::~Sound() {
    stopMusic();
    
    // Stop all sound channels
    for (int i = 0; i < 4; i++) {
        stopSound(i);
    }
}

/**
 * Play a sound effect
 * 
 * Original Pascal:
 * procedure playsound(name: string; channel: byte);
 * 
 * @param name The name of the sound file to play
 * @param channel The channel to play the sound on (0-3)
 */
void Sound::playSound(const Common::String &name, byte channel) {
    if (channel >= 4) {
        warning("Invalid sound channel: %d", channel);
        return;
    }
    
    // Stop any sound currently playing on this channel
    stopSound(channel);
    
    // Open the sound file
    Common::File *soundFile = new Common::File();
    if (!soundFile->open(name)) {
        warning("Could not open sound file: %s", name.c_str());
        delete soundFile;
        return;
    }
    
    // Create an audio stream from the sound file
    Audio::RewindableAudioStream *stream = nullptr;
    
    // Check file extension to determine format
    if (name.hasSuffix(".WAV")) {
        // WAV file
        stream = Audio::makeWAVStream(soundFile, DisposeAfterUse::YES);
    } else {
        // Raw PCM data (assuming 8-bit unsigned mono at 11025 Hz)
        uint32 size = soundFile->size();
        byte *data = (byte *)malloc(size);
        
        if (!data) {
            warning("Could not allocate memory for sound data");
            delete soundFile;
            return;
        }
        
        if (soundFile->read(data, size) != size) {
            warning("Could not read sound data from file: %s", name.c_str());
            free(data);
            delete soundFile;
            return;
        }
        
        stream = Audio::makeRawStream(data, size, 11025, Audio::FLAG_UNSIGNED, DisposeAfterUse::YES);
        delete soundFile; // Stream now owns the data
    }
    
    if (!stream) {
        warning("Could not create audio stream for sound: %s", name.c_str());
        return;
    }
    
    // Play the sound
    _mixer->playStream(Audio::Mixer::kSFXSoundType, &_soundHandles[channel], stream);
}

/**
 * Stop a sound effect
 * 
 * Original Pascal:
 * procedure stopsound(channel: byte);
 * 
 * @param channel The channel to stop (0-3)
 */
void Sound::stopSound(byte channel) {
    if (channel >= 4) {
        warning("Invalid sound channel: %d", channel);
        return;
    }
    
    _mixer->stopHandle(_soundHandles[channel]);
}

/**
 * Play background music
 * 
 * Original Pascal:
 * procedure playmusic(musicId: integer);
 * 
 * @param musicId The ID of the music to play
 */
void Sound::playMusic(int musicId) {
    // Stop any currently playing music
    stopMusic();
    
    // Load the music resource
    if (!loadSound(musicId)) {
        warning("Could not load music: %d", musicId);
        return;
    }
    
    // Play the music
    // This would use the music system
    // For now, we'll just use a placeholder
    // _mixer->playStream(Audio::Mixer::kMusicSoundType, &_musicHandle, stream);
    
    _musicPlaying = true;
    _currentMusic = musicId;
}

/**
 * Stop background music
 * 
 * Original Pascal:
 * procedure stopmusic;
 */
void Sound::stopMusic() {
    if (!_musicPlaying)
        return;
        
    // Stop the music
    // This would use the music system
    // For now, we'll just use a placeholder
    // _mixer->stopHandle(_musicHandle);
    
    _musicPlaying = false;
    _currentMusic = -1;
}

/**
 * Set the volume for sound effects and music
 * 
 * @param volume The volume level (0-255)
 */
void Sound::setVolume(byte volume) {
    // Convert from 0-255 to 0-Audio::Mixer::kMaxMixerVolume
    int mixerVolume = (volume * Audio::Mixer::kMaxMixerVolume) / 255;
    
    // Set the volume for sound effects and music
    _mixer->setVolumeForSoundType(Audio::Mixer::kSFXSoundType, mixerVolume);
    _mixer->setVolumeForSoundType(Audio::Mixer::kMusicSoundType, mixerVolume);
}

/**
 * Load a sound resource
 * 
 * @param soundId The ID of the sound to load
 * @return True if the sound was loaded successfully, false otherwise
 */
bool Sound::loadSound(int soundId) {
    // This would use the resource management system
    // For now, we'll just use a placeholder
    // return _vm->loadResource(RESOURCETYPE_SOUND, soundId);
    return true;
}

/**
 * Unload a sound resource
 * 
 * @param soundId The ID of the sound to unload
 */
void Sound::unloadSound(int soundId) {
    // This would use the resource management system
    // For now, we'll just use a placeholder
    // _vm->unloadResource(RESOURCETYPE_SOUND, soundId);
}

} // End of namespace Ack
