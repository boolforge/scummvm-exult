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

#include "common/system.h"
#include "common/textconsole.h"
#include "audio/audiostream.h"
#include "audio/decoders/raw.h"

#include "ack/sound_decoder.h"

namespace Ack {

AckSoundDecoder::AckSoundDecoder(byte *data, uint32 size, uint32 rate, bool stereo) :
    _data(data), _size(size), _rate(rate), _stereo(stereo) {
}

AckSoundDecoder::~AckSoundDecoder() {
    // We don't free _data here as it's owned by the caller
}

/**
 * Create an audio stream from the sound data
 * 
 * @return A rewindable audio stream
 */
Audio::RewindableAudioStream *AckSoundDecoder::createAudioStream() {
    // Create a copy of the data that will be owned by the audio stream
    byte *data = (byte *)malloc(_size);
    if (!data) {
        warning("Could not allocate memory for sound data");
        return nullptr;
    }
    
    memcpy(data, _data, _size);
    
    // Create a raw audio stream
    // ACK sounds are typically 8-bit unsigned PCM
    return Audio::makeRawStream(data, _size, _rate, 
                               _stereo ? Audio::FLAG_UNSIGNED | Audio::FLAG_STEREO : Audio::FLAG_UNSIGNED);
}

} // End of namespace Ack
