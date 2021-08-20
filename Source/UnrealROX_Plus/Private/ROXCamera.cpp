// Fill out your copyright notice in the Description page of Project Settings.

#include "ROXCamera.h"
#include "Engine/TextureRenderTarget2D.h"
#include "UObject/ConstructorHelpers.h"
#include "UObject/UObjectIterator.h"
#include "CommandLine.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "ImageUtils.h"
#include "Modules/ModuleManager.h"
#include "TimerManager.h"
#include "ROXTaskUtils.h"
#include "ROXLib.h"

//bool AROXCamera::SceneManagerReady = false;

AROXCamera::AROXCamera() :
	GetGroundTruth(true),
	isStereoCamera(false),
	StereoCameraBaseline(0),
	generate_rgb(true),
	format_rgb(EROXRGBImageFormats::RIF_JPG95),
	generate_normal(true),
	generate_depth(true),
	generate_object_mask(true),
	generate_depth_txt_cm(false),
	generate_masks_changing_materials(false),
	delay_change_materials(0.2),
	screenshots_save_directory(),
	screenshots_folder("GeneratedSequences"),
	generated_images_width(1920),
	generated_images_height(1080),
	FOV(90.0f)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	// Init screenshots save directory to the project folder
	screenshots_save_directory = FPaths::ProjectUserDir();

	// Init materials for viewmodes
	NormalMat = nullptr;
	static ConstructorHelpers::FObjectFinder<UMaterial> matNormal(TEXT("/Game/ROX/ViewModeMats/WorldNormal.WorldNormal"));
	if (matNormal.Succeeded())
	{
		NormalMat = (UMaterial*)matNormal.Object;
	}

	MaskMat = nullptr;
	static ConstructorHelpers::FObjectFinder<UMaterial> matMask(TEXT("/Game/ROX/ViewModeMats/PPM_SegMask.PPM_SegMask"));
	if (matMask.Succeeded())
	{
		MaskMat = (UMaterial*)matMask.Object;
	}

	// Init SceneCapture structures
	InitComponents();
}

void AROXCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AROXCamera::BeginPlay()
{
	Super::BeginPlay();

	// Check that the last character of directories is a slash
	if (screenshots_save_directory[screenshots_save_directory.Len() - 1] != '/')
	{
		screenshots_save_directory += "/";
	}

	// Configure SceneCapture structures
	InitVMActiveList();
	InitSceneManager();
	SceneCapture_ConfigComponents();
}

void AROXCamera::BeginDestroy()
{
	Super::BeginDestroy();
}

void AROXCamera::InitVMActiveList()
{
	if (generate_rgb) VMActiveList.Add(EROXViewMode::RVM_RGB);
	if (generate_albedo) VMActiveList.Add(EROXViewMode::RVM_Albedo);
	if (generate_depth) VMActiveList.Add(EROXViewMode::RVM_Depth);
	if (generate_normal) VMActiveList.Add(EROXViewMode::RVM_Normal);
	if (generate_object_mask) VMActiveList.Add(EROXViewMode::RVM_Mask);
}

void AROXCamera::InitSceneManager()
{
	/*for (TObjectIterator<AROXSceneManager> Itr; Itr && SceneManager == NULL; ++Itr)
	{
		SceneManager = *Itr;
		generate_masks_changing_materials = SceneManager->generate_masks_changing_materials;
		if (generate_masks_changing_materials)
		{
			FString status_msg = " " + ((*Itr)->GetFullName()) + " " + this->GetFullName();
			UE_LOG(LogUnrealROX, Warning, TEXT("%s"), *status_msg);
		}
	}*/

	if (SceneManager == NULL)
	{
		FString status_msg = "No ROXSceneManager in the scene.";
		UE_LOG(LogUnrealROX, Warning, TEXT("%s"), *status_msg);
	}
	else
	{
		generate_masks_changing_materials = SceneManager->generate_masks_changing_materials;
	}
}


void AROXCamera::InitComponents()
{	
	Stereo_R = CreateDefaultSubobject<USceneComponent>("Stereo_R");
	Stereo_R->SetupAttachment(GetRootComponent());
	Stereo_R->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	Stereo_L = CreateDefaultSubobject<USceneComponent>("Stereo_L");
	Stereo_L->SetupAttachment(GetRootComponent());
	Stereo_L->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));

	for (EROXViewMode vm : EROXViewModeList)
	{
		FName sc_name = FName(*("SceneCapture_" + FROXTypes::GetViewmodeString(vm)));
		USceneCaptureComponent2D* SceneCapture_aux = CreateDefaultSubobject<USceneCaptureComponent2D>(sc_name);
		SceneCapture_aux->SetupAttachment(Stereo_R);
		SceneCapture_aux->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
		SceneCapture_VMs.Add(SceneCapture_aux);

		FName sc_name_L = FName(*("SceneCapture_" + FROXTypes::GetViewmodeString(vm) + "_L"));
		SceneCapture_aux = CreateDefaultSubobject<USceneCaptureComponent2D>(sc_name_L);
		SceneCapture_aux->SetupAttachment(Stereo_L);
		SceneCapture_aux->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
		SceneCapture_VMs_L.Add(SceneCapture_aux);
	}
}


void AROXCamera::SceneCapture_ConfigComponents()
{
	if (GetGroundTruth)
	{
		for (int i = 0; i < EROXViewModeList.Num(); ++i)
		{
			SceneCapture_ConfigComp(SceneCapture_VMs[i], EROXViewModeList[i], SceneCapture_VMs[i]->GetFName());
		}

		if (isStereoCamera)
		{
			for (int i = 0; i < EROXViewModeList.Num(); ++i)
			{
				SceneCapture_ConfigComp(SceneCapture_VMs_L[i], EROXViewModeList[i], SceneCapture_VMs_L[i]->GetFName());
			}
			SceneCapture_ConfigStereo();
		}
		else
		{
			SceneCapture_DisableStereoComponents();
		}

	}
	else
	{
		SceneCapture_DisableComponents();
	}
}

void AROXCamera::SceneCapture_ConfigComp(USceneCaptureComponent2D* SceneCaptureComp, EROXViewMode vm, FName CompName)
{
	SceneCaptureComp->TextureTarget = NewObject<UTextureRenderTarget2D>();
	SceneCaptureComp->TextureTarget->InitAutoFormat(generated_images_width, generated_images_height);
	SceneCaptureComp->PostProcessSettings.bOverride_AutoExposureMinBrightness = true;
	SceneCaptureComp->PostProcessSettings.bOverride_AutoExposureMaxBrightness = true;
	SceneCaptureComp->PostProcessSettings.AutoExposureMinBrightness = 1.0f;
	SceneCaptureComp->PostProcessSettings.AutoExposureMaxBrightness = 1.0f;
	SceneCaptureComp->FOVAngle = FOV;

	switch (vm)
	{
	case EROXViewMode::RVM_RGB:
		SceneCaptureComp->CaptureSource = ESceneCaptureSource::SCS_FinalColorLDR;
		SceneCaptureComp->TextureTarget->RenderTargetFormat = ETextureRenderTargetFormat::RTF_RGBA8;
		SceneCaptureComp->TextureTarget->TargetGamma = 2.2;
		SceneCaptureComp->PostProcessBlendWeight = 0;
		break;
	case EROXViewMode::RVM_Depth:
		SceneCaptureComp->CaptureSource = ESceneCaptureSource::SCS_SceneDepth;
		SceneCaptureComp->TextureTarget->RenderTargetFormat = ETextureRenderTargetFormat::RTF_RGBA16f;
		SceneCaptureComp->TextureTarget->TargetGamma = 0;
		break;
	case EROXViewMode::RVM_Mask:
		if (!generate_masks_changing_materials)
		{
			SceneCaptureComp->CaptureSource = ESceneCaptureSource::SCS_FinalColorLDR;
			SceneCaptureComp->TextureTarget->RenderTargetFormat = ETextureRenderTargetFormat::RTF_RGBA8;
			check(MaskMat);
			SceneCaptureComp->PostProcessSettings.WeightedBlendables.Array.Empty();
			SceneCaptureComp->PostProcessSettings.AddBlendable(MaskMat, 1);
			SceneCaptureComp->PostProcessBlendWeight = 1;
		}
		else
		{
			SceneCaptureComp->CaptureSource = ESceneCaptureSource::SCS_BaseColor;
			SceneCaptureComp->TextureTarget->RenderTargetFormat = ETextureRenderTargetFormat::RTF_RGBA8;
			SceneCaptureComp->TextureTarget->TargetGamma = 1;
		}
		break;
	case EROXViewMode::RVM_Normal:
		SceneCaptureComp->CaptureSource = ESceneCaptureSource::SCS_FinalColorLDR;
		SceneCaptureComp->TextureTarget->RenderTargetFormat = ETextureRenderTargetFormat::RTF_RGBA8;
		check(NormalMat);
		SceneCaptureComp->PostProcessSettings.WeightedBlendables.Array.Empty();
		SceneCaptureComp->PostProcessSettings.AddBlendable(NormalMat, 1);
		SceneCaptureComp->PostProcessBlendWeight = 1;
		break;
	case EROXViewMode::RVM_Albedo:
		SceneCaptureComp->CaptureSource = ESceneCaptureSource::SCS_BaseColor;
		SceneCaptureComp->TextureTarget->RenderTargetFormat = ETextureRenderTargetFormat::RTF_RGBA8;
		SceneCaptureComp->TextureTarget->TargetGamma = 1;
		break;
	}
}

void AROXCamera::SceneCapture_DisableComponents()
{
	for (USceneCaptureComponent2D* sc : SceneCapture_VMs)
	{
		sc->Deactivate();
	}
	SceneCapture_DisableStereoComponents();
}

void AROXCamera::SceneCapture_DisableStereoComponents()
{
	for (USceneCaptureComponent2D* sc : SceneCapture_VMs_L)
	{
		sc->Deactivate();
	}
}

void AROXCamera::SceneCapture_ConfigStereo()
{
	Stereo_R->SetRelativeLocation(FVector(0.0f, StereoCameraBaseline * (0.5f), 0.0f));
	Stereo_L->SetRelativeLocation(FVector(0.0f, StereoCameraBaseline * (-0.5f), 0.0f));
}


void AROXCamera::SaveRTImage(USceneCaptureComponent2D* SceneCaptureComp, const EROXViewMode viewmode, FString Filename)
{
	FString CameraName = GetActorLabel();
	FString FullFilename = screenshots_save_directory + screenshots_folder + "/" + CameraName + "/" + FROXTypes::GetViewmodeString(viewmode) + "/" + Filename;

	int32 Width = SceneCaptureComp->TextureTarget->SizeX;
	int32 Height = SceneCaptureComp->TextureTarget->SizeY;
	FTextureRenderTargetResource* RenderTargetResource;
	RenderTargetResource = SceneCaptureComp->TextureTarget->GameThread_GetRenderTargetResource();

	// Configure image
	EImageFormat ImageFormat = EImageFormat::PNG;
	FString FullFilenameExtension = FullFilename + ".png";
	int32 ComprQuality = 0;
	if (viewmode == EROXViewMode::RVM_RGB && (format_rgb == EROXRGBImageFormats::RVM_JPG80 || format_rgb == EROXRGBImageFormats::RIF_JPG95))
	{
		ImageFormat = EImageFormat::JPEG;
		FullFilenameExtension = FullFilename + ".jpg";
		ComprQuality = (format_rgb == EROXRGBImageFormats::RIF_JPG95 ? 95 : 80);
	}

	static IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
	static TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(ImageFormat);

	TArray<uint8> FinalImageData;

	// Depth
	if (viewmode == EROXViewMode::RVM_Depth)
	{
		TArray<FFloat16Color> ImageData_Depth;
		ImageData_Depth.AddUninitialized(Width * Height);
		RenderTargetResource->ReadFloat16Pixels(ImageData_Depth);

		if (ImageData_Depth.Num() != 0 && ImageData_Depth.Num() == Width * Height)
		{
			TArray<uint16> Grayscaleuint16Data;
			for (auto px : ImageData_Depth)
			{
				// Max value float16: 65504.0 -> It is cm, so it can represent up to 655.04m
				// Max value uint16: 65535 (65536 different values) -> It is going to be mm, so it can represent up to 65.535m - 6553.5cm
				float pixelCm = px.R.GetFloat();
				if (pixelCm > 6553.4f || pixelCm < 0.3f)
				{
					Grayscaleuint16Data.Add(0);
				}
				else
				{
					float pixelMm = pixelCm * 10.0f;
					Grayscaleuint16Data.Add((uint16)floorf(pixelMm + 0.5f));
				}
			}
			ImageWrapper->SetRaw(Grayscaleuint16Data.GetData(), Grayscaleuint16Data.GetAllocatedSize(), Width, Height, ERGBFormat::Gray, 16);
			FinalImageData = ImageWrapper->GetCompressed();

			if (generate_depth_txt_cm)
			{
				FString DepthCm("");
				for (auto px : ImageData_Depth)
				{
					DepthCm += FString::SanitizeFloat(px.R.GetFloat()) + "\n";
				}
				(new FAutoDeleteAsyncTask<FROXWriteStringTask>(DepthCm, FullFilename + ".txt"))->StartBackgroundTask();
			}
		}
	}

	// RGB, Normal, Mask, Albedo
	else
	{
		TArray<FColor> ImageData;
		ImageData.AddUninitialized(Width * Height);

		TArray<FLinearColor> ImageDataLC;
		ImageDataLC.AddUninitialized(Width * Height);
		RenderTargetResource->ReadLinearColorPixels(ImageDataLC);

		if (ImageData.Num() != 0 && ImageData.Num() == Width * Height)
		{
			// sRGB Color space. True for Mask and Albedo images, False otherwise
			bool sRGB = (viewmode == EROXViewMode::RVM_Mask || viewmode == EROXViewMode::RVM_Albedo);

			for (int i = 0; i < ImageData.Num(); ++i)
			{
				ImageData[i] = ImageDataLC[i].ToFColor(sRGB);
				ImageData[i].A = 255;
			}
		}
		ImageWrapper->SetRaw(ImageData.GetData(), ImageData.GetAllocatedSize(), Width, Height, ERGBFormat::BGRA, 8);
		FinalImageData = ImageWrapper->GetCompressed(ComprQuality);
	}
	
	// Save image
	(new FAutoDeleteAsyncTask<FROXScreenshotTask>(FinalImageData, FullFilenameExtension))->StartBackgroundTask();
}


void AROXCamera::SaveImageData(USceneCaptureComponent2D* SceneCaptureComp_R, USceneCaptureComponent2D* SceneCaptureComp_L, const EROXViewMode viewmode, FString Filename)
{
	if (GetGroundTruth)
	{
		if (Filename.IsEmpty())
		{
			Filename = UROXLib::GetDateTimeString();
		}
		if (!isStereoCamera)
		{
			SaveRTImage(SceneCaptureComp_R, viewmode, Filename);
		}
		else
		{
			SaveRTImage(SceneCaptureComp_R, viewmode, Filename + "_R");
			SaveRTImage(SceneCaptureComp_L, viewmode, Filename + "_L");
		}
	}
}

void AROXCamera::SaveAnyImage(EROXViewMode vm, FString Filename)
{
	uint8 i = (uint8)vm;
	// Introduce a little delay if material must be changed
	if (SceneManager != NULL && SceneManager->SetMaskedMaterials(vm == EROXViewMode::RVM_Mask))
	{
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateUObject(this, &AROXCamera::SaveImageData, SceneCapture_VMs[i], SceneCapture_VMs_L[i], vm, Filename), delay_change_materials, false);
	}
	else
	{
		SaveImageData(SceneCapture_VMs[i], SceneCapture_VMs_L[i], vm, Filename);
	}
}

void AROXCamera::SaveRGBImage(FString Filename)
{
	SaveAnyImage(EROXViewMode::RVM_Normal, Filename);
}

void AROXCamera::SaveNormalImage(FString Filename)
{
	SaveAnyImage(EROXViewMode::RVM_Normal, Filename);
}

void AROXCamera::SaveDepthImage(FString Filename)
{
	SaveAnyImage(EROXViewMode::RVM_Depth, Filename);
}

void AROXCamera::SaveAlbedoImage(FString Filename)
{
	SaveAnyImage(EROXViewMode::RVM_Albedo, Filename);
}

void AROXCamera::SaveMaskImage(FString Filename)
{
	SaveAnyImage(EROXViewMode::RVM_Mask, Filename);
}

void AROXCamera::SaveGTImages(bool rgb, bool depth, bool mask, bool normal, bool albedo, FString Filename)
{
	TArray<EROXViewMode> VM_list;
	if (rgb) VM_list.Add(EROXViewMode::RVM_RGB);
	if (albedo) VM_list.Add(EROXViewMode::RVM_Albedo);
	if (depth) VM_list.Add(EROXViewMode::RVM_Depth);
	if (normal) VM_list.Add(EROXViewMode::RVM_Normal);
	if (mask) VM_list.Add(EROXViewMode::RVM_Mask);

	for (EROXViewMode vm : VM_list)
	{
		SaveAnyImage(vm, Filename);
	}
}