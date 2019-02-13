#ifndef UTILS_H
#define UTILS_H
#include <fstream>
#include <vector>

#include <errno.h>
#include <sys/stat.h>

std::string string_to_hex(const std::string& input) {
	static const char* const lut = "0123456789ABCDEF";
	size_t len = input.length();

	std::string output;
	output.reserve(2 * len);
	for(size_t i = 0; i < len; ++i) {
		const unsigned char c = input[i];
		output.push_back(lut[c >> 4]);
		output.push_back(lut[c & 15]);
	}
	return output;
}
bool Makedirectory(const std::string& path) {
	return !mkdir((const char *)path.c_str(), 0777) || errno == EEXIST;
}
bool Movefile(const std::string& source, const std::string& destination) {
	std::ifstream  src(source, std::ios::binary);
	if(!src.is_open())
		return false;
	std::ofstream  dst(destination, std::ios::binary);
	if(!dst.is_open())
		return false;
	dst << src.rdbuf();
	src.close();
	remove(source.c_str());
	return true;
}
bool CreatePath(const std::string& path, const std::string& workingdir = "") {
	std::vector<std::string> folders;
	std::string temp;
	for(int i = 0; i < (int)path.size(); i++) {
		if(path[i] == '/') {
			folders.push_back(temp);
			temp.clear();
		} else
			temp += path[i];
	}
	if(!temp.empty())
		folders.push_back(temp);
	temp.clear();
	for(auto folder : folders) {
		if(temp.empty() && !workingdir.empty())
			temp = workingdir + "/" + folder;
		else
			temp += "/" + folder;
		if(!Makedirectory(temp))
			return false;
	}
	return true;
}

#endif