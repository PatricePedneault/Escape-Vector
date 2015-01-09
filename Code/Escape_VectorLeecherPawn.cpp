

#include "Escape_Vector.h"
#include "Escape_VectorLeecherPawn.h"
#include <math.h>  


AEscape_VectorLeecherPawn::AEscape_VectorLeecherPawn(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	MeshLeecher = PCIP.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("MeshLeecher"));

	capsuleComponentLeecher = PCIP.CreateDefaultSubobject<UCapsuleComponent>(this, TEXT("capsuleComponentLeecher"));

	sceneComponentLeecher = PCIP.CreateDefaultSubobject<USceneComponent>(this, TEXT("sceneComponentLeecher"));

	RootComponent = sceneComponentLeecher;

	capsuleComponentLeecher->AttachParent = sceneComponentLeecher;

	MeshLeecher->AttachParent = sceneComponentLeecher;

	MeshLeecher->SetWorldScale3D(FVector(10,10,10));

	PrimaryActorTick.bCanEverTick = true;

	shipStaticMeshReference = NULL;

	shipSkeletalMeshReference = NULL;

	VitesseLeecher = 1000;

	SocketName = "";

	goLeftOrRight = 0;

	lastShipPosition = FVector(0,0,0);

};

// Méthode qui permet lorsque l'ennemi a recu un nom de socket qui se trouve sur le transporteur(donc une location où se diriger pour s'accrocher) de renvoyer 
// une position où l'ennemi doit aller mais pas directement la position du socket. Le but est que le l'ennemi ne se dirige pas directement sur le socket, pour cause
// que celui-ci traversait certaine fois le vaisseau. Au final la méthode aura retourner plusieurs vecteur apres plusieurs appelles et fait diriger l'ennemi sur le socket
// en effectuant un cercle comme déplacement.
FVector AEscape_VectorLeecherPawn::getFVectorToGoSocket()
{
	FVector vectorPerpen_1;
	FVector vectorPerpen_2;
	FVector perpenticulaire;


	firstVectorPerpen = firstVectorPerpen + (shipSkeletalMeshReference->GetSocketLocation(SocketName) - vectorSocketDeplacement);
	vectorSocketDeplacement = shipSkeletalMeshReference->GetSocketLocation(SocketName);
	
	// Apres un certain angle on dirige directement l'ennemi vers le socket
	if (getAngleBetweenTwoVector((shipSkeletalMeshReference->GetSocketLocation(SocketName) - this->GetActorLocation()), firstVectorPerpen) > 70)
	{
		return shipSkeletalMeshReference->GetSocketLocation(SocketName);
	}


	perpenticulaire.X = shipSkeletalMeshReference->GetSocketLocation(SocketName).X - this->GetActorLocation().X;
	perpenticulaire.Y = shipSkeletalMeshReference->GetSocketLocation(SocketName).Y - this->GetActorLocation().Y;

	// On calcul les deux verteurs perpenticulaire au vecteur entre le vaisseau et l'ennemi (un sera à droite l'autre a gaucge)
	vectorPerpen_1.X = shipSkeletalMeshReference->GetSocketLocation(SocketName).X - perpenticulaire.Y;
	vectorPerpen_1.Y = shipSkeletalMeshReference->GetSocketLocation(SocketName).Y + perpenticulaire.X;
	vectorPerpen_1.Z = shipSkeletalMeshReference->GetSocketLocation(SocketName).Z;

	vectorPerpen_2.X = shipSkeletalMeshReference->GetSocketLocation(SocketName).X + perpenticulaire.Y;
	vectorPerpen_2.Y = shipSkeletalMeshReference->GetSocketLocation(SocketName).Y - perpenticulaire.X;
	vectorPerpen_2.Z = shipSkeletalMeshReference->GetSocketLocation(SocketName).Z;


	// Permet de retourner le bon vecteur perpenticulaire au transporteur (un sera vers l'extérieur et l'autre traversera le vaisseau) et de 
	// ne jamais changer en chemain
	if (goLeftOrRight !=0)
	{
		if (goLeftOrRight == 1)
		{
			return vectorPerpen_2;
		}
		return vectorPerpen_1;
	}

	// Si c'est la première appelle de méthode, on regarde de quel côté le vecteur perpenticulaire est le plus court, ainsi celui qui traverse le vaisseau sera plus loin donc on prend l'autre
	if (vectorPerpen_1.Dist(vectorPerpen_1, shipSkeletalMeshReference->GetComponentLocation()) > vectorPerpen_1.Dist(vectorPerpen_2, shipSkeletalMeshReference->GetComponentLocation()))
	{
		goLeftOrRight = 2;
		firstVectorPerpen = vectorPerpen_1;
		return vectorPerpen_1;
	}
	else
	{
		goLeftOrRight = 1;
		firstVectorPerpen = vectorPerpen_2;
		return vectorPerpen_2;
	}
	
}

// Méthode qui donne l'angle entre deux vecteurs
float AEscape_VectorLeecherPawn::getAngleBetweenTwoVector(FVector vector1, FVector vector2)
{
	float var1 = (vector1.X * vector2.X) + (vector1.Y * vector2.Y);

	float var2 = var1 / (sqrt((pow(vector1.X, 2) + pow(vector1.Y, 2)))* sqrt((pow(vector2.X, 2) + pow(vector2.Y, 2))));

	float angle = acos(var2) * 180.0 / PI;

	return angle;
}

// Méthode qui permet de lancer un "trace" pour savoir si le transporteur est visible pour l'ennemi, si oui la variable
// "ActorClassName" contiendra le nom du transporteur qui est "Ship_C"
bool AEscape_VectorLeecherPawn::getIsShipVisible(FVector startVector, FVector endVector)
{
	
	FCollisionQueryParams CombatCollisionQuery(FName(TEXT("CombatTrace")), true, NULL); //trace complex, ignore nothing
	CombatCollisionQuery.bTraceComplex = true;
	CombatCollisionQuery.bTraceAsyncScene = false;
	CombatCollisionQuery.bReturnPhysicalMaterial = false;
	CombatCollisionQuery.TraceTag = "traceShipIsVisible";
		
	FHitResult RV_Hit(ForceInit);

	//GetWorld()->DebugDrawTraceTag = "traceShipIsVisible";

	bool DidTrace = GetWorld()->LineTraceSingle(RV_Hit,  startVector,endVector,  ECC_Visibility,CombatCollisionQuery);

	if (DidTrace)
	{
		FString ActorClassName = RV_Hit.GetActor()->GetActorClass()->GetName();

		if (ActorClassName == "Ship_C")
		{
			return true;
		}
		
	}

	return false;
}


// Méthode qui permet de diriger l'ennemi dans un direction (vecteur)
void AEscape_VectorLeecherPawn::goToFVector(FVector location ,float deltaSeconds)
{
	// On calcul le vecteur entre le leecher et le lieu où aller
	FVector vecteurDirecteur = FVector((location.X - this->GetActorLocation().X), (location.Y - this->GetActorLocation().Y), (location.Z - this->GetActorLocation().Z));
	

	// On calcul la norme du vecteur 
	float normeVector = sqrt(pow(vecteurDirecteur.X, 2) + pow(vecteurDirecteur.Y, 2) + pow(vecteurDirecteur.Z, 2));

	// Calcul du vecteur unitaire du vecteur vecteurDirecteur
	FVector unitaireVecteurDirecteur = (1 / normeVector) * vecteurDirecteur;

	// On calcul la position d'un point un peut plus loin que la position du leecher sur le vecteurUnitaire
	FVector vecteurFutureLocation = FVector((((deltaSeconds * VitesseLeecher) *unitaireVecteurDirecteur.X) + this->GetActorLocation().X), (((deltaSeconds * VitesseLeecher) *unitaireVecteurDirecteur.Y) + this->GetActorLocation().Y), (((deltaSeconds * VitesseLeecher) *unitaireVecteurDirecteur.Z) + this->GetActorLocation().Z));

	// Calcul de la rotation du leecher en fonction du vecteur directeur
	FRotator rotationLeecher = vecteurDirecteur.Rotation();

	this->SetActorRotation(rotationLeecher);

	this->SetActorLocation(vecteurFutureLocation);
}

void AEscape_VectorLeecherPawn::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (shipSkeletalMeshReference != NULL)
	{
		// On regarde si l'ennemi a un nom de socket où il doit se diriger
		if (SocketName == "")
		{
			// On regarde si le transporteur est visible
			if (getIsShipVisible(this->GetActorLocation(), shipSkeletalMeshReference->GetComponentLocation()))
			{
				lastShipPosition = shipSkeletalMeshReference->GetComponentLocation();

				// on appelle la méthode pour déplacer l'ennemi
				goToFVector(lastShipPosition, DeltaSeconds);

			}
			else
			{
				// Si l'ennemi a perdu le transporteur de vu, celui-ci se dirigera vers la dernière position que le vaisseau a été vu
				if (lastShipPosition != FVector(0, 0, 0))
				{
					if (this->GetActorLocation().Dist(this->GetActorLocation(), lastShipPosition) > 50.0f)
					{
						goToFVector(lastShipPosition, DeltaSeconds);
					}
					else
					{
						this->SetActorLocation(lastShipPosition);
					}
				}
			}
		}
		else
		{
			// On demande à la méthode "getFVectorToGoSocket" où l'ennemi doit se diriger 
			FVector whereToGo = getFVectorToGoSocket();

			// Si la distance est plus petite que 100 on déplacement directement l'ennemi sur le socket du transporteur
			if (this->GetActorLocation().Dist(this->GetActorLocation(), shipSkeletalMeshReference->GetSocketLocation(SocketName)) < 100.0f)
			{

				this->SetActorLocation(shipSkeletalMeshReference->GetSocketLocation(SocketName));
				this->SetActorRotation(shipSkeletalMeshReference->GetSocketRotation(SocketName));
				return;
			}
			else
			{
				// On dirige l'ennemi vers la position que "whereToGo" contient
				goToFVector(whereToGo, DeltaSeconds);
			}
		}

	}
};



