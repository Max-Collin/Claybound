#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ModularPartData.generated.h"

UENUM(BlueprintType)
enum class ECustomizationType : uint8
{
	ECT_Chest UMETA(DisplayName = "Chest"),
	ECT_Leg UMETA(DisplayName = "Leg"),
	ECT_Foot UMETA(DisplayName = "Foot"),
	ECT_Hand UMETA(DisplayName = "Hand"),
	ECT_Head UMETA(DisplayName = "Head"),
	ECT_Hair UMETA(DisplayName = "Hair"),
	ECT_Max UMETA(DisplayName = "Default Max")
};


USTRUCT(BlueprintType)
struct FModularPart : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Customization")
	FText PartName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Customization")
	TSoftObjectPtr<USkeletalMesh> MeshAsset;
};
