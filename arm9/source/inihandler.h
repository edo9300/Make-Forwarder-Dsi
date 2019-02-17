/*
	inihandler.h
	Copyright (C) 2019 edo9300

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _INIHANDLER_H_
#define _INIHANDLER_H_
#include <string>
#include <map>

typedef std::map<std::string/*field*/, std::map<std::string /*key*/, std::string /*val*/>> ini_map;

class IniFile {
public:
	IniFile():is_from_file(false), modified(false){};
	IniFile(const std::string& filename) :is_from_file(false), modified(false) {
		LoadFile(filename);
	};
	bool LoadFile(const std::string& filename);
	bool SaveFile(const std::string& filename = "");
	bool SaveFileIfModified(const std::string& filename = "") {
		return modified ? SaveFile(filename) : true;
	};
	void SetValue(const std::string& field, const std::string& key, const std::string& _val);
	void SetValue(const std::string& field, const std::string& key, int _val) {
		SetValue(field, key, std::to_string(_val));
	};
	std::string GetValueString(const std::string& field, const std::string& key, const std::string& _default = "");
	int GetValueInt(const std::string& field, const std::string& key, int _default = 0);
	bool HasFileHandle() {
		return is_from_file;
	};
	ini_map::iterator begin() {
		return contents.begin();
	};
	ini_map::iterator end() {
		return contents.end();
	};
private:
	ini_map contents;
	std::string cur_filename;
	bool is_from_file;
	bool modified;
};

#endif