// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ROX.generated.h"

/**
 * 
 */
UCLASS()
class UNREALROX_PLUS_API UROX : public UObject
{
	GENERATED_BODY()
	
	
	UFUNCTION(CallInEditor, BlueprintCallable, Category = "UnrealROX")
	void TestFunction();
	
};
