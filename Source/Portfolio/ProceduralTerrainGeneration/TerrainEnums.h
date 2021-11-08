#pragma once

#include "UObject/ObjectMacros.h"

/** Custom movement modes for Characters. */
UENUM(BlueprintType)
enum class EDirectionException: uint8
{
	Ve_None UMETA(DisplayName="None"),
	
	Ve_North UMETA(DisplayName="North"),
	
	Ve_South	UMETA(DisplayName = "South"),
	
	Ve_East		UMETA(DisplayName = "East"),
	
	Ve_West	UMETA(DisplayName = "West")

};

UENUM(BlueprintType)
enum class EClampingNeeds: uint8
{
	Ve_None		UMETA(DisplayName="None"),

	Ve_All		UMETA(DisplayName="All"),

	Ve_SameAsPrevious	UMETA(DisplayName= "Same as Previous"),
	
	Ve_Top		UMETA(DisplayName="Top"),
	
	Ve_Bottom	UMETA(DisplayName = "Bottom"),
	
	Ve_Left		UMETA(DisplayName = "Left"),
	
	Ve_Right	UMETA(DisplayName = "Right"),

	Ve_TopRight		UMETA(DisplayName="Top Right"),
	
	Ve_TopLeft		UMETA(DisplayName="Top Left"),
	
	Ve_BottomLeft	UMETA(DisplayName = "Bottom Left"),
		
	Ve_BottomRight	UMETA(DisplayName = "Bottom Right")
};

UENUM(BlueprintType)
enum class EComponentShapes: uint8
{
	Ve_Plain UMETA(DisplayName="Plain"),
	
	Ve_Cube UMETA(DisplayName="Cube"),
	
	Ve_Sphere	UMETA(DisplayName = "Sphere")
	
};
UENUM(BlueprintType)
enum class EShapeSide: uint8
{
	/**Positive Z Axis*/
	Ve_Top		UMETA(DisplayName="Top"),

	/**Negative Z Axis*/
	Ve_Bottom	UMETA(DisplayName="Bottom"),

	/**Negative Y Axis*/
	Ve_Left		UMETA(DisplayName = "Left"),

	/**Positive Y Axis*/
	Ve_Right	UMETA(DisplayName = "Right"),

	/**Positive X Axis*/
	Ve_Front	UMETA(DisplayName = "Front"),

	/**Negative X Axis*/
	Ve_Back		UMETA(DisplayName = "Back")
};
UENUM(BlueprintType)
enum class ESubSections: uint8
{
	Ve_One			UMETA(DisplayName="1"),

	Ve_Two			UMETA(DisplayName="2"),
	
	Ve_Four			UMETA(DisplayName="4"),

	Ve_Eight		UMETA(DisplayName="8"),
	
	Ve_Sixteen		UMETA(DisplayName="16"),

	Ve_ThirtyTwo	UMETA(DisplayName="32"),
	
	Ve_SixtyFour	UMETA(DisplayName="64")
};
