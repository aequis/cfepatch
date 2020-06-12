#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <ShlObj.h>
#include <string>

#include "FUNCTION.H"

std::string Get_User_Mod_Dir()
{
	std::string dirPath;
	char documentsPath[1024];
	HRESULT result = SHGetFolderPathA(NULL, CSIDL_MYDOCUMENTS, NULL, NULL, documentsPath);
	if (SUCCEEDED(result))
	{
		dirPath = documentsPath;
		dirPath += "\\CnCRemastered\\Mods\\Tiberian_Dawn";
	}
	return dirPath;
}

bool Populate_CFE_Patch_Config_From_INI(const char* path)
{
	static constexpr int InvalidSetting = std::numeric_limits<int>::lowest();
	char* const fileBuffer = _ShapeBuffer;
	CCFileClass file(path);
	memset(fileBuffer, '\0', _ShapeBufferSize);

	if (file.Is_Available())
	{
		file.Read(fileBuffer, _ShapeBufferSize - 1);
		file.Close();

		const auto OverwriteIfValid = [](auto& target, const int value) { if (value != InvalidSetting) target = value; };

		//cfehunter 12/06/2020 It's really starting to get to the point where I should make feature flags
		//Features
		OverwriteIfValid(ActiveCFEPatchConfig.EnableASPathing           , WWGetPrivateProfileInt("FEATURES", "ASTAR_PATHING", InvalidSetting, fileBuffer));
		OverwriteIfValid(ActiveCFEPatchConfig.EnableOOMRepair           , WWGetPrivateProfileInt("FEATURES", "OOM_REPAIR", InvalidSetting, fileBuffer));
		OverwriteIfValid(ActiveCFEPatchConfig.EnableRallyPoints         , WWGetPrivateProfileInt("FEATURES", "RALLY_POINTS", InvalidSetting, fileBuffer));
		OverwriteIfValid(ActiveCFEPatchConfig.EnableHarvyQueueJump      , WWGetPrivateProfileInt("FEATURES", "HARVY_QUEUE_JUMP", InvalidSetting, fileBuffer));
		OverwriteIfValid(ActiveCFEPatchConfig.EnableRepairQueue         , WWGetPrivateProfileInt("FEATURES", "REPAIR_QUEUE", InvalidSetting, fileBuffer));
		OverwriteIfValid(ActiveCFEPatchConfig.EnableHarvySelfRepair     , WWGetPrivateProfileInt("FEATURES", "HARVY_SELF_REPAIR", InvalidSetting, fileBuffer));
		OverwriteIfValid(ActiveCFEPatchConfig.DisableCommandoAirstrikes , WWGetPrivateProfileInt("FEATURES", "DISABLE_COMMANDO_AIRSTRIKES", InvalidSetting, fileBuffer));

		//Settings
		OverwriteIfValid(ActiveCFEPatchConfig.BuildingGap               , WWGetPrivateProfileInt("SETTINGS", "BUILDING_GAP", InvalidSetting, fileBuffer));
		OverwriteIfValid(ActiveCFEPatchConfig.HarvyQueueJumpCutoff      , WWGetPrivateProfileInt("SETTINGS", "QUEUE_JUMP_CUTOFF", InvalidSetting, fileBuffer));
		OverwriteIfValid(ActiveCFEPatchConfig.TibGrowthScale            , WWGetPrivateProfileInt("SETTINGS", "TIB_GROWTH_SCALE", InvalidSetting, fileBuffer));
		return true;
	}

	return false;
}

//cfehunter 12/06/2020 I really miss std::filesystem
void Initialise_CFE_Patch_Config()
{
	static constexpr const char* DefaultININame = "DEFAULT.CFEPATCH.INI";

	const std::string iniDir = Get_User_Mod_Dir() + "\\CFEPatch";
	const std::string iniPath = iniDir + "\\CFEPATCH.INI";

	//Initialise from the default config
	Populate_CFE_Patch_Config_From_INI(DefaultININame);

	if (!Populate_CFE_Patch_Config_From_INI(iniPath.c_str())) {
		//Clone the default
		CCFileClass defaultConfig(DefaultININame);
		if (defaultConfig.Is_Available())
		{
			//Create directories and clone the default config
			const int directoryResult = SHCreateDirectoryExA(NULL, iniDir.c_str(), NULL);
			if (directoryResult == ERROR_SUCCESS || directoryResult == ERROR_ALREADY_EXISTS)
			{
				//Copy the default ini to the user directory so they can modify it later
				CopyFileA(defaultConfig.File_Name(), iniPath.c_str(), TRUE);
			}
		}
	}

}
