

#pragma once

#include "GameFramework/Pawn.h"
#include "Escape_VectorLeecherPawn.generated.h"

UCLASS()
class AEscape_VectorLeecherPawn : public APawn
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Leecher)
	TSubobjectPtr<class USkeletalMeshComponent> MeshLeecher;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Leecher)
	TSubobjectPtr<class UCapsuleComponent> capsuleComponentLeecher;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Leecher)
	TSubobjectPtr<class USceneComponent> sceneComponentLeecher;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Leecher)
	UStaticMeshComponent* shipStaticMeshReference;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Leecher)
	USkeletalMeshComponent* shipSkeletalMeshReference;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Leecher)
	float		VitesseLeecher;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Leecher)
	FName		SocketName;
	
protected:

	FVector		lastShipPosition;

	FVector		getFVectorToGoSocket();

	float		fTimer;

	// 0 aucun choix, 1 left , 2 right
	int			goLeftOrRight;
	
	void		goToFVector(FVector location,float deltaSeconds);
	
	bool getIsShipVisible(FVector startVector, FVector endVector);

	FVector		firstVectorPerpen;


	FVector		vectorSocketDeplacement;

	float getAngleBetweenTwoVector(FVector vector1, FVector vector2);


	virtual void Tick(float DeltaSeconds) override;
	
};
