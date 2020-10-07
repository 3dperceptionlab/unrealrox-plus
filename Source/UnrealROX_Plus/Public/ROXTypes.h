// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Camera/CameraActor.h"
#include "ROXTypes.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogUnrealROX, Log, All);

UENUM(BlueprintType)
enum class EROXViewMode : uint8
{
	RVM_RGB			UMETA(DisplayName = "RGB"),
	RVM_Normal		UMETA(DisplayName = "Normal"),
	RVM_Depth		UMETA(DisplayName = "Depth"),
	RVM_Mask		UMETA(DisplayName = "Mask"),
	RVM_Albedo		UMETA(DisplayName = "Albedo")
};
// Contains the types of images that will be generated for a concrete execution.
static TArray<EROXViewMode> EROXViewModeList;
// First and last elements from EROXViewModeList (updated in runtime)
static EROXViewMode EROXViewMode_First = EROXViewMode::RVM_RGB;
static EROXViewMode EROXViewMode_Last = EROXViewMode::RVM_Albedo;

class FROXTypes
{
public:
	static FString GetViewmodeString(EROXViewMode vm)
	{
		FString res("rgb");
		switch (vm)
		{
		case EROXViewMode::RVM_RGB: res = "rgb";
			break;
		case EROXViewMode::RVM_Depth: res = "depth";
			break;
		case EROXViewMode::RVM_Mask: res = "mask";
			break;
		case EROXViewMode::RVM_Normal: res = "normal";
			break;
		case EROXViewMode::RVM_Albedo: res = "albedo";
			break;
		}
		return res;
	}
};


// Lists the different formats available for RGB images.
UENUM(BlueprintType)
enum class EROXRGBImageFormats : uint8
{
	RIF_PNG			UMETA(DisplayName = "PNG"),
	RIF_JPG95		UMETA(DisplayName = "JPG (95%)"),
	RVM_JPG80		UMETA(DisplayName = "JPG (80%)")
};

UENUM(BlueprintType)
enum class EROXMeshState : uint8
{
	RMS_Grasped_L		UMETA(DisplayName = "Grasped Left"),
	RMS_Grasped_R		UMETA(DisplayName = "Grasped Right"),
	RMS_Interacted		UMETA(DisplayName = "Interacted"),
	RMS_Body			UMETA(DisplayName = "Body"),
	RMS_None			UMETA(DisplayName = "None")
};

USTRUCT()
struct FROXActorState
{
	GENERATED_USTRUCT_BODY()

	FVector Position;
	FRotator Rotation;

	FROXActorState()
	{}
};

USTRUCT()
struct FROXActorStateExtended
{
	GENERATED_USTRUCT_BODY()

	FVector Position;
	FRotator Rotation;
	FVector BoundingBox_Min;
	FVector BoundingBox_Max;

	FROXActorStateExtended()
	{}
};

USTRUCT()
struct FROXSkeletonState
{
	GENERATED_USTRUCT_BODY()

	FVector Position;
	FRotator Rotation;
	TMap<FString, FROXActorState> Bones;

	FROXSkeletonState()
	{}
};

USTRUCT()
struct FROXFrame
{
	GENERATED_USTRUCT_BODY()

	int n_frame;
	int n_frame_generated;
	float time_stamp;
	TMap<FString, FROXActorState> Cameras;
	TMap<FString, FROXActorStateExtended> Objects;
	TMap<FString, FROXSkeletonState> Skeletons;

	FROXFrame()
	{}
};

USTRUCT()
struct FROXCameraConfig
{
	GENERATED_USTRUCT_BODY()

	FString CameraName;
	float StereoBaseline;
	float FieldOfView;

	FROXCameraConfig()
	{}
};

USTRUCT()
struct FROXPawnInfo
{
	GENERATED_USTRUCT_BODY()

	FString PawnName;
	int NumBones;

	FROXPawnInfo()
	{}
};

USTRUCT()
struct FROXMeshComponentMaterials
{
	GENERATED_USTRUCT_BODY()

	UMeshComponent* MeshComponent;
	TArray<UMaterialInterface*> DefaultMaterials;
	FLinearColor MaskMaterialLinearColor;
	FColor MaskMaterialColor;
	UMaterialInstanceDynamic* MaskMaterial;

	FROXMeshComponentMaterials()
	{}
};

USTRUCT()
struct FROXSceneObject
{
	GENERATED_USTRUCT_BODY()

	FName instance_name;
	FColor instance_color;
	FName instance_class;

	FROXSceneObject()
	{}
};
