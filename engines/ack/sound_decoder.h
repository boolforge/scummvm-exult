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

#ifndef ACK_SOUND_DECODER_H
#define ACK_SOUND_DECODER_H

#include "common/scummsys.h"
#include "common/str.h"
#include "audio/audiostream.h"

namespace Ack {

/**
 * Sound decoder for ACK engine
 * Handles decoding of ACK sound formats
 */
class AckSoundDecoder {
private:
    byte *_data;
    uint32 _size;
    uint32 _rate;
    bool _stereo;
    
public:
    AckSoundDecoder(byte *data, uint32 size, uint32 rate = 11025, bool stereo = false);
    ~AckSoundDecoder();
    
    // Create an audio stream from the sound data
    Audio::RewindableAudioStream *createAudioStream();
    
    // Accessors
    byte *getData() const { return _data; }
    uint32 getSize() const { return _size; }
    uint32 getRate() const { return _rate; }
    bool isStereo() const { return _stereo; }
};

} // End of namespace Ack

#endif // ACK_SOUND_DECODER_H
