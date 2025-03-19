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
#include "ack/detection.h"
#include "ack/metaengine.h"

namespace Ack {

class DetectionTestSuite : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(DetectionTestSuite);
    CPPUNIT_TEST(testGameDetection);
    CPPUNIT_TEST(testMetaEngine);
    CPPUNIT_TEST(testEngineFeatures);
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

    void testGameDetection() {
        // Test game detection
        
        // Test game features
        CPPUNIT_ASSERT_EQUAL((uint32)0, _engine->getFeatures());
        
        // Test game language
        CPPUNIT_ASSERT_EQUAL(Common::EN_ANY, _engine->getLanguage());
        
        // Test game platform
        CPPUNIT_ASSERT_EQUAL(Common::kPlatformDOS, _engine->getPlatform());
        
        // Test if game is demo
        CPPUNIT_ASSERT_EQUAL(false, _engine->isDemo());
    }

    void testMetaEngine() {
        // Test meta engine
        // Since we can't create a meta engine directly in the test,
        // we'll just test the engine features
        
        // Test engine features
        CPPUNIT_ASSERT_EQUAL(true, _engine->hasFeature(Engine::kSupportsReturnToLauncher));
        CPPUNIT_ASSERT_EQUAL(true, _engine->hasFeature(Engine::kSupportsLoadingDuringRuntime));
        CPPUNIT_ASSERT_EQUAL(true, _engine->hasFeature(Engine::kSupportsSavingDuringRuntime));
    }

    void testEngineFeatures() {
        // Test engine features
        
        // Test save/load state
        CPPUNIT_ASSERT_EQUAL(true, _engine->canSaveGameStateCurrently());
        CPPUNIT_ASSERT_EQUAL(true, _engine->canLoadGameStateCurrently());
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(DetectionTestSuite);

} // End of namespace Ack
