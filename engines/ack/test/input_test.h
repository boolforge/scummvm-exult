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
#include "ack/input.h"

namespace Ack {

class InputTestSuite : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(InputTestSuite);
    CPPUNIT_TEST(testInputInitialization);
    CPPUNIT_TEST(testKeyboardBuffer);
    CPPUNIT_TEST(testKeyCodeConversion);
    CPPUNIT_TEST(testMouseHandling);
    CPPUNIT_TEST_SUITE_END();

private:
    AckEngine *_engine;
    Input *_input;

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
        _input = _engine->getInput();
    }

    void tearDown() {
        delete _engine;
    }

    void testInputInitialization() {
        // Test input initialization
        CPPUNIT_ASSERT(_input != nullptr);
        
        // Test keyboard buffer is initially empty
        CPPUNIT_ASSERT_EQUAL(false, _input->isKeyAvailable());
    }

    void testKeyboardBuffer() {
        // Test keyboard buffer
        
        // Test clear keyboard buffer
        _input->clearKeyboardBuffer();
        CPPUNIT_ASSERT_EQUAL(false, _input->isKeyAvailable());
        
        // Test add key to buffer
        _input->addKeyToBuffer(Common::KEYCODE_A);
        CPPUNIT_ASSERT_EQUAL(true, _input->isKeyAvailable());
        
        // Test get key from buffer
        Common::KeyCode key = _input->getKeyFromBuffer();
        CPPUNIT_ASSERT_EQUAL(Common::KEYCODE_A, key);
        CPPUNIT_ASSERT_EQUAL(false, _input->isKeyAvailable());
    }

    void testKeyCodeConversion() {
        // Test key code conversion
        
        // Test regular keys
        CPPUNIT_ASSERT_EQUAL((byte)'A', _input->convertKeyCode(Common::KEYCODE_A));
        CPPUNIT_ASSERT_EQUAL((byte)'Z', _input->convertKeyCode(Common::KEYCODE_Z));
        CPPUNIT_ASSERT_EQUAL((byte)'0', _input->convertKeyCode(Common::KEYCODE_0));
        CPPUNIT_ASSERT_EQUAL((byte)'9', _input->convertKeyCode(Common::KEYCODE_9));
        
        // Test special keys
        CPPUNIT_ASSERT_EQUAL((byte)8, _input->convertKeyCode(Common::KEYCODE_BACKSPACE));
        CPPUNIT_ASSERT_EQUAL((byte)9, _input->convertKeyCode(Common::KEYCODE_TAB));
        CPPUNIT_ASSERT_EQUAL((byte)13, _input->convertKeyCode(Common::KEYCODE_RETURN));
        CPPUNIT_ASSERT_EQUAL((byte)27, _input->convertKeyCode(Common::KEYCODE_ESCAPE));
        CPPUNIT_ASSERT_EQUAL((byte)32, _input->convertKeyCode(Common::KEYCODE_SPACE));
        
        // Test function keys
        CPPUNIT_ASSERT_EQUAL((byte)59, _input->convertKeyCode(Common::KEYCODE_F1));
        CPPUNIT_ASSERT_EQUAL((byte)68, _input->convertKeyCode(Common::KEYCODE_F10));
        
        // Test arrow keys
        CPPUNIT_ASSERT_EQUAL((byte)72, _input->convertKeyCode(Common::KEYCODE_UP));
        CPPUNIT_ASSERT_EQUAL((byte)80, _input->convertKeyCode(Common::KEYCODE_DOWN));
        CPPUNIT_ASSERT_EQUAL((byte)75, _input->convertKeyCode(Common::KEYCODE_LEFT));
        CPPUNIT_ASSERT_EQUAL((byte)77, _input->convertKeyCode(Common::KEYCODE_RIGHT));
    }

    void testMouseHandling() {
        // Test mouse handling
        
        // Test get mouse position
        int x, y;
        _input->getMousePosition(&x, &y);
        CPPUNIT_ASSERT(x >= 0);
        CPPUNIT_ASSERT(y >= 0);
        
        // Test get mouse buttons
        byte buttons = _input->getMouseButtons();
        CPPUNIT_ASSERT(buttons >= 0);
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(InputTestSuite);

} // End of namespace Ack
