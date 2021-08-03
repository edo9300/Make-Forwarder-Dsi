#include <fstream>
#include <vector>
#include <cstring>
#include <nds.h>
#include <cstdio>
#include "apppatch.h"
#include "headers.h"

class CRC16 {
	std::vector<uint16_t>crc16_tab;
	uint16_t crc16_constant = 0xA001;
	bool mdflag;
public:
	CRC16(bool modbus_flag = false) {
		// initialize the precalculated tables
		if(crc16_tab.empty()) {
			init_crc16();
		}
		mdflag = modbus_flag;
	}

	uint16_t calculate(std::string input_data) {
		uint16_t crc_value = mdflag ? 0xffff : 0x0000;

		for(auto c : input_data) {
			uint16_t tmp = crc_value ^ c;
			uint16_t rotated = crc_value >> 8;
			crc_value = rotated ^ crc16_tab[(tmp & 0x00ff)];
		}

		return crc_value;
	}

	void init_crc16() {
		for(int i = 0; i < 256; i++) {
			uint16_t crc = i;
			for(int j = 0; j < 8; j++) {
				if(crc & 0x0001)
					crc = (crc >> 1) ^ crc16_constant;
				else
					crc = crc >> 1;
			}
			crc16_tab.push_back(crc);
		}
	}
};

int32_t GetBanerSize(uint16_t banner_type) {
	switch(banner_type) {
		case NDS_BANNER_VER_ZH:
		{
			return NDS_BANNER_SIZE_ZH;
			break;
		}
		case NDS_BANNER_VER_ZH_KO:
		{
			return NDS_BANNER_SIZE_ZH_KO;
			break;
		}
		case NDS_BANNER_VER_DSi:
		{
			return NDS_BANNER_SIZE_DSi;
			break;
		}
		default:
			return NDS_BANNER_SIZE_ORIGINAL;
	}
}
void ReplaceBanner(const std::string& target, const std::string& input, const std::string& output, const std::string& gameidhex) {
	std::string destination = target;
	std::ifstream infile(input, std::ifstream::binary);
	if(!output.empty()) {
		std::ifstream  src(target, std::ios::binary);
		std::ofstream  dst(output, std::ios::binary);
		dst << src.rdbuf();
		src.close();
		dst.close();
		destination = output;
	}

	SrlHeader isrlheader;
	infile.read((char*)&isrlheader, sizeof(SrlHeader));

	TwlHeader itwlheader = { 0 };
	if(isrlheader.rom_header_size >= 0x300) {
		infile.read((char*)&itwlheader, sizeof(SrlHeader));
	}
	infile.seekg(0, infile.beg);
	infile.seekg(isrlheader.banner_offset, infile.beg);
	int16_t ibanner_type = 0;
	infile.read((char*)&ibanner_type, sizeof(int16_t));
	infile.seekg(isrlheader.banner_offset, infile.beg);
	sNDSBannerExt ibanner;
	infile.read((char*)&ibanner, GetBanerSize(ibanner_type));
	std::fstream trfile(destination, std::fstream::binary | std::fstream::out | std::fstream::in);

	SrlHeader tsrlheader;
	trfile.read((char*)&tsrlheader, sizeof(SrlHeader));

	TwlHeader ttwlheader;
	trfile.read((char*)&ttwlheader, sizeof(SrlHeader));

	trfile.seekg(tsrlheader.banner_offset, trfile.beg);
	int16_t tbanner_type = 0;
	trfile.read((char*)&tbanner_type, sizeof(int16_t));
	trfile.seekg(tsrlheader.banner_offset, trfile.beg);
	{
		int32_t diff = GetBanerSize(ibanner_type) - GetBanerSize(tbanner_type);
		std::ofstream  tmp("tmp.nds", std::ios::binary);
		trfile.seekg(0, trfile.beg);
		std::string tmpstr;
		tmpstr.resize(tsrlheader.banner_offset);
		trfile.read(&tmpstr[0], tsrlheader.banner_offset);
		tmp.write(&tmpstr[0], tsrlheader.banner_offset);
		tmp.write((char*)&ibanner, GetBanerSize(ibanner_type));
		int32_t start = tsrlheader.banner_offset + GetBanerSize(tbanner_type);
		trfile.seekg(0, trfile.end);
		int32_t length = trfile.tellg();
		trfile.seekg(0, trfile.beg);
		trfile.seekg(start);
		tmpstr.clear();
		tmpstr.resize(length - start);
		trfile.read(&tmpstr[0], length - start);
		tmp.write(&tmpstr[0], length - start);
		tmp.close();
		trfile.close();
		remove(destination.c_str());
		rename("tmp.nds", destination.c_str());
		trfile.open(destination, std::fstream::binary | std::fstream::out | std::fstream::in);
		tsrlheader.application_end_offset += diff;
		ttwlheader.dsi9_rom_offset += diff;
		ttwlheader.dsi7_rom_offset += diff;
		ttwlheader.total_rom_size += diff;
	}
	tsrlheader.gamecode[0] = isrlheader.gamecode[0];
	tsrlheader.gamecode[1] = isrlheader.gamecode[1];
	tsrlheader.gamecode[2] = isrlheader.gamecode[2];
	tsrlheader.gamecode[3] = stoi(gameidhex.substr(6, 2), 0, 16);
	ttwlheader.tid_low = tsrlheader.gamecode[3] | (tsrlheader.gamecode[2] << 8) | (tsrlheader.gamecode[1] << 16) | (tsrlheader.gamecode[0] << 24);
	trfile.seekg(0, trfile.beg);
	trfile.write((char*)&tsrlheader, sizeof(tsrlheader));
	trfile.write((char*)&ttwlheader, sizeof(ttwlheader));
	trfile.close();
	infile.close();
}

/*
Straight port of ahezard's python script to patch the header for dsiwares apps
*/
void Patch(const std::string& name, bool backup) {
	if(backup) {
		//make a backup of the nds
		std::ifstream  src(name, std::ios::binary);
		std::ofstream  dst((name + ".orig.nds"), std::ios::binary);
		dst << src.rdbuf();
		src.close();
		dst.close();
	}
	std::fstream infile(name, std::fstream::binary | std::fstream::out | std::fstream::in);
	infile.seekg(0, infile.beg);
	SrlHeader header;
	infile.read((char*)&header, sizeof(SrlHeader));
	header.dsi_flags = 0x01;
	header.nds_region = 0x00;
	header.unitcode = 0x03;
	CRC16 crc(true);
	std::string aa;
	aa.resize(0x15E);
	memcpy(&aa[0], (char*)&header, 0x15E);
	auto headercrc = crc.calculate(aa);
	header.header_crc = headercrc;

	TwlHeader header2;
	infile.read((char*)&header2, sizeof(TwlHeader));

	header2.access_control = 0x00000138;
	header2.scfg_ext_mask = 0x80040000;
	memset(header2.offset_0x1BC, 0, sizeof(header2.offset_0x1BC));
	header2.appflags = 0;

	SrlSignedHeader header3 = { 0 };

	if(header.rom_header_size >= 0x1100) {
		infile.read((char*)&header3, sizeof(SrlSignedHeader));
	}

	infile.seekg(0x4000, infile.beg);
	memset(header3.hmac_arm7, 0xff, sizeof(header3.hmac_arm7));
	memset(header3.hmac_arm7i, 0xff, sizeof(header3.hmac_arm7i));

	memset(header3.hmac_arm9, 0xff, sizeof(header3.hmac_arm9));
	memset(header3.hmac_arm9i, 0xff, sizeof(header3.hmac_arm9i));
	memset(header3.hmac_arm9_no_secure, 0xff, sizeof(header3.hmac_arm9_no_secure));

	memset(header3.hmac_icon_title, 0xff, sizeof(header3.hmac_icon_title));
	memset(header3.hmac_digest_master, 0xff, sizeof(header3.hmac_digest_master));
	memset(header3.rsa_signature, 0xff, sizeof(header3.rsa_signature));

	auto arm9FooterAddr = header.arm9_rom_offset + header.arm9_size;
	bool writefooter = false;

	ARM9Footer footer;

	infile.seekg(arm9FooterAddr, infile.beg);
	infile.read((char*)&footer, sizeof(ARM9Footer));

	if(footer.nitrocode != 0xDEC00621) {
		footer.nitrocode = 0xDEC00621;
		footer.versionInfo = 0xad8;
		footer.reserved = 0;
		writefooter = true;
	}


	infile.seekg(0, infile.beg);
	infile.write((char*)&header, sizeof(SrlHeader));
	infile.write((char*)&header2, sizeof(TwlHeader));
	infile.write((char*)&header3, 0xC80);
	std::string dummy;
	dummy.resize(16 * 8);
	std::fill_n(&dummy[0], 16 * 8, 0xff);
	infile.write(&dummy[0], 16 * 8);

	if(writefooter) {
		infile.seekg(arm9FooterAddr, infile.beg);
		infile.write((char*)&footer, sizeof(ARM9Footer));
	}
	infile.close();
}

int PathStringReplace(std::string path) {
	std::fstream target("sd:/MakeForwarder/banner.nds", std::fstream::binary | std::fstream::out | std::fstream::in);
	if(!target.is_open())
		return 255;
	std::string str((std::istreambuf_iterator<char>(target)),
					std::istreambuf_iterator<char>());
	std::size_t found = str.find("sd:/kkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkk");
	if(found == std::string::npos)
		return 5;
	target.seekg(found + 4, target.beg);
	target.write(&path[0], path.size());
	target.put('\0');
	return 0;
}