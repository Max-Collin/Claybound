// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "CustomCharacterSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class CLAYBOUND_API UCustomCharacterSaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:
	// Base Character Model (Unequipped)
	UPROPERTY(visibleAnywhere,BlueprintReadWrite, Category = "Saved Data")
	FName SavedBaseChest;
	UPROPERTY(visibleAnywhere,BlueprintReadWrite, Category = "Saved Data")
	FName SavedBaseLeg;
	UPROPERTY(visibleAnywhere,BlueprintReadWrite, Category = "Saved Data")
	FName SavedBaseHead;
	UPROPERTY(visibleAnywhere,BlueprintReadWrite, Category = "Saved Data")
	FName SavedBaseHair;
	
	// Active Equipment
	UPROPERTY(visibleAnywhere,BlueprintReadWrite, Category = "Saved Data")
	FName SavedEquippedChest;
	UPROPERTY(visibleAnywhere,BlueprintReadWrite, Category = "Saved Data")
	FName SavedEquippedHelmet;
	UPROPERTY(visibleAnywhere,BlueprintReadWrite, Category = "Saved Data")
	FName SavedEquippedArms;
	UPROPERTY(visibleAnywhere,BlueprintReadWrite, Category = "Saved Data")
	FName SavedEquippedLegs;
	UPROPERTY(visibleAnywhere,BlueprintReadWrite, Category = "Saved Data")
	FName SavedEquippedFoot;
	
	// Colors
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Saved Data")
	FLinearColor SavedSkinColor;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Saved Data")
	FLinearColor SavedHairColor;
};
