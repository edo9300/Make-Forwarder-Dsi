#ifndef HEADERS
#define HEADERS

struct SrlHeader {
	int8_t title[0xC];
	int8_t gamecode[0x4];
	int8_t makercode[2];
	uint8_t unitcode;							// product code. 0=NDS, 2=NDS+DSi, 3=DSi
	uint8_t devicetype;						// device code. 0 = normal
	uint8_t devicecap;						// device size. (1<<n Mbit)
	uint8_t reserved1[0x7];					// 0x015..0x01D
	uint8_t dsi_flags;
	uint8_t nds_region;
	uint8_t romversion;
	uint8_t reserved2;						// 0x01F
	uint32_t arm9_rom_offset;					// points to libsyscall and rest of ARM9 binary
	uint32_t arm9_entry_address;
	uint32_t arm9_ram_address;
	uint32_t arm9_size;
	uint32_t arm7_rom_offset;
	uint32_t arm7_entry_address;
	uint32_t arm7_ram_address;
	uint32_t arm7_size;
	uint32_t fnt_offset;
	uint32_t fnt_size;
	uint32_t fat_offset;
	uint32_t fat_size;
	uint32_t arm9_overlay_offset;
	uint32_t arm9_overlay_size;
	uint32_t arm7_overlay_offset;
	uint32_t arm7_overlay_size;
	uint32_t rom_control_info1;					// 0x00416657 for OneTimePROM
	uint32_t rom_control_info2;					// 0x081808F8 for OneTimePROM
	uint32_t banner_offset;
	uint16_t secure_area_crc;
	uint16_t rom_control_info3;				// 0x0D7E for OneTimePROM
	uint32_t offset_0x70;						// magic1 (64 bit encrypted magic code to disable LFSR)
	uint32_t offset_0x74;						// magic2
	uint32_t offset_0x78;						// unique ID for homebrew
	uint32_t offset_0x7C;						// unique ID for homebrew
	uint32_t application_end_offset;			// rom size
	uint32_t rom_header_size;
	uint32_t offset_0x88;						// reserved... ?
	uint32_t offset_0x8C;

	// reserved
	uint32_t offset_0x90;
	uint32_t offset_0x94;
	uint32_t offset_0x98;
	uint32_t offset_0x9C;
	uint32_t offset_0xA0;
	uint32_t offset_0xA4;
	uint32_t offset_0xA8;
	uint32_t offset_0xAC;
	uint32_t offset_0xB0;
	uint32_t offset_0xB4;
	uint32_t offset_0xB8;
	uint32_t offset_0xBC;

	uint8_t logo[156];						// character data
	uint16_t logo_crc;
	uint16_t header_crc;

	// 0x160..0x17F reserved
	uint32_t debug_rom_offset;
	uint32_t debug_size;
	uint32_t debug_ram_address;
	uint32_t offset_0x16C;
	uint8_t zero[0x10];
};

struct TwlHeader {
	// DSi extended stuff below
	uint8_t global_mbk_setting[5][4];
	uint32_t arm9_mbk_setting[3];
	uint32_t arm7_mbk_setting[3];
	uint32_t mbk9_wramcnt_setting;

	uint32_t region_flags;
	uint32_t access_control;
	uint32_t scfg_ext_mask;
	uint8_t offset_0x1BC[3];
	uint8_t appflags;

	uint32_t dsi9_rom_offset;
	uint32_t offset_0x1C4;
	uint32_t dsi9_ram_address;
	uint32_t dsi9_size;
	uint32_t dsi7_rom_offset;
	uint32_t offset_0x1D4;
	uint32_t dsi7_ram_address;
	uint32_t dsi7_size;

	uint32_t digest_ntr_start;
	uint32_t digest_ntr_size;
	uint32_t digest_twl_start;
	uint32_t digest_twl_size;
	uint32_t sector_hashtable_start;
	uint32_t sector_hashtable_size;
	uint32_t block_hashtable_start;
	uint32_t block_hashtable_size;
	uint32_t digest_sector_size;
	uint32_t digest_block_sectorcount;

	uint32_t banner_size;
	uint32_t offset_0x20C;
	uint32_t total_rom_size;
	uint32_t offset_0x214;
	uint32_t offset_0x218;
	uint32_t offset_0x21C;

	uint32_t modcrypt1_start;
	uint32_t modcrypt1_size;
	uint32_t modcrypt2_start;
	uint32_t modcrypt2_size;

	uint32_t tid_low;
	uint32_t tid_high;
	uint32_t public_sav_size;
	uint32_t private_sav_size;
	uint8_t reserved3[176];
	uint8_t age_ratings[0x10];
};
struct SrlSignedHeader {
	uint8_t hmac_arm9[20];
	uint8_t hmac_arm7[20];
	uint8_t hmac_digest_master[20];
	uint8_t hmac_icon_title[20];
	uint8_t hmac_arm9i[20];
	uint8_t hmac_arm7i[20];
	uint8_t reserved4[40];
	uint8_t hmac_arm9_no_secure[20];
	uint8_t reserved5[2636];
	uint8_t debug_args[0x180];
	uint8_t rsa_signature[0x80];
};
struct ARM9Footer {
	uint32_t nitrocode;
	uint32_t versionInfo;
	uint32_t reserved;
};

struct sNDSBannerExt {
	uint16_t version;		//!< version of the banner.
	uint16_t crc[4];		//!< CRC-16s of the banner.
	uint8_t reserved[22];
	uint8_t icon[512];		//!< 32*32 icon of the game with 4 bit per pixel.
	uint16_t palette[16];	//!< the palette of the icon.
	uint16_t titles[8][128];	//!< title of the game in 8 different languages.

						// [0xA40] Reserved space, possibly for other titles.
	uint8_t reserved2[0x800];

	// DSi-specific.
	uint8_t dsi_icon[8][512];	//!< DSi animated icon frame data.
	uint16_t dsi_palette[8][16];	//!< Palette for each DSi icon frame.
	uint16_t dsi_seq[64];	//!< DSi animated icon sequence.
};
// sNDSBanner version.
enum sNDSBannerVersion {
	NDS_BANNER_VER_ORIGINAL = 0x0001,
	NDS_BANNER_VER_ZH = 0x0002,
	NDS_BANNER_VER_ZH_KO = 0x0003,
	NDS_BANNER_VER_DSi = 0x0103,
};

// sNDSBanner sizes.
enum sNDSBannerSize {
	NDS_BANNER_SIZE_ORIGINAL = 0x0840,
	NDS_BANNER_SIZE_ZH = 0x0940,
	NDS_BANNER_SIZE_ZH_KO = 0x0A40,
	NDS_BANNER_SIZE_DSi = 0x23C0,
};

#endif