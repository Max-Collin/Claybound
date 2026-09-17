// Fill out your copyright notice in the Description page of Project Settings.


#include "ClayboundCharacter.h"

#include "Struct/ModularPartData.h"

// Sets default values
AClayboundCharacter::AClayboundCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	HeadMesh = CreateDefaultSubobject<USkeletalMeshComponent>(FName("Head Mesh"));
	LegMesh = CreateDefaultSubobject<USkeletalMeshComponent>(FName("Leg Mesh"));
	HandMesh = CreateDefaultSubobject<USkeletalMeshComponent>(FName("Hands Mesh"));
	FootMesh = CreateDefaultSubobject<USkeletalMeshComponent>(FName("Foot Mesh"));
	HairMesh = CreateDefaultSubobject<USkeletalMeshComponent>(FName("Hair Mesh"));
	
	HeadMesh->SetupAttachment(GetMesh());
	LegMesh->SetupAttachment(GetMesh());
	HandMesh->SetupAttachment(GetMesh());
	FootMesh->SetupAttachment(GetMesh());
	HairMesh->SetupAttachment(GetMesh());
}

// Called when the game starts or when spawned
void AClayboundCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AClayboundCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AClayboundCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AClayboundCharacter::UpdatePart(ECustomizationType Category, FName RowName, UDataTable* PartDataTable)
{
	
	UE_LOG(LogTemp, Warning, TEXT("Character customization"));
	if (!PartDataTable) return;

	// Find the row matching the asset we want to load
	FModularPart* PartData = PartDataTable->FindRow<FModularPart>(RowName, TEXT("Context"));
	if (!PartData) return;

	// Resolve the soft object pointer (synchronously loading for simplicity here)
	USkeletalMesh* NewMesh = PartData->MeshAsset.LoadSynchronous();
	if (!NewMesh) return;

	// Route the mesh to the correct component based on category
	
	switch (Category)
	{
	case ECustomizationType::ECT_Chest:
		GetMesh()->SetSkeletalMeshAsset(NewMesh);
		break;
	case ECustomizationType::ECT_Leg:
		LegMesh->SetSkeletalMeshAsset(NewMesh);
		break;
	case ECustomizationType::ECT_Foot:
		FootMesh->SetSkeletalMeshAsset(NewMesh);
		break;
	case ECustomizationType::ECT_Hand:
		HandMesh->SetSkeletalMeshAsset(NewMesh);
		break;
	case ECustomizationType::ECT_Head:
		HeadMesh->SetSkeletalMeshAsset(NewMesh);
		break;
	case ECustomizationType::ECT_Hair:
		HairMesh->SetSkeletalMeshAsset(NewMesh);
		break;
		
	default: 
		UE_LOG(LogTemp, Warning, TEXT("Unhandled  Category, Character customization"));
		break;
	}
}

