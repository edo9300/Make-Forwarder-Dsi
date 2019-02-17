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
#include <cstdio>

static bool fgetline(FILE* f, std::string& str) {
	str.clear();
	char p = 0;
	size_t read = 0;
	while((read = fread(&p, 1, 1, f))) {
		if(p == '\n' || p == '\r')
			break;
		str += p;
	}
	return !!read;
}

bool IniFile::LoadFile(const std::string & filename) {
	FILE* f = fopen(filename.c_str(), "rb");
	if(!f)
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
	while(fgetline(f, cur_str)) {
		if(cur_str.empty())
			continue;
		auto pos = cur_str.find('[');
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
		contents[cur_field][key] = val;
	}
	fclose(f);
	return true;
}

bool IniFile::SaveFile(const std::string & filename) {
	if(filename != "")
		cur_filename = filename;
	FILE* inifile = fopen(filename.c_str(), "wb");
	if(!inifile)
		return false;
	modified = false;
	for(auto& content : contents) {
		fprintf(inifile, "[%s]\n", content.first.c_str());
		for(auto& obj : content.second) {
			fprintf(inifile, "%s = %s\n", obj.first.c_str(), obj.second.c_str());
		}
	}
	fclose(inifile);
	return true;
}

void IniFile::SetValue(const std::string& field, const std::string& key, const std::string& _val) {
	modified = true;
	contents[field][key] = _val;
}

std::string IniFile::GetValueString(const std::string & field, const std::string & key, const std::string & _default) {
	if(contents.find(field) != contents.end())
		return contents[field][key];
	return _default;
}

int IniFile::GetValueInt(const std::string & field, const std::string & key, int _default) {
	if(contents.find(field) != contents.end())
		return std::stoi(contents[field][key]);
	return _default;
}
