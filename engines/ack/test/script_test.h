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
#include "ack/script.h"

namespace Ack {

class ScriptInterpreterTestSuite : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(ScriptInterpreterTestSuite);
    CPPUNIT_TEST(testScriptLoading);
    CPPUNIT_TEST(testScriptExecution);
    CPPUNIT_TEST(testScriptVariables);
    CPPUNIT_TEST_SUITE_END();

private:
    AckEngine *_engine;
    ScriptInterpreter *_scriptInterpreter;

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
        _scriptInterpreter = _engine->getScriptInterpreter();
    }

    void tearDown() {
        delete _engine;
    }

    void testScriptLoading() {
        // Test script loading
        // Since we don't have actual script files, we'll just test the API
        // and make sure it doesn't crash
        
        // Test loading a script
        bool result = _scriptInterpreter->loadScript("test.scr");
        // We expect this to fail since we don't have actual script files
        CPPUNIT_ASSERT_EQUAL(false, result);
    }

    void testScriptExecution() {
        // Test script execution
        // Since we don't have actual script files, we'll just test the API
        // and make sure it doesn't crash
        
        // Test script execution state
        CPPUNIT_ASSERT_EQUAL(false, _scriptInterpreter->isScriptRunning());
        
        // Test running a script
        _scriptInterpreter->runScript();
        
        // Test stopping a script
        _scriptInterpreter->stopScript();
        CPPUNIT_ASSERT_EQUAL(false, _scriptInterpreter->isScriptRunning());
    }

    void testScriptVariables() {
        // Test script variables
        
        // Test setting a variable
        _engine->setVar(0, 42);
        CPPUNIT_ASSERT_EQUAL((byte)42, _engine->getVar(0));
        
        // Test setting a flag
        _engine->setFlag(0, true);
        CPPUNIT_ASSERT_EQUAL(true, _engine->getFlag(0));
        
        // Test setting a string
        _engine->setString(0, "Test String");
        CPPUNIT_ASSERT_EQUAL(0, strcmp("Test String", _engine->getString(0)));
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(ScriptInterpreterTestSuite);

} // End of namespace Ack
