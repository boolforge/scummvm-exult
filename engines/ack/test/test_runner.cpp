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

#include <cppunit/TestRunner.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>

#include "common/system.h"
#include "common/textconsole.h"
#include "common/debug.h"

#include "ack/test/ack_engine_test.h"

int main(int argc, char* argv[]) {
    // Create the event manager and test controller
    CppUnit::TestResult controller;

    // Add a listener that collects test results
    CppUnit::TestResultCollector result;
    controller.addListener(&result);

    // Add a listener that prints dots as tests run
    CppUnit::BriefTestProgressListener progress;
    controller.addListener(&progress);

    // Add the top suite to the test runner
    CppUnit::TestRunner runner;
    runner.addTest(CppUnit::TestFactoryRegistry::getRegistry().makeTest());
    
    // Run the tests
    runner.run(controller);

    // Print test results in compiler format
    CppUnit::CompilerOutputter outputter(&result, std::cerr);
    outputter.write();

    // Return error code if any tests failed
    return result.wasSuccessful() ? 0 : 1;
}
