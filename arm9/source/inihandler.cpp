/*
	inihandler.cpp
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

#include "inihandler.h"
#include <fstream>

bool IniFile::LoadFile(const std::string & filename) {
	std::ifstream inifile(filename, std::ifstream::in);
	if(!inifile.is_open())
		return false;
	is_from_file = true;
	cur_filename = filename;
	std::string cur_str;
	std::string cur_field("");
	auto remove_trailing_space = [](const std::string& str) {
		auto pos = str.find_last_of(" ");
		if(pos != std::string::npos)
			return str.substr(0, pos);
		return str;

	};
	auto remove_leading_space = [](const std::string& str) {
		auto pos = str.find_first_of(" ");
		if(pos != std::string::npos)
			return str.substr(pos + 1);
		return str;

	};
	while(std::getline(inifile, cur_str)) {
		auto pos = cur_str.find_first_of("\n\r");
		if(cur_str.size() && pos != std::string::npos)
			cur_str = cur_str.substr(0, pos);
		pos = cur_str.find('[');
		if(pos != std::string::npos) {
			auto endpos = cur_str.find(']');
			cur_field = cur_str.substr(pos + 1, endpos - pos - 1);
			continue;
		}
		if(cur_field == "")
			continue;
		pos = cur_str.find('=');
		if(pos == std::string::npos)
			continue;
		auto key = remove_trailing_space(cur_str.substr(0, pos));
		auto val = remove_leading_space(cur_str.substr(pos + 1, cur_str.size() - pos));
		contents[cur_field].Set(key, val);
	}
	return true;
}

bool IniFile::SaveFile(const std::string & filename) {
	if(filename != "")
		cur_filename = filename;
	std::ofstream inifile(cur_filename, std::ofstream::out);
	if(!inifile.is_open())
		return false;
	modified = false;
	for(auto& content : contents) {
		inifile << "[" << content.first << "]\n";
		for(auto& obj : content.second.GetHandle()) {
			inifile << obj.first << " = " << obj.second << "\n";
		}
	}
	return true;
}

void IniFile::SetValue(const std::string& field, const std::string& key, const std::string& _val) {
	modified = true;
	contents[field].Set(key, _val);
}

std::string IniFile::GetValueString(const std::string & field, const std::string & key, const std::string & _default) {
	if(contents.find(key) != contents.end())
		return contents[field].GetValString(key);
	return _default;
}

int IniFile::GetValueInt(const std::string & field, const std::string & key, int _default) {
	if(contents.find(key) != contents.end())
		return contents[field].GetValInt(key);
	return _default;
}
