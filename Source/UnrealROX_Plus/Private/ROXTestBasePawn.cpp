// Fill out your copyright notice in the Description page of Project Settings.


#include "ROXTestBasePawn.h"

// Sets default values
AROXTestBasePawn::AROXTestBasePawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AROXTestBasePawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AROXTestBasePawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AROXTestBasePawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AROXTestBasePawn::EmplaceRotation(FName n, FRotator r)
{
	UE_LOG(LogTemp, Warning, TEXT("%s"), *n.ToString());
	if (SocketsTransf.Contains(n))
	{
		SocketsTransf[n].SetRotation(FQuat(r));
	}
	else
		SocketsTransf.Emplace(n, r);
}

void AROXTestBasePawn::EmplaceTranslation(FName n, FVector t)
{
	SocketsTransf.Emplace(n, FTransform(t));
}