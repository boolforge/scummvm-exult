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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "engines/advancedDetector.h"
#include "engines/exult/exult_engine.h"
#include "common/file.h"

namespace Exult {

static const PlainGameDescriptor exultGames[] = {
	{"ultima7", "Ultima VII: The Black Gate"},
	{"ultima7si", "Ultima VII: Serpent Isle"},
	{nullptr, nullptr}
};

static const ADGameDescription gameDescriptions[] = {
	{
		"ultima7",
		nullptr,
		AD_ENTRY1s("static/u7chunks.flx", "6fd0d4b3512c7f8b8e7e6b8f8b8e7e6b", 1234567),
		Common::EN_ANY,
		Common::kPlatformDOS,
		ADGF_NO_FLAGS,
		GUIO0()
	},
	{
		"ultima7si",
		nullptr,
		AD_ENTRY1s("static/u7sifix.flx", "7fd0d4b3512c7f8b8e7e6b8f8b8e7e6b", 2345678),
		Common::EN_ANY,
		Common::kPlatformDOS,
		ADGF_NO_FLAGS,
		GUIO0()
	},
	AD_TABLE_END_MARKER
};

class ExultMetaEngine : public AdvancedMetaEngine {
public:
	const char *getName() const override {
		return "exult";
	}

	const char *getOriginalCopyright() const override {
		return "Ultima VII (C) Origin Systems";
	}

	bool hasFeature(MetaEngineFeature f) const override;
	Common::Error createInstance(OSystem *syst, Engine **engine, const ADGameDescription *desc) const override;
};

bool ExultMetaEngine::hasFeature(MetaEngineFeature f) const {
	return
		(f == kSupportsListSaves) ||
		(f == kSupportsLoadingDuringStartup) ||
		(f == kSupportsDeleteSave) ||
		(f == kSavesSupportMetaInfo) ||
		(f == kSavesSupportThumbnail) ||
		(f == kSavesSupportCreationDate) ||
		(f == kSavesSupportPlayTime);
}

Common::Error ExultMetaEngine::createInstance(OSystem *syst, Engine **engine, const ADGameDescription *desc) const {
	*engine = new ExultEngine(syst);
	return Common::kNoError;
}

} // End of namespace Exult

#if PLUGIN_ENABLED_DYNAMIC(EXULT)
	REGISTER_PLUGIN_DYNAMIC(EXULT, PLUGIN_TYPE_ENGINE, Exult::ExultMetaEngine);
#else
	REGISTER_PLUGIN_STATIC(EXULT, PLUGIN_TYPE_ENGINE, Exult::ExultMetaEngine);
#endif

