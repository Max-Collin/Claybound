// Fill out your copyright notice in the Description page of Project Settings.


#include "ClayboundCharacter.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
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
	
	
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(GetMesh());
    CameraBoom->TargetArmLength = 600.f;
    CameraBoom->bUsePawnControlRotation = true;
    
    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom,USpringArmComponent::SocketName);
    FollowCamera->bUsePawnControlRotation = false;
	
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement =true;
}

// Called when the game starts or when spawned
void AClayboundCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void AClayboundCharacter::Move(const FInputActionValue& Value)
{
	if (bDisableGameplay) return;
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AClayboundCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(-LookAxisVector.Y);
	}
}

void AClayboundCharacter::Jump()
{
	Super::Jump();
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

	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
	
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AClayboundCharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AClayboundCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AClayboundCharacter::Look);
		
	}



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

