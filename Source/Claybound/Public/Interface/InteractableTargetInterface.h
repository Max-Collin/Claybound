// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractableTargetInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UInteractableTargetInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class CLAYBOUND_API IInteractableTargetInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

// Returns true if the target can currently be locked onto
	virtual bool CanBeTargeted() const = 0;

	// Returns the location the camera should look at (e.g., a specific bone or socket)
	virtual FVector GetTargetLocation() const = 0;
	

	
};
