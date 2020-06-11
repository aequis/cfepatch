#include "FUNCTION.H"

bool CFE_Patch_Can_Enter_Override(const ObjectClass& object)
{
	if (ActiveCFEPatchConfig.EnableRepairQueue && object.What_Am_I() == RTTI_BUILDING) {
		const StructType buildingType = static_cast<const BuildingClass&>(object);
		return buildingType == STRUCT_REPAIR;
	}
	return false;
}