// Fill out your copyright notice in the Description page of Project Settings.

#include "ROXSceneManager.h"
#include "ROXCamera.h"
#include "Components/MeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "UObject/UObjectIterator.h"
#include "UObject/ConstructorHelpers.h"


// Sets default values
AROXSceneManager::AROXSceneManager() :
	screenshots_save_directory(),
	screenshots_folder("GeneratedSequences"),
	Persistence_Level_Filter_Str("UEDPIE_0"),
	isMaskedMaterial(false)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Init screenshots save directory to the project folder
	screenshots_save_directory = FPaths::ProjectUserDir();

	MaskPlainMat = nullptr;
	static ConstructorHelpers::FObjectFinder<UMaterial> matMask(TEXT("/Game/ROX/ViewModeMats/Mat_InstanceMaskColor.Mat_InstanceMaskColor"));
	if (matMask.Succeeded())
	{
		MaskPlainMat = (UMaterial*)matMask.Object;
	}
}

// Called when the game starts or when spawned
void AROXSceneManager::BeginPlay()
{
	Super::BeginPlay();

	// Check that the last character of directories is a slash
	if (screenshots_save_directory[screenshots_save_directory.Len() - 1] != '/')
	{
		screenshots_save_directory += "/";
	}

	//if (generate_masks_changing_materials)
	//{
		PrepareMaterials();
	//}
}

// Called every frame
void AROXSceneManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

/*
*/
FColor AROXSceneManager::AssignColor(int idx_in)
{
	/* RGB values are assigned like this:
	00 ->	255	0	0	| 10 -> 127	255	255	| 21 ->	255	127	127
	01 ->	0	255	0	| 11 -> 0	127	0	| 21 ->	127	0	127
	02 ->	0	0	255	| 12 -> 0	127	255	| 22 ->	127	255	127
	03 ->	0	255	255	| 13 -> 255	127	0	| 23 ->	127	127	0
	04 ->	255	0	255	| 14 ->	255	127	255	| 24 ->	127	127	255
	05 ->	255	255	0	| 15 ->	0	0	127	| 25 ->	127	127	127
	06 ->	255	255	255	| 16 ->	0	255	127	| 26 ->	63	0	0
	07 ->	127	0	0	| 17 ->	255	0	127	| 27 ->	63	0	255
	08 ->	127	0	255	| 18 ->	255	255	127	| 28 ->	63	0	127
	09 ->	127	255	0	| 19 ->	0	127	127	| 29 ->	63	255	0
	*/
	const static uint8 ChannelValues[] = { 0, 255, 127, 63, 191, 31, 95, 159, 223, 15, 47, 79, 111, 143, 175, 207, 239, 7, 23, 39, 55, 71, 87, 103, 119, 135, 151, 167, 183, 199, 215, 231 };

	FColor color = FColor(0, 0, 0, 255);
	//int idx = idx_in + 1; // Avoid black to be assigned
	int idx = idx_in; // Avoid black starting from 1

	if (idx > 0 && idx < 32760) // Max colors that can be assigned combining the previous channel values
	{
		// VAL variable represents two things (related between them):
		// - The position index on ChannelValues array of the last channel value.
		// - The number of previous channel values
		int val = FMath::FloorToInt(FMath::Pow(idx, (float)(1.0f / 3.0f)));
		int prev_combinations = FMath::Pow(val, 3);

		int combination_idx = idx - prev_combinations;
		int sqr_val = val * val;
		int n_comb_double = 3 * sqr_val;
		int n_comb_unit = 3 * val;

		uint8 color_arr[3] = { 0, 0, 0 };
		if (combination_idx >= 0 && combination_idx < n_comb_double)
		{
			int partial_group_idx = combination_idx / sqr_val;
			int partial_line_idx = combination_idx % sqr_val;

			int inner_group_idx = partial_line_idx / val;
			int inner_line_idx = partial_line_idx % val;
			int low = 0, high = 2;
			if (partial_group_idx == 0) low = 1;
			else if (partial_group_idx == 2) high = 1;

			color_arr[partial_group_idx] = ChannelValues[val];
			color_arr[low] = ChannelValues[inner_group_idx];
			color_arr[high] = ChannelValues[inner_line_idx];
		}
		else if (combination_idx >= n_comb_double && combination_idx < (n_comb_unit + n_comb_double))
		{
			for (int i = 0; i < 3; ++i) color_arr[i] = ChannelValues[val];
			int partial_comb_idx = combination_idx - n_comb_double;
			int partial_group_idx = partial_comb_idx / val;
			int partial_line_idx = partial_comb_idx % val;

			color_arr[partial_group_idx] = ChannelValues[partial_line_idx];
		}
		else if (combination_idx == (n_comb_unit + n_comb_double))
		{
			for (int i = 0; i < 3; ++i) color_arr[i] = ChannelValues[val];
		}

		color.R = color_arr[0];
		color.G = color_arr[1];
		color.B = color_arr[2];
		color.A = 255;
	}
	return color;
}

int AROXSceneManager::GetIdxFromColor(FColor color)
{
	int idx = 0;
	const static uint8 ChannelValues[] = { 0, 255, 127, 63, 191, 31, 95, 159, 223, 15, 47, 79, 111, 143, 175, 207, 239, 7, 23, 39, 55, 71, 87, 103, 119, 135, 151, 167, 183, 199, 215, 231 };
	const static uint8 ChannelValuesSize = 32;
	uint8 color_input[3] = { color.R, color.G, color.B };
	uint8 color_arr[3] = { 0, 0, 0 };

	// Translation from colors to positions in ChannelValues array.
	// Max of the three position values is stored.
	// If max value is repeated, it is taken into account.
	bool found = true;
	int max = -1;
	int max_pos = -1, max_pos2 = -1, not_max_pos = -1, not_max_pos2 = -1;
	bool isMaxRepeatedTwice = false, isAllEqual = false;
	for (int i = 0; i < 3 && found; ++i)
	{
		found = false;
		for (int j = 0; j < ChannelValuesSize && !found; ++j)
		{
			if (color_input[i] == ChannelValues[j])
			{
				found = true;
				color_arr[i] = j;
				if (max == j)
				{
					if (!isMaxRepeatedTwice)
					{
						isMaxRepeatedTwice = true;
						max_pos2 = i;

						if (i == 1)	not_max_pos = 2;
						else if (i == 2)
						{
							if (max_pos == 0) not_max_pos = 1;
							else if (max_pos == 1) not_max_pos = 0;
						}
					}
					else
					{
						isMaxRepeatedTwice = false;
						isAllEqual = true;
					}
				}
				else if (max < j)
				{
					max = j;
					isMaxRepeatedTwice = false;
					max_pos = i;
					max_pos2 = -1;
				}
			}
		}
	}

	if (found)
	{
		int prev_combinations = FMath::Pow(max, 3);

		// Case 1 -> MAX - MAX - MAX (Ex: 127 127 127)
		if (isAllEqual)
		{
			idx = FMath::Pow(max + 1, 3) - 1;
		}
		// Case 2 -> MAX - MAX - X (Ex: 127 127 255)
		else if (isMaxRepeatedTwice)
		{
			int case3_combinations = FMath::Pow(max, 2) * 3;
			idx = prev_combinations + case3_combinations + max * not_max_pos + color_arr[not_max_pos];
		}
		// Case 3 -> MAX - X - X (Ex: 127 0 255)
		else
		{
			if (max_pos == 0) { not_max_pos = 1; not_max_pos2 = 2; }
			else if (max_pos == 1) { not_max_pos = 0; not_max_pos2 = 2; }
			else { not_max_pos = 0; not_max_pos2 = 1; }

			int case3_prev_combinations = FMath::Pow(max, 2) * max_pos;
			idx = prev_combinations + case3_prev_combinations + color_arr[not_max_pos] * max + color_arr[not_max_pos2];
		}
	}

	//return idx - 1;
	return idx;
}

void AROXSceneManager::PrepareMaterials()
{
	bool file_loaded = false;
	TMap<FName, FROXSceneObject> SceneObjects;
	//if (json_file_names.Num() > 0)
	//{
		//FString sceneObject_json_filename = screenshots_save_directory + screenshots_folder + "/" + json_file_names[CurrentJsonFile] + "/sceneObject.json";
		FString sceneObject_json_filename = screenshots_save_directory + screenshots_folder + "/sceneObject.json";
		SceneObjects = ROXJsonParser::LoadSceneObjects(sceneObject_json_filename);
		file_loaded = SceneObjects.Num() > 0;
	//}

	TArray<int> used_idxs;
	if (file_loaded)
	{
		TArray<FName> SceneObjects_name;
		SceneObjects.GetKeys(SceneObjects_name);
		for (FName SceneObject_name : SceneObjects_name)
		{
			FROXSceneObject* SceneObject_data = SceneObjects.Find(SceneObject_name);
			int color_idx = GetIdxFromColor(SceneObject_data->instance_color);
			if (color_idx > 0)
			{
				used_idxs.Add(color_idx);
			}
		}
		used_idxs.Sort();
	}

	int comp_idx = 1;
	int used_idxs_i = 0;
	for (TObjectIterator<AActor> Itr; Itr; ++Itr)
	{
		TArray<UMeshComponent*> Components;
		(*Itr)->GetComponents<UMeshComponent>(Components);
		FString ActorFullName = (*Itr)->GetFullName();

		if (Components.Num() > 0 && ActorFullName.Contains(Persistence_Level_Filter_Str) && !(*Itr)->IsA(ACameraActor::StaticClass()) && !(*Itr)->IsA(AROXCamera::StaticClass()))
		{
			TArray<FROXMeshComponentMaterials> ComponentMaterials;
			FColor ActorColor = FColor::Black;
			int ActorColorNum = 0;
			if (file_loaded)
			{
				FROXSceneObject* SceneObject = SceneObjects.Find(FName(*(*Itr)->GetName()));
				if (SceneObject != nullptr)
				{
					ActorColor = SceneObject->instance_color;
					ActorColorNum = GetIdxFromColor(ActorColor);
				}
				else
				{
					while (comp_idx == used_idxs[used_idxs_i])
					{
						comp_idx++;
						if (used_idxs_i < (used_idxs.Num() - 1)) used_idxs_i++;
					}
					ActorColor = AssignColor(comp_idx);
					ActorColorNum = comp_idx;
					++comp_idx;
				}
			}
			else
			{
				ActorColor = AssignColor(comp_idx);
				ActorColorNum = comp_idx;
				++comp_idx;
			}

			FString status_msg = ActorFullName + " " + FString::FromInt(ActorColorNum);
			UE_LOG(LogUnrealROX, Warning, TEXT("%s"), *status_msg);

			for (UMeshComponent* MeshComponent : Components)
			{
				MeshComponent->bRenderCustomDepth = true;
				MeshComponent->SetCustomDepthStencilValue(ActorColorNum);

				FROXMeshComponentMaterials MaterialStruct;
				MaterialStruct.MeshComponent = MeshComponent;
				MaterialStruct.MaskMaterialColor = ActorColor;
				//MaterialStruct.MaskMaterialLinearColor = FLinearColor::FromPow22Color(MaterialStruct.MaskMaterialColor);
				MaterialStruct.MaskMaterialLinearColor = FLinearColor::FromSRGBColor(MaterialStruct.MaskMaterialColor);
				MaterialStruct.MaskMaterial = UMaterialInstanceDynamic::Create(MaskPlainMat, this);
				MaterialStruct.MaskMaterial->SetVectorParameterValue("MatColor", MaterialStruct.MaskMaterialLinearColor);

				//UE_LOG(LogUnrealROX, Warning, TEXT("|-%s"), *(MeshComponent->GetName()));
				//FString msg = (*Itr)->GetName() + " " + FString::FromInt(comp_idx) + " " + FString::FromInt(GetIdxFromColor(MaterialStruct.MaskMaterialColor));
				//msg += ": R: " + FString::FromInt(MaterialStruct.MaskMaterialColor.R) + " G: " + FString::FromInt(MaterialStruct.MaskMaterialColor.G) + " B: " + FString::FromInt(MaterialStruct.MaskMaterialColor.B);
				//UE_LOG(LogUnrealROX, Warning, TEXT("%s"), *msg);

				TArray <UMaterialInterface*> Materials = MeshComponent->GetMaterials();
				for (UMaterialInterface* Material : Materials)
				{
					if (Material != NULL)
					{
						MaterialStruct.DefaultMaterials.Add(Material);
						//UE_LOG(LogUnrealROX, Warning, TEXT("__|-%s"), *(Material->GetName()));
					}
				}

				ComponentMaterials.Add(MaterialStruct);
			}

			MeshMaterials.Add(*Itr, ComponentMaterials);
		}
	}

	//if (json_file_names.Num() < 1)
	//{
		//FString sceneObject_json_filename = screenshots_save_directory + screenshots_folder + "/sceneObject.json";
		ROXJsonParser::WriteSceneObjects(MeshMaterials, sceneObject_json_filename);
	//}
}

void AROXSceneManager::ToggleActorMaterials()
{
	if (generate_masks_changing_materials)
	{
		isMaskedMaterial = !isMaskedMaterial;
		for (const TPair<AActor*, TArray<FROXMeshComponentMaterials>>& pair : MeshMaterials)
		{
			AActor* actor = pair.Key;
			const TArray<FROXMeshComponentMaterials> components = pair.Value;

			for (FROXMeshComponentMaterials component : components)
			{
				for (int i = 0; i < component.DefaultMaterials.Num(); ++i)
				{
					if (isMaskedMaterial)
					{
						component.MeshComponent->SetMaterial(i, component.MaskMaterial);
					}
					else
					{
						component.MeshComponent->SetMaterial(i, component.DefaultMaterials[i]);
					}
				}
			}
		}
	}
}

bool AROXSceneManager::SetMaskedMaterials(bool bPlainColorMat)
{
	bool isMaterialChanged = false;
	if (isMaskedMaterial != bPlainColorMat)
	{
		ToggleActorMaterials();
		isMaterialChanged = true;
	}
	return isMaterialChanged;
}


void AROXSceneManager::CacheStaticMeshActors()
{
	// StaticMeshActor dump
	CachedSM.Empty();
	for (TObjectIterator<AStaticMeshActor> Itr; Itr; ++Itr)
	{
		FString fullName = Itr->GetFullName();
		/*if ((fullName.Contains(Persistence_Level_Filter_Str) || bStandaloneMode || bDebugMode) && Itr->GetStaticMeshComponent()->Mobility == EComponentMobility::Movable)
		{
			CachedSM.Add(*Itr);
		}*/
	}
}


void AROXSceneManager::CacheSceneActors(const TArray<FROXPawnInfo> &PawnsInfo, const TArray<FROXCameraConfig> &CameraConfigs)
{
	// StaticMeshActor dump
	/*CacheStaticMeshActors();

	// Cameras dump
	CameraActors.Empty();
	StereoCameraBaselines.Empty();
	SceneCapture_Depth.Empty();
	SceneCapture_Lit.Empty();
	SceneCapture_Normal.Empty();
	SceneCapture_Mask.Empty();

	DefaultSceneCapture = SpawnSceneCapture("DefaultSceneCapture", 90);
	SetViewmodeSceneCapture(DefaultSceneCapture, EROXViewMode::RVM_Lit);*/

	/*for (TActorIterator <ACameraActor> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		ActorItr->DetachFromActor(FDetachmentTransformRules(EDetachmentRule::KeepWorld, true));
		ActorItr->Destroy();
	}*/
	/*bool check_camera = (json_file_names.Num() == cameras_to_rebuild.Num()) && (cameras_to_rebuild[CurrentJsonFile].Len() == CameraConfigs.Num());
	for (int i = 0; i < CameraConfigs.Num(); ++i)
	{
		if (!check_camera || cameras_to_rebuild[CurrentJsonFile][i] == '1')
		{
			SpawnCamerasPlayback(CameraConfigs[i], 0);

			// Stereo
			if (CameraConfigs[i].StereoBaseline > 0.0)
			{
				SpawnCamerasPlayback(CameraConfigs[i], -1);
				SpawnCamerasPlayback(CameraConfigs[i], 1);
			}
		}
	}*/
}