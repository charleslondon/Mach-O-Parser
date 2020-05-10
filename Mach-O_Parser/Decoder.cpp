#include <string>
#include <fstream>
#include <deque>
#include <iostream>
#include <filesystem>
#include <winsock2.h> /*Access to endian conversion functions*/
#pragma comment(lib, "Ws2_32.lib")
#include "CommandVariant.h"

template <typename Structure>
Structure readInAndReset(std::ifstream& fin)
{
	std::streampos originalOffset = fin.tellg();
	Structure structure;
	fin.read((char*)&structure, sizeof(structure));
	fin.seekg(originalOffset);

	return structure;
}

template <typename Structure>
Structure readIn(std::ifstream& fin)
{
	Structure structure;
	fin.read((char*)&structure, sizeof(structure));

	return structure;
}

bool is64Arch(std::ifstream& fin)
{
	std::streampos originalOffset = fin.tellg();
	fin.seekg(0, std::ios_base::beg);
	auto magic = readIn<unsigned long>(fin);
	fin.seekg(originalOffset);

	switch (magic) //Our magic number will be the first bytes of the file.
	{
	case MH_MAGIC:
	case MH_CIGAM:
		return false;

	case MH_MAGIC_64:
	case MH_CIGAM_64:
		return true;

	default:
		throw; //We don't have a signature match, this probably isn't a Mach-O file.
	}
}

Command_Struct determineCommand(std::ifstream& fin, uint32_t commandType)
{
	switch (commandType)
	{
	case LC_SYMTAB:
		return readInAndReset<symtab_command>(fin);

	case LC_ROUTINES:
		return readInAndReset<routines_command>(fin);

	case LC_DYSYMTAB:
		return readInAndReset<dysymtab_command>(fin);

	case LC_LOAD_DYLIB:
	case LC_ID_DYLIB:
	case LC_LOAD_WEAK_DYLIB:
	case LC_REEXPORT_DYLIB:
		return readInAndReset<dylib_command>(fin);

	case LC_ID_DYLINKER:
	case LC_LOAD_DYLINKER:
	case LC_DYLD_ENVIRONMENT:
		return readInAndReset<dylinker_command>(fin);

	case LC_UUID:
		return readInAndReset<uuid_command>(fin);

	case LC_SOURCE_VERSION:
		return readInAndReset<source_version_command>(fin);

	case LC_VERSION_MIN_MACOSX:
	case LC_VERSION_MIN_IPHONEOS:
		return readInAndReset<version_min_command>(fin);

	case LC_CODE_SIGNATURE:
	case LC_SEGMENT_SPLIT_INFO:
	case LC_FUNCTION_STARTS:
	case LC_DATA_IN_CODE:
	case LC_DYLIB_CODE_SIGN_DRS:
		return readInAndReset<linkedit_data_command>(fin);

	case LC_SEGMENT_64:
		return readInAndReset<segment_command_64>(fin);

	case LC_ROUTINES_64:
		return readInAndReset<routines_command_64>(fin);

	case LC_DYLD_INFO_ONLY:
		return readInAndReset<dyld_info_command>(fin);

	case LC_MAIN:
		return readInAndReset<entry_point_command>(fin);

	default:
		return std::monostate();
	}
}

mach_header_64 decodeHeader(std::ifstream& fin)
{
	return readIn<mach_header_64>(fin);
}

void handleCommand(std::ifstream& fin, std::ofstream& fout, Command_Struct cmd)
{
	if (std::holds_alternative<version_min_command>(cmd))
	{
		struct MacOS_Version
		{
			unsigned int xxxx	: 16;
			unsigned int yy		: 8;
			unsigned int zz		: 8;
		};

		MacOS_Version versionInfo;
		memcpy(&versionInfo, &std::get<version_min_command>(cmd).version, sizeof(MacOS_Version));
		fout << "MacOS SDK Version : " << versionInfo.xxxx << "." << versionInfo.yy << "." << versionInfo.zz << std::endl;
	}
	else if (std::holds_alternative<segment_command_64>(cmd))
	{
		fout << "Data Segment Name : " << std::get<segment_command_64>(cmd).segname << std::endl;
	}
}

void decodeFile(const std::string& inputFileName, const std::string& outputFileName)
{
	std::ifstream fin(inputFileName.c_str(), std::ios::binary);
	std::ofstream fout(outputFileName.c_str(), std::ofstream::binary);

	mach_header_64 header = decodeHeader(fin);
	auto commands = std::deque<Command_Struct>();

	for (uint32_t idx = 0; idx < header.ncmds; ++idx)
	{
		load_command loadCommandHeader = readInAndReset<load_command>(fin);
		commands.emplace_back(determineCommand(fin, loadCommandHeader.cmd));
		fin.ignore(loadCommandHeader.cmdsize);
	}

	for (const auto& command : commands)
	{
		handleCommand(fin, fout, command);
	}

	fin.close();
	fout.close();
}

int main(int argc, char* argv[])
{
	decodeFile(
		std::filesystem::current_path().append(argv[1]).string(),
		std::filesystem::current_path().append(argv[2]).string());

	return 0;
}