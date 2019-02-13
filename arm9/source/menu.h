#ifndef MENU_H
#define MENU_H

#include <string>
#include <vector>

#define DISPLAY_COLUMNS 32
#define DISPLAY_ROWS 26

class menu{
	private:
	std::vector<std::string> options;
	int current;
	public:
	void AddOption(const std::string& option);
	int DoMenu(PrintConsole* screen);
	int GetLastSeletedOption();
	
};



#endif //MENU_H
