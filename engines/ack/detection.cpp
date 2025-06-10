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
#include "common/config-manager.h"
#include "common/debug.h"
#include "common/debug-channels.h"
#include "common/file.h"
#include "common/fs.h"
#include "common/savefile.h"

#include "engines/util.h"
#include "engines/advancedDetector.h"

#include "ack/ack.h"
#include "ack/detection.h"

namespace Ack {

uint32 AckEngine::getFeatures() const {
	return _gameDescription->desc.flags;
}

Common::Language AckEngine::getLanguage() const {
	return _gameDescription->desc.language;
}

Common::Platform AckEngine::getPlatform() const {
	return _gameDescription->desc.platform;
}

bool AckEngine::isDemo() const {
	return _gameDescription->desc.flags & ADGF_DEMO;
}

} // End of namespace Ack

class AckMetaEngineDetection : public AdvancedMetaEngineDetection {
public:
	AckMetaEngineDetection() : AdvancedMetaEngineDetection(Ack::gameDescriptions, sizeof(ADGameDescription), Ack::ackGames) {
		_maxScanDepth = 2;
	}

	const char *getName() const override {
		return "ack";
	}

	const char *getEngineName() const override {
		return "Adventure Construction Kit";
	}

	const char *getOriginalCopyright() const override {
		return "Adventure Construction Kit (C) 1992-2009 Chris Hopkins";
	}
};

REGISTER_PLUGIN_STATIC(ACK_DETECTION, PLUGIN_TYPE_ENGINE_DETECTION, AckMetaEngineDetection);
