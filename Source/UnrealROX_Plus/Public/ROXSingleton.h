// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ROXEventHandler.h"
#include "ROXSingleton.generated.h"

class UROXEventHandler;

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class UNREALROX_PLUS_API UROXSingleton : public UObject
{
	GENERATED_BODY()
	
public:
	UROXSingleton(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(BlueprintReadOnly, Category = "ROXTracker Events")
	UROXEventHandler* eventsHandler;
	
	
};
