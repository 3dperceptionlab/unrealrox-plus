// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/StaticMeshActor.h"
#include "ROXTypes.h"
#include "ROXJsonParser.h"
#include "ROXSceneManager.generated.h"

UCLASS()
class UNREALROX_PLUS_API AROXSceneManager : public AActor
{
	GENERATED_BODY()


public:
	/* If checked, Object Mask images will be generated changing all objects materials for plain color ones. This is slower, but it overcomes the limitation of 255 objects for the custom stencil approach */
	UPROPERTY(EditAnywhere)
	bool generate_masks_changing_materials;

	/* Directory where the folder for storing generated images from rebuilt sequences will be created */
	UPROPERTY(EditAnywhere, Category = ROX_GroundTruth)
	FString screenshots_save_directory;
	/* Folder where generated images from rebuilt sequences will be stored */
	UPROPERTY(EditAnywhere, Category = ROX_GroundTruth)
	FString screenshots_folder;

	// Advanced Display
	/* String for filtering actors. Useful for avoiding duplicates when working from the editor */
	UPROPERTY(EditAnywhere, AdvancedDisplay)
	FString Persistence_Level_Filter_Str;

protected:
	// Cached properties
	TArray<AStaticMeshActor*> CachedSM;
	TArray<bool> CachedSM_Gravity;
	TArray<bool> CachedSM_Physics;

	TMap<AActor*, TArray<FROXMeshComponentMaterials>> MeshMaterials;
	bool isMaskedMaterial;
	UMaterial* MaskPlainMat;

	ROXJsonParser* JsonParser;
	FROXFrame currentFrame;

private:

public:	
	// Sets default values for this actor's properties
	AROXSceneManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void CacheStaticMeshActors();
	void CacheSceneActors(const TArray<FROXPawnInfo> &PawnsInfo, const TArray<FROXCameraConfig> &CameraConfigs);
	void PrepareMaterials();
	FColor AssignColor(int idx_in);
	int GetIdxFromColor(FColor color);

	UFUNCTION(BlueprintCallable, Category = "ROXSceneManager")
	void ToggleActorMaterials();
	UFUNCTION(BlueprintCallable, Category = "ROXSceneManager")
	bool SetMaskedMaterials(bool bPlainColorMat);
	
};
