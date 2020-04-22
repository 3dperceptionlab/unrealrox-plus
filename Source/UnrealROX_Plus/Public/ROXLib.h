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
		static void TakeGroundTruth(bool RGB, bool Depth, bool SegMask, bool Normal);

		UFUNCTION(CallInEditor, BlueprintCallable, Category = "UnrealROX")
		static FString GetDateTimeString()
		{
			return 	FDateTime::Now().ToString(TEXT("%Y%m%d%-%H%M%S"));
		}

		//static bool bUnderExecution;

		//UPROPERTY(BlueprintAssignable, Category = "UnrealROX Events")
		//FUROXFrameCompleted FrameComleted;
	
};

/*
#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ROXTypes.h"
#include "ROXScene.generated.h"
UCLASS()
static class UNREALROX_PLUS_API ROXScene : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

private:
	static TMap<AActor*, TArray<FROXMeshComponentMaterials>> MeshMaterials;
	static bool isMaskedMaterial;
	static bool isMaterialReady;

public:

	UFUNCTION(CallInEditor, BlueprintCallable, Category = "UnrealROX")
		static void PrepareMaterials();

	UFUNCTION(CallInEditor, BlueprintCallable, Category = "UnrealROX")
		static void ToggleActorMaterials();

	UFUNCTION(CallInEditor, BlueprintCallable, Category = "UnrealROX")
		static bool IsMaskedMaterial();

	//static FColor AssignColor(int idx_in);
	//static int GetIdxFromColor(FColor color);

};


void ROXScene::PrepareMaterials()
{
	isMaterialReady = true;
}

void ROXScene::ToggleActorMaterials()
{
	isMaskedMaterial = !isMaskedMaterial;
}

bool ROXScene::IsMaskedMaterial()
{
	return isMaskedMaterial;
}
*/