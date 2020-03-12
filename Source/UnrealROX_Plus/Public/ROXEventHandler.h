// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ROXEventHandler.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FROXTracker_OnPreFrameGenerated);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FROXTracker_OnNextJson);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UNREALROX_PLUS_API UROXEventHandler : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UROXEventHandler();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "ROXTracker Events")
	FROXTracker_OnPreFrameGenerated OnPreFrameGenerated;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "ROXTracker Events")
	FROXTracker_OnNextJson OnNextJson;
	
};
