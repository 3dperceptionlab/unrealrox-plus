// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ROXLib.generated.h"

/**
 * 
 */
UCLASS()
class UNREALROX_PLUS_API UROXLib : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
	public:

		UFUNCTION(CallInEditor, BlueprintCallable, Category = "UnrealROX")
		static FString GetDateTimeString()
		{
			return 	FDateTime::Now().ToString(TEXT("%Y%m%d%-%H%M%S"));
		}	
};