// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ROXSingleton.h"
#include "ROXSingletonBPLib.generated.h"

/**
 * 
 */
UCLASS()
class UNREALROX_PLUS_API UROXSingletonBPLib : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintPure, Category = "ROXTracker Events")
	static UROXSingleton* GetROXEvents();
	
	
};
