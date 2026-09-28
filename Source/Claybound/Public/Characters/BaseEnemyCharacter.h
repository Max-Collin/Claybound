// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interface/InteractableTargetInterface.h"
#include "BaseEnemyCharacter.generated.h"

UCLASS()
class CLAYBOUND_API ABaseEnemyCharacter : public ACharacter, public IInteractableTargetInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseEnemyCharacter();
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual FVector GetTargetLocation() const override;
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	
	
private:
	bool bIsAlive =true;
	
public:	
	

	FORCEINLINE	virtual bool CanBeTargeted() const override {return IsAlive();}
	FORCEINLINE bool IsAlive() const { return bIsAlive; }

};
