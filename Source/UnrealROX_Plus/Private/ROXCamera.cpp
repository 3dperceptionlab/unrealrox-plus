// Fill out your copyright notice in the Description page of Project Settings.

#include "ROXCamera.h"
#include "Engine/TextureRenderTarget2D.h"
#include "UObject/ConstructorHelpers.h"
#include "CommandLine.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "ImageUtils.h"
#include "Modules/ModuleManager.h"
#include "ROXTaskUtils.h"
#include "ROXLib.h"

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
	SceneCapture_ConfigComponents();
}

void AROXCamera::BeginDestroy()
{
	Super::BeginDestroy();
}


void AROXCamera::InitComponents()
{	
	Stereo_R = CreateDefaultSubobject<USceneComponent>("Stereo_R");
	Stereo_R->SetupAttachment(GetRootComponent());
	Stereo_R->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	Stereo_L = CreateDefaultSubobject<USceneComponent>("Stereo_L");
	Stereo_L->SetupAttachment(GetRootComponent());
	Stereo_L->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));

	SceneCapture_RGB = CreateDefaultSubobject<USceneCaptureComponent2D>("SceneCapture_RGB");
	SceneCapture_RGB->SetupAttachment(Stereo_R);
	SceneCapture_RGB->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	SceneCapture_Depth = CreateDefaultSubobject<USceneCaptureComponent2D>("SceneCapture_Depth");
	SceneCapture_Depth->SetupAttachment(Stereo_R);
	SceneCapture_Depth->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	SceneCapture_Normal = CreateDefaultSubobject<USceneCaptureComponent2D>("SceneCapture_Normal");
	SceneCapture_Normal->SetupAttachment(Stereo_R);
	SceneCapture_Normal->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	SceneCapture_Mask = CreateDefaultSubobject<USceneCaptureComponent2D>("SceneCapture_Mask");
	SceneCapture_Mask->SetupAttachment(Stereo_R);
	SceneCapture_Mask->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));

	SceneCapture_RGB_L = CreateDefaultSubobject<USceneCaptureComponent2D>("SceneCapture_RGB_L");
	SceneCapture_RGB_L->SetupAttachment(Stereo_L);
	SceneCapture_RGB_L->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	SceneCapture_Depth_L = CreateDefaultSubobject<USceneCaptureComponent2D>("SceneCapture_Depth_L");
	SceneCapture_Depth_L->SetupAttachment(Stereo_L);
	SceneCapture_Depth_L->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	SceneCapture_Normal_L = CreateDefaultSubobject<USceneCaptureComponent2D>("SceneCapture_Normal_L");
	SceneCapture_Normal_L->SetupAttachment(Stereo_L);
	SceneCapture_Normal_L->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	SceneCapture_Mask_L = CreateDefaultSubobject<USceneCaptureComponent2D>("SceneCapture_Mask_L");
	SceneCapture_Mask_L->SetupAttachment(Stereo_L);
	SceneCapture_Mask_L->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
}


void AROXCamera::SceneCapture_ConfigComponents()
{
	if (GetGroundTruth)
	{
		SceneCapture_ConfigComp(SceneCapture_RGB, EROXViewMode::RVM_RGB, TEXT("SceneCapture_RGB"));
		SceneCapture_ConfigComp(SceneCapture_Depth, EROXViewMode::RVM_Depth, TEXT("SceneCapture_Depth"));
		SceneCapture_ConfigComp(SceneCapture_Normal, EROXViewMode::RVM_Normal, TEXT("SceneCapture_Normal"));
		SceneCapture_ConfigComp(SceneCapture_Mask, EROXViewMode::RVM_Mask, TEXT("SceneCapture_Mask"));

		if (isStereoCamera)
		{
			SceneCapture_ConfigComp(SceneCapture_RGB_L, EROXViewMode::RVM_RGB, TEXT("SceneCapture_RGB_L"));
			SceneCapture_ConfigComp(SceneCapture_Depth_L, EROXViewMode::RVM_Depth, TEXT("SceneCapture_Depth_L"));
			SceneCapture_ConfigComp(SceneCapture_Normal_L, EROXViewMode::RVM_Normal, TEXT("SceneCapture_Normal_L"));
			SceneCapture_ConfigComp(SceneCapture_Mask_L, EROXViewMode::RVM_Mask, TEXT("SceneCapture_Mask_L"));
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
		SceneCaptureComp->CaptureSource = ESceneCaptureSource::SCS_BaseColor;
		SceneCaptureComp->TextureTarget->RenderTargetFormat = ETextureRenderTargetFormat::RTF_RGBA8;
		SceneCaptureComp->TextureTarget->TargetGamma = 1;
		break;
	case EROXViewMode::RVM_Normal:
		SceneCaptureComp->CaptureSource = ESceneCaptureSource::SCS_FinalColorLDR;
		SceneCaptureComp->TextureTarget->RenderTargetFormat = ETextureRenderTargetFormat::RTF_RGBA8;
		check(NormalMat);
		SceneCaptureComp->PostProcessSettings.WeightedBlendables.Array.Empty();
		SceneCaptureComp->PostProcessSettings.AddBlendable(NormalMat, 1);
		SceneCaptureComp->PostProcessBlendWeight = 1;
		break;
	}
}

void AROXCamera::SceneCapture_DisableComponents()
{
	SceneCapture_RGB->Deactivate();
	SceneCapture_Depth->Deactivate();
	SceneCapture_Normal->Deactivate();
	SceneCapture_Mask->Deactivate();
	SceneCapture_DisableStereoComponents();
}

void AROXCamera::SceneCapture_DisableStereoComponents()
{
	SceneCapture_RGB_L->Deactivate();
	SceneCapture_Depth_L->Deactivate();
	SceneCapture_Normal_L->Deactivate();
	SceneCapture_Mask_L->Deactivate();
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

	// RGB, Normal, Mask
	else
	{
		TArray<FColor> ImageData;
		ImageData.AddUninitialized(Width * Height);

		TArray<FLinearColor> ImageDataLC;
		ImageDataLC.AddUninitialized(Width * Height);
		RenderTargetResource->ReadLinearColorPixels(ImageDataLC);

		if (ImageData.Num() != 0 && ImageData.Num() == Width * Height)
		{
			// sRGB Color space. True for Mask images, False otherwise
			bool sRGB = (viewmode == EROXViewMode::RVM_Mask);

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


void AROXCamera::SaveRTImageStereo(USceneCaptureComponent2D* SceneCaptureComp_R, USceneCaptureComponent2D* SceneCaptureComp_L, const EROXViewMode viewmode, FString Filename)
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

void AROXCamera::SaveRGBImage(FString Filename)
{
	SaveRTImageStereo(SceneCapture_RGB, SceneCapture_RGB_L, EROXViewMode::RVM_RGB, Filename);
}

void AROXCamera::SaveDepthImage(FString Filename)
{
	SaveRTImageStereo(SceneCapture_Depth, SceneCapture_Depth_L, EROXViewMode::RVM_Depth, Filename);
}

void AROXCamera::SaveNormalImage(FString Filename)
{
	SaveRTImageStereo(SceneCapture_Normal, SceneCapture_Normal_L, EROXViewMode::RVM_Normal, Filename);
}

void AROXCamera::SaveMaskImage(FString Filename)
{
	SaveRTImageStereo(SceneCapture_Mask, SceneCapture_Mask_L, EROXViewMode::RVM_Mask, Filename);
}