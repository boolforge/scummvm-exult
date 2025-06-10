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
#include "ack/graphics.h"
#include "ack/graphics_renderer.h"

namespace Ack {

class GraphicsTestSuite : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(GraphicsTestSuite);
    CPPUNIT_TEST(testGraphicsInitialization);
    CPPUNIT_TEST(testDrawingFunctions);
    CPPUNIT_TEST(testPaletteFunctions);
    CPPUNIT_TEST_SUITE_END();

private:
    AckEngine *_engine;
    Graphics *_graphics;
    GraphicsRenderer *_graphicsRenderer;

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
        _graphics = _engine->getGraphics();
        _graphicsRenderer = _engine->getGraphicsRenderer();
    }

    void tearDown() {
        delete _engine;
    }

    void testGraphicsInitialization() {
        // Test graphics initialization
        CPPUNIT_ASSERT(_graphics != nullptr);
        CPPUNIT_ASSERT(_graphicsRenderer != nullptr);

        // Test screen dimensions
        CPPUNIT_ASSERT_EQUAL(320, _graphics->getScreenWidth());
        CPPUNIT_ASSERT_EQUAL(200, _graphics->getScreenHeight());

        // Test surface creation
        CPPUNIT_ASSERT(_graphics->getSurface() != nullptr);
    }

    void testDrawingFunctions() {
        // Test drawing functions
        // Since we can't actually see the output, we'll just test the API
        // and make sure it doesn't crash
        
        // Test clear screen
        _graphics->clearScreen();
        
        // Test draw line
        _graphics->drawLine(0, 0, 100, 100, 1);
        
        // Test draw box
        _graphics->drawBox(10, 10, 50, 50, 2);
        
        // Test fill box
        _graphics->fillBox(60, 60, 100, 100, 3);
        
        // Test draw text
        _graphics->drawText(10, 10, "Test Text", 4);
        
        // Test update screen
        _graphics->updateScreen();
    }

    void testPaletteFunctions() {
        // Test palette functions
        // Since we can't actually see the output, we'll just test the API
        // and make sure it doesn't crash
        
        // Test set palette
        byte palette[256 * 3];
        for (int i = 0; i < 256; i++) {
            palette[i * 3 + 0] = i;
            palette[i * 3 + 1] = i;
            palette[i * 3 + 2] = i;
        }
        _graphics->setPalette(palette, 0, 256);
        
        // Test set palette entry
        _graphics->setPaletteEntry(0, 255, 0, 0);
        
        // Test fade in/out
        // These are visual effects, so we'll just test the API
        _graphics->fadeIn(1, 1);
        _graphics->fadeOut(1, 1);
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(GraphicsTestSuite);

} // End of namespace Ack
