// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/BaseEnemyCharacter.h"

// Sets default values
ABaseEnemyCharacter::ABaseEnemyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABaseEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

FVector ABaseEnemyCharacter::GetTargetLocation() const
{
	// Return the mesh location, or a specific lock-on socket if you have one
	return GetMesh() ? GetMesh()->GetSocketLocation(FName("LockOnSocket")) : GetActorLocation();
}

// Called every frame
void ABaseEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


