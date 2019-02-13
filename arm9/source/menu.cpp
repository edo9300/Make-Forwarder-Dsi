/*---------------------------------------------------------------------------------

	Basic template code for starting a DS app

---------------------------------------------------------------------------------*/
#include <nds.h>
#include <cstdio>
#include <vector>
#include <string>
#include "menu.h"

void menu::AddOption(const std::string& option){
	options.push_back(option);
}
int menu::DoMenu(PrintConsole* screen){
	consoleSelect(screen);
	current = 0;
	consoleClear();
	consoleSetWindow(screen, 1, 0, DISPLAY_COLUMNS-1, DISPLAY_ROWS);
	for(auto option : options)
		iprintf((option+"\n").c_str());
	consoleSetWindow(screen, 0, 0, 1, DISPLAY_ROWS);
	iprintf(">");
	while (true){
		swiWaitForVBlank();
		scanKeys();
		int pressed = keysDownRepeat();
		if(pressed & KEY_UP) {
			consoleClear();
			current--;
			if(current<0)
				current = options.size() - 1;
			for (int a = 0; a<current; a++)
				iprintf("\n");
			iprintf(">");
		}
		if(pressed & KEY_DOWN) {
			consoleClear();
			current++;
			if(current>=options.size())
				current = 0;
			for (int a = 0; a<current; a++)
				iprintf("\n");
			iprintf(">");
		}
		if(pressed & KEY_A) {
			consoleSetWindow(screen, 0, 0, DISPLAY_COLUMNS, DISPLAY_ROWS);
			consoleClear();
			return current;
		}
		if(pressed & KEY_START) {
			consoleSetWindow(screen, 0, 0, DISPLAY_COLUMNS, DISPLAY_ROWS);
			consoleClear();
			current = -1;
			return current;
		}
	}
}
int menu::GetLastSeletedOption(){
	return current;
}
