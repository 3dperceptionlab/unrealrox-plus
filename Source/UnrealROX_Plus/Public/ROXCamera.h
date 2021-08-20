// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraActor.h"
#include "Engine/World.h"
#include "Engine/SceneCapture2D.h"
#include "Components/SceneCaptureComponent2D.h"
#include "ROXTypes.h"
#include "ROXSceneManager.h"
#include "ROXCamera.generated.h"

/**
 * 
 */
UCLASS()
class UNREALROX_PLUS_API AROXCamera : public ACameraActor
{
	GENERATED_BODY()

public:
	// ROXConfig

	/* Scene Manager reference. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ROX_Config)
	AROXSceneManager* SceneManager;

	bool generate_masks_changing_materials;

	/* Disable for not generating the structures to retrieve ground truth. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ROX_Config)
	bool GetGroundTruth;

	/* If checked, this camera will represent a stereo pair of cameras. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ROX_Config)
	bool isStereoCamera;
	/* Baseline in cm for this stereo camera pair. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ROX_Config, meta = (EditCondition = "isStereoCamera"))
	float StereoCameraBaseline;
	/* Field of view of this point of view. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ROX_Config, meta = (ClampMin = "5.0", ClampMax = "170.0", UIMin = "5.0", UIMax = "170.0"))
	float FOV;

	/* Width size for generated images */
	UPROPERTY(EditAnywhere, Category = ROX_Config)
	int generated_images_width;
	/* Height size for generated images */
	UPROPERTY(EditAnywhere, Category = ROX_Config)
	int generated_images_height;
	
	// ROXGroundTruth
	/* Directory where the folder for storing generated images from rebuilt sequences will be created */
	UPROPERTY(EditAnywhere, Category = ROX_GroundTruth)
	FString screenshots_save_directory;
	/* Folder where generated images from rebuilt sequences will be stored */
	UPROPERTY(EditAnywhere, Category = ROX_GroundTruth)
	FString screenshots_folder;

	/* If checked, RGB images (JPG RGB 8bit) will be generated for each frame of rebuilt sequences */
	UPROPERTY(EditAnywhere, Category = ROX_GroundTruth)
	bool generate_rgb;
	/* Format for RGB images (PNG ~3MB, JPG 95% ~800KB, JPG 80% ~120KB) */
	UPROPERTY(EditAnywhere, Category = ROX_GroundTruth)
	EROXRGBImageFormats format_rgb;
	/* If checked, Normal images (PNG RGB 8bit) will be generated for each frame of rebuilt sequences */
	UPROPERTY(EditAnywhere, Category = ROX_GroundTruth)
	bool generate_normal;
	/* If checked, Depth images (PNG Gray 16bit) will be generated for each frame of rebuilt sequences */
	UPROPERTY(EditAnywhere, Category = ROX_GroundTruth)
	bool generate_depth;
	/* If checked, Object Mask images (PNG RGB 8bit) will be generated for each frame of rebuilt sequences */
	UPROPERTY(EditAnywhere, Category = ROX_GroundTruth)
	bool generate_object_mask;
	/* If checked, Albedo images (PNG RGB 8bit) will be generated for each frame of rebuilt sequences */
	UPROPERTY(EditAnywhere, Category = ROX_GroundTruth)
	bool generate_albedo;
	/* If checked, a TXT file with depth in cm for each pixel will be printed (WARNING: large size ~2MB per file) */
	UPROPERTY(EditAnywhere, Category = ROX_GroundTruth, AdvancedDisplay)
	bool generate_depth_txt_cm;

	/* Seconds to wait since materials have been changed for all actors (0.2 is enough).*/
	UPROPERTY(EditAnywhere, Category = ROX_GroundTruth, AdvancedDisplay)
	float delay_change_materials;


	UPROPERTY(Category = GroundTruth, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USceneComponent* Stereo_R;
	UPROPERTY(Category = GroundTruth, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USceneComponent* Stereo_L;

private:

	/* SceneCapture structures for each type of ground truth image to obtain */
	UPROPERTY(Category = GroundTruth, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TArray<USceneCaptureComponent2D*> SceneCapture_VMs;
	UPROPERTY(Category = GroundTruth, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TArray<USceneCaptureComponent2D*> SceneCapture_VMs_L;

	UMaterial* NormalMat;
	UMaterial* MaskMat;

	TArray<EROXViewMode> VMActiveList;

	/**************************************
	************* FUNCTIONS ***************
	***************************************/
	
public:
	// Default contructor
	AROXCamera();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	// Destroy method
	virtual void BeginDestroy() override;

private:
	void InitSceneManager();
	void InitComponents();
	void InitVMActiveList();

	void SceneCapture_ConfigComponents();
	void SceneCapture_ConfigComp(USceneCaptureComponent2D* SceneCaptureComp, EROXViewMode vm, FName CompName);
	void SceneCapture_DisableComponents();
	void SceneCapture_DisableStereoComponents();
	void SceneCapture_ConfigStereo();
	void SceneCapture_ConfigStereoPair(USceneCaptureComponent2D* SCComp_R, USceneCaptureComponent2D* SCComp_L);

	void SaveImageData(USceneCaptureComponent2D* SceneCaptureComp_R, USceneCaptureComponent2D* SceneCaptureComp_L, const EROXViewMode viewmode, FString Filename);
	void SaveRTImage(USceneCaptureComponent2D* SceneCaptureComp, const EROXViewMode viewmode, FString Filename);
	void SaveRTDepthImage(USceneCaptureComponent2D* SceneCaptureComp, FString Filename);
	void SaveAnyImage(EROXViewMode vm, FString Filename);

public:
	UFUNCTION(CallInEditor, BlueprintCallable, Category = "ROXCamera")
	void SaveRGBImage(FString Filename = "");
	UFUNCTION(CallInEditor, BlueprintCallable, Category = "ROXCamera")
	void SaveDepthImage(FString Filename = "");
	UFUNCTION(CallInEditor, BlueprintCallable, Category = "ROXCamera")
	void SaveNormalImage(FString Filename = "");
	UFUNCTION(CallInEditor, BlueprintCallable, Category = "ROXCamera")
	void SaveMaskImage(FString Filename = "");
	UFUNCTION(CallInEditor, BlueprintCallable, Category = "ROXCamera")
	void SaveAlbedoImage(FString Filename = "");
	UFUNCTION(CallInEditor, BlueprintCallable, Category = "ROXCamera")
	void SaveGTImages(bool rgb, bool depth, bool mask, bool normal, bool albedo, FString Filename = "");

	friend class AROXServer;
};
