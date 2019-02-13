/*---------------------------------------------------------------------------------

	Basic template code for starting a DS app

---------------------------------------------------------------------------------*/
#include <nds.h>
#include <cstdio>
#include <fstream>
#include <vector>
#include <unistd.h>
#include <fat.h>

#include "file_browse.h"
#include "headers.h"
#include "utils.h"
#include "menu.h"
#include "inifile.h"
#include "apppatch.h"

CIniFile bootsrtapconfig;
CIniFile bootstrap_template;

PrintConsole upperScreen;
PrintConsole lowerScreen;

void WriteMessage(std::string text, bool clear = false, PrintConsole* screen = nullptr) {
	if(screen)
		consoleSelect(screen);
	if(clear)
		consoleClear();
	if(text.size() <= DISPLAY_COLUMNS) {
		iprintf(text.c_str());
		return;
	}
	std::vector<std::string> words;
	std::string temp;
	for(int i = 0; i < (int)text.size(); i++) {
		if(text[i] == ' ' || text[i] == '\n') {
			words.push_back(temp);
			temp.clear();
			if(text[i] == '\n')
				words.push_back("\n");
		} else
			temp += text[i];
	}
	if(temp.size())
		words.push_back(temp);
	int column = 0;
	for(auto word : words) {
		if(word.size() == 1 && word[0] == '\n') {
			if(column != DISPLAY_COLUMNS)
				iprintf("\n");
			column = 0;
			return;
		}
		int chkval = column + (int)word.size();
		if(column)
			chkval++;
		if(chkval <= DISPLAY_COLUMNS) {
			if(column) {
				iprintf(" ");
				column++;
			}
			iprintf(word.c_str());
			column += (int)word.size();
		} else {
			if(column != DISPLAY_COLUMNS)
				iprintf("\n");
			column = (int)word.size();
			iprintf(word.c_str());
		}
	}
}

menu yesno;

void displayInit() {
	lowerScreen = *consoleDemoInit();
	videoSetMode(MODE_0_2D);
	vramSetBankA(VRAM_A_MAIN_BG);
	consoleInit(&upperScreen, 3, BgType_Text4bpp, BgSize_T_256x256, 31, 0, true, true);
}

bool fileExists(const std::string& file) {
    struct stat buf;
    return (stat(file.c_str(), &buf) == 0);
}

std::string ReplaceAll(std::string str, const std::string& from, const std::string& to) {
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
    return str;
}
void stop (void) {
//---------------------------------------------------------------------------------
	while (1) {
		swiWaitForVBlank();
	}
}
void wait (void) {
//---------------------------------------------------------------------------------
	while (1) {
		swiWaitForVBlank();
		scanKeys();
		int pressed = keysDown();
		if(pressed & KEY_A) break;
	}
}


const char* GetErrorString(int code) {
	switch(code){
	case 1:
		return "fatInitDefault failed!";
	case 2:
		return "The MakeForwarder folder is missing!";
	case 3:
		return "Template.nds is missing from the MakeForwarder folder!";
	case 4:
		return "Error when reading the template.nds file, make sure to have the correct one!";
	case 5:
		return "Couldn't open the target rom, or the rom is not a valid target!";
	default:
		return "";
	}
}




void PrintError(int errorcode, bool halt = false) {
	consoleSetWindow(&upperScreen, 0, 3, DISPLAY_COLUMNS, 23);
	consoleSelect(&upperScreen);
	WriteMessage(GetErrorString(errorcode), true);
	if (halt)
		stop();
	else {
		WriteMessage("Press A to continue", false);
		wait();
		consoleClear();
	}
}

std::string GetHexid(std::string file, int* chk) {
	std::ifstream infile(file, std::ifstream::binary);
	infile.seekg(0xc);
	if(infile.tellg()!=0xc) {
		*chk = 1;
		return "";
	}
	std::string gameid;
	gameid.resize(4);
	infile.read(&gameid[0], 4);
	infile.close();
	return string_to_hex(gameid);
}

void CreateForwarder() {
	std::vector<std::string> extensionList={".nds"};
	WriteMessage("Select the target rom", true, &upperScreen);
	consoleSelect(&lowerScreen);
	std::string file = browseForFile(extensionList);
	chdir("sd:/");
	
	int chk = 0;
	std::string gameidhex = GetHexid(file, &chk);
	if (chk){
		PrintError(5);
		return;
	}
	std::string folderpath("sd:/title/00030004/" + gameidhex);
	
	if(fileExists(folderpath + "/content/00000000.app")) {
		WriteMessage("A DsiWare with the same id already exists, do you want to overwrite it?", true, &upperScreen);
		int ret = yesno.DoMenu(&lowerScreen);
		consoleSelect(&upperScreen);
		consoleClear();
		if(ret)
			return;
	}
	
	WriteMessage("Creating forwarder", true, &lowerScreen);
	
	ReplaceBanner("sd:/MakeForwarder/template.nds", file, "sd:/MakeForwarder/banner.nds");
	
	Patch("sd:/MakeForwarder/banner.nds", false);

	MakeTmd("sd:/MakeForwarder/banner.nds", "sd:/MakeForwarder/title.tmd");
	
	chk = PathStringReplace("title/00030004/" + gameidhex + "/data/");
	
	if(chk) {
		PrintError(chk);
		remove("sd:/MakeForwarder/banner.nds");
		remove("sd:/MakeForwarder/title.tmd");
		return;
	}
	
	if(CreatePath("title/00030004/" + gameidhex + "/data", "sd:/") && CreatePath("title/00030004/" + gameidhex + "/content", "sd:/")) {
		Movefile("sd:/MakeForwarder/banner.nds", folderpath + "/content/00000000.app");
		Movefile("sd:/MakeForwarder/title.tmd", folderpath + "/content/title.tmd");
		if(bootstrap_template.HasFileHandle()) {
			bootsrtapconfig.SaveIniFile((folderpath + "/data/config.ini").c_str());
			bootstrap_template.SetString( "NDS-BOOTSTRAP", "NDS_PATH", file.c_str());
			std::string savePath = ReplaceAll(file, ".nds", ".sav");
			bootstrap_template.SetString( "NDS-BOOTSTRAP", "SAV_PATH", savePath.c_str());
			bootstrap_template.SaveIniFile((folderpath + "/data/bootstrap.ini").c_str());
		}
	}
}

void SetBootstrap() {
	std::vector<std::string> extensionList={".nds"};
	WriteMessage("Select the target bootstrap file to be used", true, &upperScreen);
	consoleSelect(&lowerScreen);
	std::string file = browseForFile(extensionList);
	chdir("sd:/");
	
	size_t found  = file.find_last_of("/");
	
	std::string bootstrappath=file.substr(0,found+1);
	std::string bootstrapversion=file.substr(found+1);
	
	bootsrtapconfig.SetString( "NDS-FORWARDER", "BOOTSTRAP_PATH", bootstrappath.c_str());
	bootsrtapconfig.SetString( "NDS-FORWARDER", "BOOTSTRAP_VERSION", bootstrapversion.c_str());
	
	bootstrap_template.LoadIniFile((bootstrappath+"nds-bootstrap.ini").c_str());
}

void CheckResources() {
	if (!fileExists("sd:/MakeForwarder"))
		PrintError(2, true);
	if (!fileExists("sd:/MakeForwarder/template.nds"))
		PrintError(3, true);
	std::ifstream ndstemplate("sd:/MakeForwarder/template.nds", std::ifstream::binary);
	std::string str((std::istreambuf_iterator<char>(ndstemplate)),
		std::istreambuf_iterator<char>());
	std::size_t found = str.find("sd:/kkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkk");
	if(found == std::string::npos)
		PrintError(4, true);
}

int main() {
	displayInit();
	consoleSetWindow(&upperScreen, 0, 0, DISPLAY_COLUMNS, 3);
	WriteMessage("Forwarder maker by edo9300 v1.0", true, &upperScreen);
	consoleSetWindow(&upperScreen, 0, 3, DISPLAY_COLUMNS, 23);
	if (!fatInitDefault())
		PrintError(1, true);
	CheckResources();
	menu mainmenu;
	mainmenu.AddOption("Create Forwarder");
	mainmenu.AddOption("Set target bootstrap");
	yesno.AddOption("Yes");
	yesno.AddOption("No");
	while(1){
		swiWaitForVBlank();
		WriteMessage("Use the option \"Set target bootstrap\" to avoid configuring the created forwarders at startup", true, &upperScreen);
		int ret = mainmenu.DoMenu(&lowerScreen);
		if (ret==0)
			CreateForwarder();
		else if (ret==1)
			SetBootstrap();
		else
			break;
	}
	stop();
	return 0;
}
