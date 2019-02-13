#ifndef APPPATCH_H
#define APPPATCH_H

#include <string>

void ReplaceBanner(const std::string& target, const std::string& input, const std::string& output);
void Patch(const std::string& name, bool backup);
void MakeTmd(const std::string& target, const std::string& destination = "");
int PathStringReplace(std::string path);

#endif //APPPATCH_H