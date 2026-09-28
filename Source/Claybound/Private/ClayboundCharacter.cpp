// Fill out your copyright notice in the Description page of Project Settings.


#include "ClayboundCharacter.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Interface/InteractableTargetInterface.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
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

void AClayboundCharacter::ToggleTargetLock()
{
	UE_LOG(LogTemp, Warning, TEXT("Toggle Target lock"));
	if (bIsLockedOn)
	{
		// Disengage lock
		bIsLockedOn = false;
		LockedTarget = nullptr;
		// Restore normal movement settings 
		GetCharacterMovement()->bOrientRotationToMovement = true; 
		
		if(GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Target Unlocked"));	
		}
			
		
	}
	else
	{
		LockedTarget = FindBestTarget();
		if (LockedTarget)
		{
			bIsLockedOn = true;
			// character to stop rotating freely
			GetCharacterMovement()->bOrientRotationToMovement = false; 
			if(GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Target Locked"));	
			}
		}
	}
}

AActor* AClayboundCharacter::FindBestTarget()
{
	FVector Start = GetActorLocation();
	FVector End = Start; // Sphere trace happens in-place
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);

	TArray<FHitResult> HitResults;
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	// Add Pawn or custom collision channels here
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn)); 

	bool bHit = UKismetSystemLibrary::SphereTraceMultiForObjects(
		GetWorld(), Start, End, LockOnRadius, ObjectTypes, false, 
		ActorsToIgnore, EDrawDebugTrace::ForDuration, HitResults, true
	);

	AActor* BestTarget = nullptr;
	float ClosestDistance = LockOnRadius;

	if (bHit)
	{
		for (const FHitResult& Hit : HitResults)
		{
			AActor* HitActor = Hit.GetActor();
			if (HitActor && HitActor->GetClass()->ImplementsInterface(UInteractableTargetInterface::StaticClass()))
			{
				IInteractableTargetInterface* TargetInterface = Cast<IInteractableTargetInterface>(HitActor);
				if (TargetInterface && TargetInterface->CanBeTargeted())
				{
					float Dist = FVector::Dist(Start, HitActor->GetActorLocation());
					if (Dist < ClosestDistance)
					{
						ClosestDistance = Dist;
						BestTarget = HitActor;
					}
				}
			}
		}
	}
	return BestTarget;
}

void AClayboundCharacter::TargetLockMovement(float DeltaTime)
{
	IInteractableTargetInterface* TargetInterface = Cast<IInteractableTargetInterface>(LockedTarget);
		
	if (!TargetInterface || !TargetInterface->CanBeTargeted() || FVector::Dist(GetActorLocation(), LockedTarget->GetActorLocation()) > LockOnRadius)
	{
		ToggleTargetLock(); 
		return;
	}

	// 1. ROTATE CAMERA TO TARGET
	FVector CameraLocation = GetActorLocation(); // Or use your Camera Boom/Follow Camera location
	FVector TargetLoc = TargetInterface->GetTargetLocation();

	FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(CameraLocation, TargetLoc);
	FRotator CurrentControlRotation = GetControlRotation();

	// Smoothly rotate the camera controller
	FRotator InterpControlRotation = UKismetMathLibrary::RInterpTo(CurrentControlRotation, TargetRotation, DeltaTime, RotationInterpSpeed);
		
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC)
	{
		// Constrain Pitch if you don't want the camera tilting wildly up/down near tall enemies
		// InterpControlRotation.Pitch = FMath::Clamp(InterpControlRotation.Pitch, -30.f, 0.f); 
			
		PC->SetControlRotation(InterpControlRotation);
	}

	// 2. SMOOTHLY ROTATE CHARACTER CAPSULE TO TARGET
	// This makes the character body gracefully face the enemy while strafing
	FRotator CurrentActorRotation = GetActorRotation();
		
	// We only want the Yaw (horizontal rotation) for the character's physical body
	FRotator TargetActorRotation = FRotator(0.f, TargetRotation.Yaw, 0.f); 
		
	FRotator InterpActorRotation = UKismetMathLibrary::RInterpTo(CurrentActorRotation, TargetActorRotation, DeltaTime, RotationInterpSpeed);
	SetActorRotation(InterpActorRotation);
}

// Called every frame
void AClayboundCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsLockedOn && LockedTarget)
	{
		TargetLockMovement(DeltaTime);
	}
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
		
		// Target Lock
		EnhancedInputComponent->BindAction(TargetLockAction, ETriggerEvent::Started, this, &AClayboundCharacter::ToggleTargetLock);
		
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

