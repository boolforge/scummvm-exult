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
#include "common/config-manager.h"
#include "engines/util.h"

#include "ack/ack.h"
#include "ack/graphics.h"
#include "ack/input.h"
#include "ack/resource.h"
#include "ack/script.h"
#include "ack/sound.h"

namespace Ack {

class AckEngineTestSuite : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(AckEngineTestSuite);
    CPPUNIT_TEST(testGraphicsInitialization);
    CPPUNIT_TEST(testInputHandling);
    CPPUNIT_TEST(testResourceLoading);
    CPPUNIT_TEST(testScriptExecution);
    CPPUNIT_TEST(testSoundPlayback);
    CPPUNIT_TEST_SUITE_END();

private:
    AckEngine *_engine;

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
    }

    void tearDown() {
        delete _engine;
    }

    void testGraphicsInitialization() {
        // Test graphics initialization
        Graphics *graphics = _engine->getGraphics();
        CPPUNIT_ASSERT(graphics != nullptr);

        // Test screen dimensions
        CPPUNIT_ASSERT_EQUAL(320, graphics->getScreenWidth());
        CPPUNIT_ASSERT_EQUAL(200, graphics->getScreenHeight());

        // Test surface creation
        CPPUNIT_ASSERT(graphics->getSurface() != nullptr);
    }

    void testInputHandling() {
        // Test input initialization
        Input *input = _engine->getInput();
        CPPUNIT_ASSERT(input != nullptr);

        // Test keyboard buffer is initially empty
        CPPUNIT_ASSERT_EQUAL(false, input->isKeyAvailable());

        // Test key code conversion
        CPPUNIT_ASSERT_EQUAL((byte)13, input->convertKeyCode(Common::KEYCODE_RETURN));
        CPPUNIT_ASSERT_EQUAL((byte)27, input->convertKeyCode(Common::KEYCODE_ESCAPE));
        CPPUNIT_ASSERT_EQUAL((byte)32, input->convertKeyCode(Common::KEYCODE_SPACE));
    }

    void testResourceLoading() {
        // Test resource manager initialization
        ResourceManager *resourceManager = _engine->getResourceManager();
        CPPUNIT_ASSERT(resourceManager != nullptr);

        // Test system directory
        CPPUNIT_ASSERT(!resourceManager->getSystemDir().empty());
    }

    void testScriptExecution() {
        // Test script interpreter initialization
        ScriptInterpreter *scriptInterpreter = _engine->getScriptInterpreter();
        CPPUNIT_ASSERT(scriptInterpreter != nullptr);

        // Test script execution state
        CPPUNIT_ASSERT_EQUAL(false, scriptInterpreter->isScriptRunning());
    }

    void testSoundPlayback() {
        // Test sound system initialization
        Sound *sound = _engine->getSound();
        CPPUNIT_ASSERT(sound != nullptr);

        // Test music state
        CPPUNIT_ASSERT_EQUAL(false, sound->isMusicPlaying());
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(AckEngineTestSuite);

} // End of namespace Ack
