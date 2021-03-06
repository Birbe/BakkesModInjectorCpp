#include "stdafx.h"
#undef max
#undef min
#include "Installer.h"
#include "zip_file.hpp"
#include "WindowsUtils.h"
#include "logger.h"

Installer::Installer(std::wstring updatePackageLocation, std::string destination)
{
	updatePackage = updatePackageLocation;
	extractDir = destination;
	LOG_LINE(INFO, "Initialized installer, location=" << WindowsUtils::WStringToString(updatePackage) << ", destination=" << extractDir)
}

Installer::~Installer()
{
}

void Installer::Install()
{
	LOG_LINE(INFO, "Installing update")
		std::ifstream fileStream;
	fileStream.open(updatePackage);
	std::basic_ifstream<BYTE> fileStr(updatePackage, std::ios::binary);

	// read the data:
	auto data = std::vector<BYTE>((std::istreambuf_iterator<BYTE>(fileStr)),
	std::istreambuf_iterator<BYTE>());

	std::streampos	fsize = fileStream.tellg();
	fileStream.seekg(0, std::ios::end);
	fsize = fileStream.tellg() - fsize;
	fileStream.seekg(0);
	//fileStream.close();
	LOG_LINE(INFO, "Installing update2" << WindowsUtils::WStringToString(updatePackage) << " - " << std::to_string(fsize))
	miniz_cpp::zip_file file(data);
	LOG_LINE(INFO, "Installing update3")
	WindowsUtils::CreateFolder(extractDir);
	for (auto &member : file.infolist())
	{
		std::string fullPath = extractDir + member.filename;
		LOG_LINE(INFO, "Extracting file " << member.filename)
		if (fullPath.substr(fullPath.size() - std::string(".cfg").size()).compare(".cfg") == 0)
		{
			
			if (WindowsUtils::FileExists(fullPath)) {//Don't overwrite default cfg files
				LOG_LINE(INFO, "File (" << member.filename << ") is cfg file and it already exists, not overwriting!")
				continue;
			}
		}
		if (member.filename.find(".") == std::string::npos) //It's a folder
		{
			LOG_LINE(INFO, "File " << member.filename << ") is folder, creating if not exists yet")
			WindowsUtils::CreateFolder(fullPath);
		} else if (WindowsUtils::FileExists(fullPath))
		{
			int success = remove(fullPath.c_str());
			LOG_LINE(INFO, "File " << member.filename << " exists, deleting file result: " << success)
		}
		
		file.extract(member, extractDir);
		LOG_LINE(INFO, "File extraction result: " << WindowsUtils::FileExists(fullPath))
		//file.extract(member, "");
	}
	file.printdir(LOG(INFO));
}
