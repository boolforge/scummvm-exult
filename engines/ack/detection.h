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

#ifndef ACK_DETECTION_H
#define ACK_DETECTION_H

#include "engines/advancedDetector.h"

namespace Ack {

enum {
	GF_DEMO = (1 << 0)
};

struct AckGameDescription {
	ADGameDescription desc;
};

static const PlainGameDescriptor ackGames[] = {
	{"ack", "Adventure Construction Kit Game"},
	{nullptr, nullptr}
};

static const ADGameDescription gameDescriptions[] = {
	{
		"ack",
		"Demo",
		AD_ENTRY1s("DEMO.ACK", "md5hash", 12345),
		Common::EN_ANY,
		Common::kPlatformDOS,
		ADGF_DEMO,
		GUIO1(GUIO_NONE)
	},
	AD_TABLE_END_MARKER
};

} // End of namespace Ack

class AckMetaEngineDetection : public AdvancedMetaEngineDetection {
public:
	AckMetaEngineDetection();
	const char *getName() const override;
	const char *getEngineName() const override;
	const char *getOriginalCopyright() const override;
};

#endif // ACK_DETECTION_H
