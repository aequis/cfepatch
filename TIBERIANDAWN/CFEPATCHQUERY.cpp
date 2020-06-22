#include "FUNCTION.H"

extern bool DLL_Export_Get_Input_Key_State(KeyNumType key);

//Disable A10s in missions if the player has no buildings
bool CFE_Patch_A10_Override(const ObjectTypeClass& object, const HousesType house)
{
	if (ActiveCFEPatchConfig.DisableCommandoAirstrikes
		&& GameToPlay == GAME_NORMAL
		&& house != PlayerPtr->Class->House
		&& object.What_Am_I() == RTTI_AIRCRAFTTYPE
		&& ((const AircraftTypeClass&)(object)).Type == AIRCRAFT_A10)
	{
		return CFE_Patch_House_Has_Any_Buildings_In_Map(*PlayerPtr);
	}
	return true;
}

bool CFE_Patch_House_Has_Any_Buildings_In_Map(HouseClass& house)
{
	if (house.ActiveBScan)
	{
		/*cfehunter 12/06/2020
		*Certain missions give you buildings that are owned by you, but not yet discovered
		*We need to account for this here.
		*/
		if (house.IsHuman && GameToPlay == GAME_NORMAL)
		{
			for (const void* rawBuilding : Buildings.ActivePointers)
			{
				const BuildingClass& building = *reinterpret_cast<const BuildingClass*>(rawBuilding);
				if (building.House == &house && building.Is_Discovered_By_Player(&house))
					return true;
			}
		}
		else return true;
	}
	return false;
}

bool CFE_Patch_Is_Wall(const ObjectTypeClass& object)
{
	return object.What_Am_I() == RTTI_BUILDINGTYPE && static_cast<const BuildingTypeClass&>(object).IsWall;
}

bool CFE_Patch_Can_Have_Rally_Point(const ObjectClass& object)
{
	return object.What_Am_I() == RTTI_BUILDING && static_cast<const BuildingClass&>(object).Can_Have_Rally_Point();
}

bool CFE_Patch_Should_Extend_Walls()
{
	//#CFE TODO: This isn't multiplayer safe
	return ActiveCFEPatchConfig.WallBuildLength > 1 && !DLL_Export_Get_Input_Key_State(KN_LCTRL);
}

bool CFE_Patch_Should_Attack_Move(const TARGET whom, const TARGET target)
{
	return ActiveCFEPatchConfig.EnableAttackMove && whom == target;
}

bool CFE_Patch_Is_Cell_Friendly_To_House(const HousesType house, const CELL cell)
{
	if (Map[cell].Owner == house) {
		return(true);
	}

	if (const BuildingClass* const base = Map[cell].Cell_Building()) {
		if (base->House->Class->House == house) {
			return true;
		}
	}
	return false;
}

bool CFE_Patch_Is_Cell_In_Radius_To_Friendly_House(const HousesType house, const CELL cell, const int radius)
{
	if (Map.In_Radar(cell))
	{
		const int cellX = Cell_X(cell);
		const int cellY = Cell_Y(cell);
		for (int y = cellY - radius, yEnd = cellY + radius; y <= yEnd; ++y) {
			for (int x = cellX - radius, xEnd = cellX + radius; x <= xEnd; ++x) {
				const CELL nextCell = XY_Cell(x, y);

				if (Map.In_Radar(nextCell) && CFE_Patch_Is_Cell_Friendly_To_House(house, nextCell))
					return true;
			}
		}
	}
	return false;
}

bool CFE_Patch_Unit_Should_Rally(const TechnoClass& object)
{
	switch (object.What_Am_I())
	{
	case RTTI_UNIT:
		return !static_cast<const UnitClass&>(object).Class->IsToHarvest;
	default:
		return true;
	}
}