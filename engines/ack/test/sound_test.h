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

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include "common/system.h"
#include "common/textconsole.h"
#include "common/debug.h"

#include "ack/ack.h"
#include "ack/sound.h"

namespace Ack {

class SoundTestSuite : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(SoundTestSuite);
    CPPUNIT_TEST(testSoundInitialization);
    CPPUNIT_TEST(testSoundPlayback);
    CPPUNIT_TEST(testMusicPlayback);
    CPPUNIT_TEST_SUITE_END();

private:
    AckEngine *_engine;
    Sound *_sound;

public:
    void setUp() {
        // Create a mock game description
        static const ADGameDescription gameDesc = {
            "ack",
            "Test Game",
            AD_ENTRY1("MASTER.DAT", "12345"),
            Common::EN_ANY,
            Common::kPlatformDOS,
            ADGF_NO_FLAGS,
            GUIO1(GUIO_NONE)
        };

        static const AckGameDescription mockGameDesc = {
            gameDesc
        };

        // Create the engine
        _engine = new AckEngine(g_system, &mockGameDesc);
        _sound = _engine->getSound();
    }

    void tearDown() {
        delete _engine;
    }

    void testSoundInitialization() {
        // Test sound initialization
        CPPUNIT_ASSERT(_sound != nullptr);
        
        // Test music state
        CPPUNIT_ASSERT_EQUAL(false, _sound->isMusicPlaying());
    }

    void testSoundPlayback() {
        // Test sound playback
        // Since we don't have actual sound files, we'll just test the API
        // and make sure it doesn't crash
        
        // Test play sound
        _sound->playSound("test.wav", 0);
        
        // Test stop sound
        _sound->stopSound(0);
        
        // Test volume
        _sound->setVolume(128);
    }

    void testMusicPlayback() {
        // Test music playback
        // Since we don't have actual music files, we'll just test the API
        // and make sure it doesn't crash
        
        // Test play music
        _sound->playMusic(0);
        
        // Test stop music
        _sound->stopMusic();
        CPPUNIT_ASSERT_EQUAL(false, _sound->isMusicPlaying());
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(SoundTestSuite);

} // End of namespace Ack
