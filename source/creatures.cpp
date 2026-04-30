//////////////////////////////////////////////////////////////////////
// This file is part of Remere's Map Editor
//////////////////////////////////////////////////////////////////////
// Remere's Map Editor is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Remere's Map Editor is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.
//////////////////////////////////////////////////////////////////////

#include "main.h"

#include "gui.h"
#include "materials.h"
#include "brush.h"
#include "creatures.h"
#include "creature_brush.h"

#include <wx/dir.h>

#include <fstream>
#include <regex>
#include <sstream>
#include <vector>

CreatureDatabase g_creatures;

CreatureType::CreatureType() :
	isNpc(false),
	missing(false),
	in_other_tileset(false),
	standard(false),
	name(""),
	brush(nullptr) {
	////
}

CreatureType::CreatureType(const CreatureType& ct) :
	isNpc(ct.isNpc),
	missing(ct.missing),
	in_other_tileset(ct.in_other_tileset),
	standard(ct.standard),
	name(ct.name),
	outfit(ct.outfit),
	brush(ct.brush) {
	////
}

CreatureType& CreatureType::operator=(const CreatureType& ct) {
	isNpc = ct.isNpc;
	missing = ct.missing;
	in_other_tileset = ct.in_other_tileset;
	standard = ct.standard;
	name = ct.name;
	outfit = ct.outfit;
	brush = ct.brush;
	return *this;
}

CreatureType::~CreatureType() {
	////
}

CreatureType* CreatureType::loadFromXML(pugi::xml_node node, wxArrayString& warnings) {
	pugi::xml_attribute attribute;
	if (!(attribute = node.attribute("type"))) {
		warnings.push_back("Couldn't read type tag of creature node.");
		return nullptr;
	}

	const std::string& tmpType = attribute.as_string();
	if (tmpType != "monster" && tmpType != "npc") {
		warnings.push_back("Invalid type tag of creature node \"" + wxstr(tmpType) + "\"");
		return nullptr;
	}

	if (!(attribute = node.attribute("name"))) {
		warnings.push_back("Couldn't read name tag of creature node.");
		return nullptr;
	}

	CreatureType* ct = newd CreatureType();
	ct->name = attribute.as_string();
	ct->isNpc = tmpType == "npc";

	if ((attribute = node.attribute("looktype"))) {
		ct->outfit.lookType = attribute.as_int();
		if (g_gui.gfx.getCreatureSprite(ct->outfit.lookType) == nullptr) {
			warnings.push_back("Invalid creature \"" + wxstr(ct->name) + "\" look type #" + std::to_string(ct->outfit.lookType));
		}
	}

	if ((attribute = node.attribute("lookitem"))) {
		ct->outfit.lookItem = attribute.as_int();
	}

	if ((attribute = node.attribute("lookmount"))) {
		ct->outfit.lookMount = attribute.as_int();
	}

	if ((attribute = node.attribute("lookaddon"))) {
		ct->outfit.lookAddon = attribute.as_int();
	}

	if ((attribute = node.attribute("lookhead"))) {
		ct->outfit.lookHead = attribute.as_int();
	}

	if ((attribute = node.attribute("lookbody"))) {
		ct->outfit.lookBody = attribute.as_int();
	}

	if ((attribute = node.attribute("looklegs"))) {
		ct->outfit.lookLegs = attribute.as_int();
	}

	if ((attribute = node.attribute("lookfeet"))) {
		ct->outfit.lookFeet = attribute.as_int();
	}

	if ((attribute = node.attribute("lookmounthead"))) {
		ct->outfit.lookMountHead = attribute.as_int();
	}

	if ((attribute = node.attribute("lookmountbody"))) {
		ct->outfit.lookMountBody = attribute.as_int();
	}

	if ((attribute = node.attribute("lookmountlegs"))) {
		ct->outfit.lookMountLegs = attribute.as_int();
	}

	if ((attribute = node.attribute("lookmountfeet"))) {
		ct->outfit.lookMountFeet = attribute.as_int();
	}

	return ct;
}

CreatureType* CreatureType::loadFromOTXML(const FileName& filename, pugi::xml_document& doc, wxArrayString& warnings) {
	ASSERT(doc != nullptr);

	bool isNpc;
	pugi::xml_node node;
	if ((node = doc.child("monster"))) {
		isNpc = false;
	} else if ((node = doc.child("npc"))) {
		isNpc = true;
	} else {
		warnings.push_back("This file is not a monster/npc file");
		return nullptr;
	}

	pugi::xml_attribute attribute;
	if (!(attribute = node.attribute("name"))) {
		warnings.push_back("Couldn't read name tag of creature node.");
		return nullptr;
	}

	CreatureType* ct = newd CreatureType();
	if (isNpc) {
		ct->name = nstr(filename.GetName());
	} else {
		ct->name = attribute.as_string();
	}
	ct->isNpc = isNpc;

	for (pugi::xml_node optionNode = node.first_child(); optionNode; optionNode = optionNode.next_sibling()) {
		if (as_lower_str(optionNode.name()) != "look") {
			continue;
		}

		if ((attribute = optionNode.attribute("type"))) {
			ct->outfit.lookType = attribute.as_int();
		}

		if ((attribute = optionNode.attribute("item")) || (attribute = optionNode.attribute("lookex")) || (attribute = optionNode.attribute("typeex"))) {
			ct->outfit.lookItem = attribute.as_int();
		}

		if ((attribute = optionNode.attribute("mount"))) {
			ct->outfit.lookMount = attribute.as_int();
		}

		if ((attribute = optionNode.attribute("addon"))) {
			ct->outfit.lookAddon = attribute.as_int();
		}

		if ((attribute = optionNode.attribute("head"))) {
			ct->outfit.lookHead = attribute.as_int();
		}

		if ((attribute = optionNode.attribute("body"))) {
			ct->outfit.lookBody = attribute.as_int();
		}

		if ((attribute = optionNode.attribute("legs"))) {
			ct->outfit.lookLegs = attribute.as_int();
		}

		if ((attribute = optionNode.attribute("feet"))) {
			ct->outfit.lookFeet = attribute.as_int();
		}

		if ((attribute = optionNode.attribute("mounthead"))) {
			ct->outfit.lookMountHead = attribute.as_int();
		}

		if ((attribute = optionNode.attribute("mountbody"))) {
			ct->outfit.lookMountBody = attribute.as_int();
		}

		if ((attribute = optionNode.attribute("mountlegs"))) {
			ct->outfit.lookMountLegs = attribute.as_int();
		}

		if ((attribute = optionNode.attribute("mountfeet"))) {
			ct->outfit.lookMountFeet = attribute.as_int();
		}
	}
	return ct;
}

namespace {

struct LuaCreatureEntry {
	bool isNpc = false;
	std::string name;
	std::string variable;
	size_t createEnd = 0;
};

bool readTextFile(const FileName& filename, std::string& out, wxString& error) {
	std::ifstream file(nstr(filename.GetFullPath()), std::ios::binary);
	if (!file.is_open()) {
		error = "Couldn't open file \"" + filename.GetFullName() + "\".";
		return false;
	}

	std::ostringstream stream;
	stream << file.rdbuf();
	out = stream.str();
	return true;
}

std::string unquoteLuaString(const std::string& value) {
	if (value.size() < 2) {
		return value;
	}

	const char quote = value.front();
	if ((quote != '"' && quote != '\'') || value.back() != quote) {
		return value;
	}

	std::string result;
	result.reserve(value.size() - 2);
	bool escaped = false;
	for (size_t i = 1; i + 1 < value.size(); ++i) {
		const char c = value[i];
		if (escaped) {
			switch (c) {
				case 'n':
					result.push_back('\n');
					break;
				case 'r':
					result.push_back('\r');
					break;
				case 't':
					result.push_back('\t');
					break;
				default:
					result.push_back(c);
					break;
			}
			escaped = false;
		} else if (c == '\\') {
			escaped = true;
		} else {
			result.push_back(c);
		}
	}
	return result;
}

bool resolveLuaName(const std::string& content, const std::string& token, std::string& name) {
	if (token.empty()) {
		return false;
	}

	if (token.front() == '"' || token.front() == '\'') {
		name = unquoteLuaString(token);
		return !name.empty();
	}

	const std::regex assignmentRegex(
		"(?:local\\s+)?" + token + "\\s*=\\s*((?:\"(?:\\\\.|[^\"])*\")|(?:'(?:\\\\.|[^'])*'))",
		std::regex_constants::icase
	);
	std::smatch match;
	if (std::regex_search(content, match, assignmentRegex)) {
		name = unquoteLuaString(match[1].str());
		return !name.empty();
	}
	return false;
}

bool readBraceBlock(const std::string& content, size_t openBrace, std::string& block) {
	if (openBrace == std::string::npos || openBrace >= content.size() || content[openBrace] != '{') {
		return false;
	}

	int depth = 0;
	char quote = 0;
	bool escaped = false;
	bool lineComment = false;
	for (size_t i = openBrace; i < content.size(); ++i) {
		const char c = content[i];
		const char next = (i + 1 < content.size()) ? content[i + 1] : '\0';

		if (lineComment) {
			if (c == '\n' || c == '\r') {
				lineComment = false;
			}
			continue;
		}

		if (quote != 0) {
			if (escaped) {
				escaped = false;
			} else if (c == '\\') {
				escaped = true;
			} else if (c == quote) {
				quote = 0;
			}
			continue;
		}

		if (c == '-' && next == '-') {
			lineComment = true;
			++i;
			continue;
		}

		if (c == '"' || c == '\'') {
			quote = c;
			continue;
		}

		if (c == '{') {
			++depth;
		} else if (c == '}') {
			--depth;
			if (depth == 0) {
				block = content.substr(openBrace, i - openBrace + 1);
				return true;
			}
		}
	}
	return false;
}

bool findLuaTableAfterPattern(const std::string& content, const std::regex& pattern, size_t start, std::string& table) {
	if (start >= content.size()) {
		start = 0;
	}

	std::smatch match;
	const std::string tail = content.substr(start);
	if (!std::regex_search(tail, match, pattern)) {
		return false;
	}

	const size_t openBrace = start + match.position(0) + match.length(0) - 1;
	return readBraceBlock(content, openBrace, table);
}

void applyLuaOutfit(const std::string& table, Outfit& outfit) {
	const std::regex valueRegex(R"(([A-Za-z_][A-Za-z0-9_]*)\s*=\s*(-?\d+))", std::regex_constants::icase);
	for (std::sregex_iterator it(table.begin(), table.end(), valueRegex), end; it != end; ++it) {
		std::string key = as_lower_str((*it)[1].str());
		const int value = std::stoi((*it)[2].str());

		if (key == "looktype" || key == "type") {
			outfit.lookType = value;
		} else if (key == "looktypeex" || key == "lookitem" || key == "lookex" || key == "typeex" || key == "item") {
			outfit.lookItem = value;
		} else if (key == "lookmount" || key == "mount") {
			outfit.lookMount = value;
		} else if (key == "lookaddons" || key == "lookaddon" || key == "addons" || key == "addon") {
			outfit.lookAddon = value;
		} else if (key == "lookhead" || key == "head") {
			outfit.lookHead = value;
		} else if (key == "lookbody" || key == "body") {
			outfit.lookBody = value;
		} else if (key == "looklegs" || key == "legs") {
			outfit.lookLegs = value;
		} else if (key == "lookfeet" || key == "feet") {
			outfit.lookFeet = value;
		} else if (key == "lookmounthead" || key == "mounthead") {
			outfit.lookMountHead = value;
		} else if (key == "lookmountbody" || key == "mountbody") {
			outfit.lookMountBody = value;
		} else if (key == "lookmountlegs" || key == "mountlegs") {
			outfit.lookMountLegs = value;
		} else if (key == "lookmountfeet" || key == "mountfeet") {
			outfit.lookMountFeet = value;
		}
	}
}

std::vector<CreatureType*> loadCreatureTypesFromLua(const FileName& filename, const std::string& content, wxArrayString& warnings) {
	std::vector<LuaCreatureEntry> entries;
	const std::regex createRegex(
		R"lua((?:local\s+)?([A-Za-z_][A-Za-z0-9_]*)\s*=\s*Game\.create(Monster|Npc)Type\s*\(\s*((?:"(?:\\.|[^"])*")|(?:'(?:\\.|[^'])*')|[A-Za-z_][A-Za-z0-9_]*)\s*\))lua",
		std::regex_constants::icase
	);

	for (std::sregex_iterator it(content.begin(), content.end(), createRegex), end; it != end; ++it) {
		LuaCreatureEntry entry;
		entry.variable = (*it)[1].str();
		entry.isNpc = as_lower_str((*it)[2].str()) == "npc";
		entry.createEnd = static_cast<size_t>(it->position(0) + it->length(0));

		if (!resolveLuaName(content, (*it)[3].str(), entry.name)) {
			entry.name = nstr(filename.GetName());
		}
		entries.push_back(entry);
	}

	std::vector<CreatureType*> creatures;
	for (const LuaCreatureEntry& entry : entries) {
		std::string outfitTable;
		const std::regex directOutfitRegex("\\b" + entry.variable + R"lua(\s*:\s*outfit\s*\(\s*\{)lua", std::regex_constants::icase);
		bool hasOutfit = findLuaTableAfterPattern(content, directOutfitRegex, entry.createEnd, outfitTable);

		if (!hasOutfit) {
			const std::regex configOutfitRegex(entry.isNpc ? R"lua(\bnpcConfig\s*\.\s*outfit\s*=\s*\{)lua" : R"lua(\bmonster\s*\.\s*outfit\s*=\s*\{)lua", std::regex_constants::icase);
			hasOutfit = findLuaTableAfterPattern(content, configOutfitRegex, entry.createEnd, outfitTable);
		}

		if (!hasOutfit) {
			warnings.push_back("Lua creature \"" + wxstr(entry.name) + "\" has no supported outfit block.");
			continue;
		}

		CreatureType* creatureType = newd CreatureType();
		creatureType->name = entry.name;
		creatureType->isNpc = entry.isNpc;
		applyLuaOutfit(outfitTable, creatureType->outfit);

		if (!creatureType->isNpc && creatureType->outfit.lookType != 0 && g_gui.gfx.getCreatureSprite(creatureType->outfit.lookType) == nullptr) {
			warnings.push_back("Invalid creature \"" + wxstr(creatureType->name) + "\" look type #" + std::to_string(creatureType->outfit.lookType));
		}

		creatures.push_back(creatureType);
	}

	return creatures;
}

bool shouldSkipDirectoryImportFile(const wxString& path) {
	const wxString lower = path.Lower();
	return lower.Contains("\\lib\\") || lower.Contains("/lib/");
}

} // namespace

CreatureDatabase::CreatureDatabase() {
	////
}

CreatureDatabase::~CreatureDatabase() {
	clear();
}

void CreatureDatabase::clear() {
	for (CreatureMap::iterator iter = creature_map.begin(); iter != creature_map.end(); ++iter) {
		delete iter->second;
	}
	creature_map.clear();
}

CreatureType* CreatureDatabase::operator[](const std::string& name) {
	CreatureMap::iterator iter = creature_map.find(as_lower_str(name));
	if (iter != creature_map.end()) {
		return iter->second;
	}
	return nullptr;
}

CreatureType* CreatureDatabase::addMissingCreatureType(const std::string& name, bool isNpc) {
	assert((*this)[name] == nullptr);

	CreatureType* ct = newd CreatureType();
	ct->name = name;
	ct->isNpc = isNpc;
	ct->missing = true;
	ct->outfit.lookType = 130;

	creature_map.insert(std::make_pair(as_lower_str(name), ct));
	return ct;
}

CreatureType* CreatureDatabase::addCreatureType(const std::string& name, bool isNpc, const Outfit& outfit) {
	assert((*this)[name] == nullptr);

	CreatureType* ct = newd CreatureType();
	ct->name = name;
	ct->isNpc = isNpc;
	ct->missing = false;
	ct->outfit = outfit;

	creature_map.insert(std::make_pair(as_lower_str(name), ct));
	return ct;
}

bool CreatureDatabase::hasMissing() const {
	for (CreatureMap::const_iterator iter = creature_map.begin(); iter != creature_map.end(); ++iter) {
		if (iter->second->missing) {
			return true;
		}
	}
	return false;
}

bool CreatureDatabase::loadFromXML(const FileName& filename, bool standard, wxString& error, wxArrayString& warnings) {
	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(filename.GetFullPath().mb_str());
	if (!result) {
		error = "Couldn't open file \"" + filename.GetFullName() + "\", invalid format?";
		return false;
	}

	pugi::xml_node node = doc.child("creatures");
	if (!node) {
		error = "Invalid file signature, this file is not a valid creatures file.";
		return false;
	}

	for (pugi::xml_node creatureNode = node.first_child(); creatureNode; creatureNode = creatureNode.next_sibling()) {
		if (as_lower_str(creatureNode.name()) != "creature") {
			continue;
		}

		CreatureType* creatureType = CreatureType::loadFromXML(creatureNode, warnings);
		if (creatureType) {
			creatureType->standard = standard;
			if ((*this)[creatureType->name]) {
				warnings.push_back("Duplicate creature type name \"" + wxstr(creatureType->name) + "\"! Discarding...");
				delete creatureType;
			} else {
				creature_map[as_lower_str(creatureType->name)] = creatureType;
			}
		}
	}
	return true;
}

void CreatureDatabase::addImportedCreatureType(CreatureType* creatureType) {
	if (!creatureType) {
		return;
	}

	CreatureType* current = (*this)[creatureType->name];
	if (current) {
		CreatureBrush* currentBrush = current->brush;
		*current = *creatureType;
		current->brush = currentBrush;
		delete creatureType;
		return;
	}

	creature_map[as_lower_str(creatureType->name)] = creatureType;

	Tileset* tileSet = nullptr;
	if (creatureType->isNpc) {
		tileSet = g_materials.tilesets["NPCs"];
	} else {
		tileSet = g_materials.tilesets["Others"];
	}
	ASSERT(tileSet != nullptr);

	Brush* brush = newd CreatureBrush(creatureType);
	g_brushes.addBrush(brush);

	TilesetCategory* tileSetCategory = tileSet->getCategory(TILESET_CREATURE);
	tileSetCategory->brushlist.push_back(brush);
}

bool CreatureDatabase::importLuaFromOT(const FileName& filename, wxString& error, wxArrayString& warnings) {
	std::string content;
	if (!readTextFile(filename, content, error)) {
		return false;
	}

	std::vector<CreatureType*> creatures = loadCreatureTypesFromLua(filename, content, warnings);
	if (creatures.empty()) {
		error = "This is not valid OT npc/monster Lua data file.";
		return false;
	}

	for (CreatureType* creatureType : creatures) {
		addImportedCreatureType(creatureType);
	}
	return true;
}

bool CreatureDatabase::importDirectoryFromOT(const FileName& directory, wxString& error, wxArrayString& warnings) {
	if (!directory.DirExists()) {
		error = "Directory \"" + directory.GetFullPath() + "\" does not exist.";
		return false;
	}

	wxArrayString roots;
	FileName monstersDir;
	monstersDir.AssignDir(directory.GetFullPath());
	monstersDir.AppendDir("monsters");
	FileName npcDir;
	npcDir.AssignDir(directory.GetFullPath());
	npcDir.AppendDir("npc");

	if (monstersDir.DirExists()) {
		roots.Add(monstersDir.GetFullPath());
	}
	if (npcDir.DirExists()) {
		roots.Add(npcDir.GetFullPath());
	}
	if (roots.empty()) {
		roots.Add(directory.GetFullPath());
	}

	wxArrayString files;
	for (uint32_t i = 0; i < roots.GetCount(); ++i) {
		wxDir::GetAllFiles(roots[i], &files, "*.xml", wxDIR_FILES);
		wxDir::GetAllFiles(roots[i], &files, "*.lua", wxDIR_FILES);
	}

	uint32_t imported = 0;
	uint32_t skipped = 0;
	for (uint32_t i = 0; i < files.GetCount(); ++i) {
		if (shouldSkipDirectoryImportFile(files[i])) {
			continue;
		}

		wxString fileError;
		wxArrayString fileWarnings;
		if (importXMLFromOT(FileName(files[i]), fileError, fileWarnings)) {
			++imported;
			for (uint32_t j = 0; j < fileWarnings.GetCount(); ++j) {
				warnings.push_back(fileWarnings[j]);
			}
		} else {
			++skipped;
		}
	}

	if (imported == 0) {
		error = "No supported monster/npc XML or Lua files were found in \"" + directory.GetFullPath() + "\".";
		return false;
	}
	if (skipped > 0) {
		warnings.push_back("Skipped " + std::to_string(skipped) + " files that were not supported monster/npc data.");
	}
	return true;
}

bool CreatureDatabase::importXMLFromOT(const FileName& filename, wxString& error, wxArrayString& warnings) {
	if (filename.DirExists()) {
		return importDirectoryFromOT(filename, error, warnings);
	}

	if (filename.GetExt().CmpNoCase("lua") == 0) {
		return importLuaFromOT(filename, error, warnings);
	}

	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(filename.GetFullPath().mb_str());
	if (!result) {
		error = "Couldn't open file \"" + filename.GetFullName() + "\", invalid format?";
		return false;
	}

	pugi::xml_node node;
	if ((node = doc.child("monsters"))) {
		for (pugi::xml_node monsterNode = node.first_child(); monsterNode; monsterNode = monsterNode.next_sibling()) {
			if (as_lower_str(monsterNode.name()) != "monster") {
				continue;
			}

			pugi::xml_attribute attribute;
			if (!(attribute = monsterNode.attribute("file"))) {
				continue;
			}

			FileName monsterFile(filename);
			monsterFile.SetFullName(wxString(attribute.as_string(), wxConvUTF8));

			pugi::xml_document monsterDoc;
			pugi::xml_parse_result monsterResult = monsterDoc.load_file(monsterFile.GetFullPath().mb_str());
			if (!monsterResult) {
				continue;
			}

			CreatureType* creatureType = CreatureType::loadFromOTXML(monsterFile, monsterDoc, warnings);
			if (creatureType) {
				addImportedCreatureType(creatureType);
			}
		}
	} else if ((node = doc.child("monster")) || (node = doc.child("npc"))) {
		CreatureType* creatureType = CreatureType::loadFromOTXML(filename, doc, warnings);
		if (creatureType) {
			addImportedCreatureType(creatureType);
		}
	} else {
		error = "This is not valid OT npc/monster data file.";
		return false;
	}
	return true;
}

bool CreatureDatabase::saveToXML(const FileName& filename) {
	pugi::xml_document doc;

	pugi::xml_node decl = doc.prepend_child(pugi::node_declaration);
	decl.append_attribute("version") = "1.0";

	pugi::xml_node creatureNodes = doc.append_child("creatures");
	for (const auto& creatureEntry : creature_map) {
		CreatureType* creatureType = creatureEntry.second;
		if (!creatureType->standard) {
			pugi::xml_node creatureNode = creatureNodes.append_child("creature");

			creatureNode.append_attribute("name") = creatureType->name.c_str();
			creatureNode.append_attribute("type") = creatureType->isNpc ? "npc" : "monster";

			const Outfit& outfit = creatureType->outfit;
			creatureNode.append_attribute("looktype") = outfit.lookType;
			creatureNode.append_attribute("lookitem") = outfit.lookItem;
			creatureNode.append_attribute("lookmount") = outfit.lookMount;
			creatureNode.append_attribute("lookaddon") = outfit.lookAddon;
			creatureNode.append_attribute("lookhead") = outfit.lookHead;
			creatureNode.append_attribute("lookbody") = outfit.lookBody;
			creatureNode.append_attribute("looklegs") = outfit.lookLegs;
			creatureNode.append_attribute("lookfeet") = outfit.lookFeet;
			creatureNode.append_attribute("lookmounthead") = outfit.lookMountHead;
			creatureNode.append_attribute("lookmountbody") = outfit.lookMountBody;
			creatureNode.append_attribute("lookmountlegs") = outfit.lookMountLegs;
			creatureNode.append_attribute("lookmountfeet") = outfit.lookMountFeet;
		}
	}
	return doc.save_file(filename.GetFullPath().mb_str(), "\t", pugi::format_default, pugi::encoding_utf8);
}
