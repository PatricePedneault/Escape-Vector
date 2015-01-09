// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "Escape_VectorCharacter.generated.h"

UCLASS(config = Game)
class AEscape_VectorCharacter : public ACharacter
{
	GENERATED_UCLASS_BODY()

		/** Pawn mesh: 1st person view (arms; seen only by self) */
		UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		TSubobjectPtr<class USkeletalMeshComponent> Mesh1P;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		TSubobjectPtr<class UCameraComponent> FirstPersonCameraComponent;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseLookUpRate;

	/** Gun muzzle's offset from the characters location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		FVector GunOffset;

	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
		TSubclassOf<class AEscape_VectorProjectile> ProjectileClass;

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		USoundBase* FireSound;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		UAnimMontage* FireAnimation;

	//Vrai si on touche à un volume
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		bool bVolumeTouch;


	//Vrai si on touche le vaisseau
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	bool bShipTouch;

	UFUNCTION(BlueprintNativeEvent, Category = "Pawn|Character")
	void IsRespawning();

	UFUNCTION(BlueprintCallable, Category = "Pawn|Character")
	void touchPanneau();

	UFUNCTION(BlueprintCallable, Category = "Pawn|Character")
	void touchShip();

	// Variable qui permet de stocker le nombre de mort du joueur
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Player)
	float			iNbDead;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Player)
	USkeletalMeshComponent* panneauSkeletalMeshReference;

	// Vitesse lorsque le joueur commence à se propulser
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Player)
	float	    VitesseLow;

	// Vitesse maximal que le joueur peux atteindre par accelération
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Player)
	float	    VitesseMax;

	// Multiplication de la vitesse de deplacement plus le joueur 
	// se déplacement rapidement de plate-forme en plate-forme
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Player)
	float		MultiVitesseDeplacement;

	// Vitesse lors du Dash
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Player)
	float		MultiDashSpeed;

	// Contient le temps max que le joueur peut rester sur le vaisseau ou une plate-forme pour que le dash soit activé ( sinon un déplacement normal )
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Player)
	float		TimeForDashSpeed;


	// Variable qui permet de stocker le temps resté sur la plate-forme
	float		TimeStayOnPlateForme;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Player)
	FVector			plateFormeCheckPointPosition;

	UFUNCTION(BlueprintCallable, Category = "Pawn|Character")
	void respawnToCheckPoint();

	UFUNCTION(BlueprintCallable, Category = "Pawn|Character")
	void respawnToPanneau();

	// valeur Max que pourra atteindre le FOV
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FOV")
	float   fovMax;

	// Vitesse que le FOV augmentera lors des Dash
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FOV")
	float	fovVitesseUp;

	// Vitesse que le FOV diminue apres un respawn
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FOV")
	float	fovVitDownRespawn;

	// Vitesse que le FOV diminue apres un dash
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FOV")
	float	fovVitDownTouchPanneauShip;

	// Permet de donner la valeur du FOV lors des respawn
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FOV")
	float	fovValueRespawn;

	// Permet de donner la valeur de base du FOV (ne sera jamais plus bas)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FOV")
	float	fovBase;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Player)
	USkeletalMeshComponent* checkPointSkeletalRef;


protected:

	virtual void BeginPlay() override;

	// Permet de stocker la valeur du FOV en se moment
	float	fovNow;

	// Permet de savoir si le joueur est en Dash
	bool	dashOn;

	/** Handler for a touch input beginning. */
	void TouchStarted(const ETouchIndex::Type FingerIndex, const FVector Location);

	/** Fires a projectile. */
	UFUNCTION(BlueprintCallable, Category = "Pawn|Character")
		void OnFire();

	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles stafing movement, left and right */
	void MoveRight(float Val);

	/**
	* Called via input to turn at a given rate.
	* @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	*/
	void TurnAtRate(float Rate);

	/**
	* Called via input to turn look up/down at a given rate.
	* @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	*/
	void LookUpAtRate(float Rate);

	// Permet de stocker le nombre de seconde que le joueur ne touche plus à une plate-forme
	float			TimeCharacterFly;

	// Permet de Stocker la location de la dernière plate-forme touché
	FVector			LastPlateFormePosition;

	void FOVUp();

	void FOVDown();

	void commandConsole(FString command);

	virtual void Tick(float DeltaSeconds) override;

	void scalerMate();

	FString fString;

	// Permet de savoir si le joueur respawn
	bool	respawnCharacter;


protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	// End of APawn interface
};

