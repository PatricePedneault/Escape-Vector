// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "Escape_Vector.h"
#include "Escape_VectorCharacter.h"
#include "Escape_VectorProjectile.h"


//////////////////////////////////////////////////////////////////////////
// AEscape_VectorCharacter

AEscape_VectorCharacter::AEscape_VectorCharacter(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	// Set size for collision capsule
	CapsuleComponent->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = PCIP.CreateDefaultSubobject<UCameraComponent>(this, TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->AttachParent = CapsuleComponent;
	FirstPersonCameraComponent->RelativeLocation = FVector(0, 0, 64.f); // Position the camera

	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(100.0f, 30.0f, 10.0f);

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = PCIP.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);			// only the owning player will see this mesh
	Mesh1P->AttachParent = FirstPersonCameraComponent;
	Mesh1P->RelativeLocation = FVector(0.f, 0.f, -150.f);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;

	// Note: The ProjectileClass and the skeletal mesh/anim blueprints for Mesh1P are set in the
	// derived blueprint asset named MyCharacter (to avoid direct content references in C++)


	PrimaryActorTick.bCanEverTick = true;

	this->CharacterMovement->SetMovementMode(MOVE_Flying);

	TimeCharacterFly = 0.0f;

	TimeStayOnPlateForme = 0.0f;

	bVolumeTouch = true;

	bShipTouch = false;

	MultiVitesseDeplacement = 1.0f;

	VitesseLow = 1400.0f;

	VitesseMax = 5600.0f;

	panneauSkeletalMeshReference = NULL;

	MultiDashSpeed  = 2.0f;

	TimeForDashSpeed = 2.0f;

	fovBase = 110;

	fovNow = fovBase;

	fovVitesseUp = 0.1f;

	fovMax = 120;

	dashOn = false;

	respawnCharacter = false;

	fovVitDownRespawn = 0.5f;

	fovVitDownTouchPanneauShip = 0.5f;

	fovValueRespawn = 200.0f;
	
}

//////////////////////////////////////////////////////////////////////////
// Input

void AEscape_VectorCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	// set up gameplay key bindings
	check(InputComponent);

	InputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);

	InputComponent->BindAction("Fire", IE_Pressed, this, &AEscape_VectorCharacter::OnFire);
	InputComponent->BindTouch(EInputEvent::IE_Pressed, this, &AEscape_VectorCharacter::TouchStarted);

	InputComponent->BindAction("Key_Num+", IE_Pressed, this, &AEscape_VectorCharacter::FOVUp);
	InputComponent->BindAction("Key_Num-", IE_Pressed, this, &AEscape_VectorCharacter::FOVDown);
	

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	InputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	InputComponent->BindAxis("TurnRate", this, &AEscape_VectorCharacter::TurnAtRate);
	InputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	InputComponent->BindAxis("LookUpRate", this, &AEscape_VectorCharacter::LookUpAtRate);




}

// Méthode qui permet de d'augmenter le niveau de FOV (pour test)
void AEscape_VectorCharacter::FOVUp()
{
	fovNow++;
	FString VeryCleanString = FString::SanitizeFloat(fovNow);

	VeryCleanString = "fov " + VeryCleanString;

	commandConsole(VeryCleanString);
}

// Méthode qui permet de de diminuer le niveau de FOV (pour test)
void AEscape_VectorCharacter::FOVDown()
{
	fovNow--;
	FString VeryCleanString = FString::SanitizeFloat(fovNow);

	VeryCleanString = "fov " + VeryCleanString;

	commandConsole(VeryCleanString);
}

// Méthode qui recoit un string est qui utilise le string comme commande console
void AEscape_VectorCharacter::commandConsole(FString command)
{
	APlayerController* TargetPC = UGameplayStatics::GetPlayerController(this, 0);

	if (TargetPC)
	{
		TargetPC->ConsoleCommand(command, true);
	}
}

// Au clique de souris cette méthode est appelée
void AEscape_VectorCharacter::OnFire()
{
	// Si le joueur touche au vaisseau ou à une plate-forme, donc le joueur pourra se lancer
	if (bVolumeTouch || bShipTouch)
	{
		bShipTouch = false;
		bVolumeTouch = false;

		// Si le joueur est rester sur le convoit ou sur une plate forme un temps plus petit que la variable "TimeForDashSpeed" donc le joueur est en dash
		// on pourra augmenter sa vitesse de déplacement égale à la valeur de la variable "MultiDashSpeed"
		if (TimeStayOnPlateForme < TimeForDashSpeed)
		{
			MultiVitesseDeplacement = MultiDashSpeed;
			dashOn = true;
			
		}
		else
		{
			dashOn = false;
			MultiVitesseDeplacement = 1.0f;
		}

	}

}


void AEscape_VectorCharacter::TouchStarted(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	// only fire for first finger down
	if (FingerIndex == 0)
	{
		OnFire();
	}
}

void AEscape_VectorCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AEscape_VectorCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void AEscape_VectorCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AEscape_VectorCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AEscape_VectorCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// Si le joueur ne touche plus à aucune plate-forme ou au convoit
	if (!bVolumeTouch && !bShipTouch)
	{
		// Si le joueur est en InstantDash on doit donc augmenter le FOV pour donner un effet de vitesse
		if (dashOn)
		{
			if(fovNow < fovMax)
			{
				fovNow += fovVitesseUp;
				fString = FString::SanitizeFloat(fovNow);

				// On envoit la commande console à exécuter
				commandConsole("fov " + fString);
			}
		}

		// On ajoute le temps au compteur qui permet de savoir depuis combien de temps le joueur ne touche plus à une plateforme
		TimeCharacterFly += DeltaSeconds;

		// Si cela fait 4 secondes que le joueur n'a pas touché à aucune plate-forme on le téléporte à la dernière toucher
		if (TimeCharacterFly > 4.0f)
		{
			
			if (iNbDead < 2)
			{
				/*GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "RESPAWN_PANNEAU");
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, " ");*/
				respawnToPanneau();
			}
			else
			{
				iNbDead++;
			}
		}

		TimeStayOnPlateForme = 0.0f;

	}
	else
	{
		// Permet de savoir depuis combien de temps le joueur se trouve sur le convoit ou le transporteur
		TimeStayOnPlateForme += DeltaSeconds;
		
		// Si le joueur respawn une diminution du FOV doit avoir lieu ( mis à une valeur plus grande juste avant) pour donner une effet 
		if(respawnCharacter)
		{ 
			if (fovNow > fovBase)
			{
				fovNow -= fovVitDownRespawn;
				fString = FString::SanitizeFloat(fovNow);
				commandConsole("fov " + fString);
			}
			else
			{
				respawnCharacter = false;
			}
			return;
		}

		// Si le FOV est plus grand que le nombre qu'il devrait être lorsque le joueur touche le convoit ou le vaisseau
		// on diminue celui-ci 
		if (fovNow > fovBase)
		{
			fovNow -= fovVitDownTouchPanneauShip;
			fString = FString::SanitizeFloat(fovNow);
			commandConsole("fov " + fString);
		}
		
	}
}

// Méthode qui permet de déplacer le joueur vers le dernier checkpoint
void  AEscape_VectorCharacter::respawnToCheckPoint()
{
	APlayerController* TargetPC = UGameplayStatics::GetPlayerController(this, 0);

	this->TeleportTo(checkPointSkeletalRef->GetSocketLocation("Socket_1"), FRotator(0, 0, 0), true, true);

	// On met la rotation du joueur pour que celui-ci sois dans la meme direction que la plate-forme
	TargetPC->SetControlRotation(checkPointSkeletalRef->GetRightVector().Rotation());
	
	LastPlateFormePosition = checkPointSkeletalRef->GetSocketLocation("Socket_1");

	this->CharacterMovement->Velocity = FVector(0, 0, 0);
	iNbDead = 0;
	bVolumeTouch = true;
	TimeCharacterFly = 0.0f;

	// On met une grande valeur de FOV qui sera par la suite diminué dans le tick
	fovNow = fovValueRespawn;
	fString = FString::SanitizeFloat(fovValueRespawn);
	commandConsole("fov " + fString);
	respawnCharacter = true;
}

// Méthode qui permet de déplacer le joueur vers la dernière plate-forme touché
void AEscape_VectorCharacter::respawnToPanneau()
{
	this->TeleportTo(LastPlateFormePosition, FRotator(0, 0, 0), true, true);
	IsRespawning();
	iNbDead++;
	TimeCharacterFly = 0;
	this->CharacterMovement->Velocity = FVector(0, 0, 0);
	bVolumeTouch = true;
	TimeCharacterFly = 0.0f;

	// On met une grande valeur de FOV qui sera par la suite diminué dans le tick
	fovNow = fovValueRespawn;
	fString = FString::SanitizeFloat(fovValueRespawn);
	commandConsole("fov " + fString);
	respawnCharacter = true;
}

// Méthode qui est appelé lorsque le joueur touche à une plate-forme, on prend en note dans la variable "LastPlateFormePosition" la position
// de celle-ci qui pourra être par la suite utilisé si le joueur doit être respawner
void AEscape_VectorCharacter::touchPanneau()
{
	this->CharacterMovement->Velocity = FVector(0, 0, 0);

	LastPlateFormePosition = panneauSkeletalMeshReference->GetSocketLocation("Socket_1");

	this->SetActorLocation(LastPlateFormePosition);

	bVolumeTouch = true;
	TimeCharacterFly = 0.0f;

}

// Méthode qui est appelé lorsque le joueur touche au convoit
void AEscape_VectorCharacter::touchShip()
{
	this->CharacterMovement->Velocity = FVector(0, 0, 0);
	bShipTouch = true;
	TimeCharacterFly = 0.0f;
}

void AEscape_VectorCharacter::IsRespawning_Implementation()
{
	//Vide
}

void AEscape_VectorCharacter::BeginPlay()
{
	Super::BeginPlay();


	/*GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "FlyingMode");
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, " ");*/


	//On initialise la premiere position Last Plaforme au player start
	LastPlateFormePosition = this->GetActorLocation();

	plateFormeCheckPointPosition = LastPlateFormePosition;

	this->CharacterMovement->SetMovementMode(MOVE_Flying);

	commandConsole("DisableAllScreenMessages");

}