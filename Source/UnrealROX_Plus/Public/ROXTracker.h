// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ROXTracker.generated.h"

UCLASS()
class UNREALROX_PLUS_API AROXTracker : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AROXTracker();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	
};
