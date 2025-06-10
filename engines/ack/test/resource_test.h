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
#include "common/file.h"

#include "ack/ack.h"
#include "ack/resource.h"

namespace Ack {

class ResourceManagerTestSuite : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(ResourceManagerTestSuite);
    CPPUNIT_TEST(testResourceLoading);
    CPPUNIT_TEST(testResourceUnloading);
    CPPUNIT_TEST(testConfigLoading);
    CPPUNIT_TEST_SUITE_END();

private:
    AckEngine *_engine;
    ResourceManager *_resourceManager;

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
        _resourceManager = _engine->getResourceManager();
    }

    void tearDown() {
        delete _engine;
    }

    void testResourceLoading() {
        // Test resource loading
        // Since we don't have actual resource files, we'll just test the API
        // and make sure it doesn't crash
        
        // Test loading a logic resource
        bool result = _resourceManager->loadResource(RESOURCETYPE_LOGIC, 0);
        // We expect this to fail since we don't have actual resource files
        CPPUNIT_ASSERT_EQUAL(false, result);
        
        // Test loading a sound resource
        result = _resourceManager->loadResource(RESOURCETYPE_SOUND, 0);
        CPPUNIT_ASSERT_EQUAL(false, result);
        
        // Test loading a view resource
        result = _resourceManager->loadResource(RESOURCETYPE_VIEW, 0);
        CPPUNIT_ASSERT_EQUAL(false, result);
        
        // Test loading a picture resource
        result = _resourceManager->loadResource(RESOURCETYPE_PICTURE, 0);
        CPPUNIT_ASSERT_EQUAL(false, result);
    }

    void testResourceUnloading() {
        // Test resource unloading
        // Since we don't have actual resource files, we'll just test the API
        // and make sure it doesn't crash
        
        // Test unloading a logic resource
        _resourceManager->unloadResource(RESOURCETYPE_LOGIC, 0);
        
        // Test unloading a sound resource
        _resourceManager->unloadResource(RESOURCETYPE_SOUND, 0);
        
        // Test unloading a view resource
        _resourceManager->unloadResource(RESOURCETYPE_VIEW, 0);
        
        // Test unloading a picture resource
        _resourceManager->unloadResource(RESOURCETYPE_PICTURE, 0);
    }

    void testConfigLoading() {
        // Test config loading
        // Since we don't have actual config files, we'll just test the API
        // and make sure it doesn't crash
        
        // Test loading config
        bool result = _resourceManager->loadConfig();
        // We expect this to fail since we don't have actual config files
        CPPUNIT_ASSERT_EQUAL(false, result);
        
        // Test system directory
        CPPUNIT_ASSERT(!_resourceManager->getSystemDir().empty());
        
        // Test adventure name
        CPPUNIT_ASSERT_EQUAL(Common::String("NONAME"), _resourceManager->getAdvName());
        
        // Test master file
        CPPUNIT_ASSERT_EQUAL(Common::String("MASTER.DAT"), _resourceManager->getMasterFile());
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(ResourceManagerTestSuite);

} // End of namespace Ack
