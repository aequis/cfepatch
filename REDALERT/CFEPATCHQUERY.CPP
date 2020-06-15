#include "FUNCTION.H"

extern bool DLL_Export_Get_Input_Key_State(KeyNumType key);

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
	return ActiveCFEPatchConfig.WallBuildLength > 1 && !DLL_Export_Get_Input_Key_State(KN_LCTRL);
}

bool CFE_Patch_Unit_Should_Rally(const FootClass& object)
{
	switch (object.What_Am_I())
	{
		case RTTI_UNIT:
			return !static_cast<const UnitClass&>(object).Class->IsToHarvest;
		default:
			return true;
	}
}
