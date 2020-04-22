#include "ROXServer.h"

#include "Animation/Skeleton.h"
#include "AssetRegistryModule.h"
#include "SocketSubsystem.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "Camera/CameraActor.h"
#include "Engine/StaticMesh.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "UObject/ObjectMacros.h"
#include "Interfaces/IPv4/IPv4Endpoint.h"
#include "Common/TcpSocketBuilder.h"



AROXServer::AROXServer()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Create array that connect commands with their function and the number of parameters
	NameToFunct map [] =
	{
		{"object_list",						&AROXServer::ObjectList,				0},
		{"asset_list",						&AROXServer::AssetsList,				0},
		{"skeletal_list",					&AROXServer::SkeletalList,				0},
		{"actor_list",						&AROXServer::ActorList,				0},
		{"camera_list",						&AROXServer::CameraList,				0},
		{"socket_list",						&AROXServer::SocketList,				1},	// nameskeletal
		{"move",							&AROXServer::Move,						4}, // name, x,y,z
		{"move_socket",						&AROXServer::MoveSocket,				5}, // nameskeletal, namesocket, x,y,z
		{"rotate",							&AROXServer::Rotate,					4}, // name, pitch, yaw, roll
		{"rotate_socket",					&AROXServer::RotateSocket,				5}, // nameskeletal, namesocket, pitch, yaw, roll
		{"scale",							&AROXServer::Scale,					4}, // name, x,y,z
		{"spawn_actor",						&AROXServer::SpawnActor,				5}, // actor_name, asset_name, x,y,z
		{"spawn_camera",					&AROXServer::SpawnCamera,				4}, // camera_name, x,y,z
		{"camera_look_at",					&AROXServer::CameraLookAt,				2}, // camera_name actor_name -- camera_name x y z
		{"set_asset_path",					&AROXServer::ChangeAssetsPath,			1},	// asset_path
		{"change_texture",					&AROXServer::ChangeTexture,			2}, // nameactor, nametexture
		{"get_location",					&AROXServer::GetLocation,				1}, // nameactor
		{"get_rotation",					&AROXServer::GetRotation,				1}, // nameactor
		{"get_scale",						&AROXServer::GetScale,					1}, // nameactor
		{"get_socket_location",				&AROXServer::GetSocketLocation,		2}, // nameskeletal socket
		{"get_socket_rotation",				&AROXServer::GetSocketRotation,		2}, // nameskeletal socket
		{"get_3d_bounding_box",				&AROXServer::GetBoundingBox,			1}, // name
		{"toggle_scene_physics",			&AROXServer::ToggleScenePhysics,		1},
		{"is_scene_physics_enabled",		&AROXServer::ScenePhysicsEnabled,		0},
		{"set_camera_stereo",				&AROXServer::SetCameraStereo,			3},
		{"is_camera_stero",					&AROXServer::IsCameraStereo,			1},
		{"set_output_frames_resolution",	&AROXServer::SetOutFrameResolution,	2},
		{"set_output_frames_path",			&AROXServer::SetOutFramePath,			1},
		{"get_rgb",							&AROXServer::GetRGB,					1},
		{"get_depth",						&AROXServer::GetDepth,					1},
		{"get_normal",						&AROXServer::GetNormal,				1},
		{"get_instance_mask",				&AROXServer::GetInstanceMask,			1},
	};
	Mapping.Append(map,ARRAY_COUNT(map));

}

AROXServer::~AROXServer()
{
	if (ConnectionSocket)
	{
		ConnectionSocket->Close();
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(ConnectionSocket);
	}

	if (ListenerSocket)
	{
		ListenerSocket->Close();
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(ListenerSocket);
	}
}

// Called when the game starts or when spawned
void AROXServer::BeginPlay()
{
	Super::BeginPlay();

	ListenerSocket = CreateListener();

	//Initialize List of Objects, Skeletal and Asset
	for (TObjectIterator<AActor> itr; itr; ++itr)
	{
		TArray<UStaticMeshComponent*> components_sm;
		(*itr)->GetComponents<UStaticMeshComponent>(components_sm);
		if (components_sm.Num() > 0 && (*itr)->IsRootComponentMovable())
		{
			LObjMov.Emplace(*(*itr)->GetName(), (*itr));
		}
		
		TArray<USkeletalMeshComponent*> components_skm;
		(*itr)->GetComponents<USkeletalMeshComponent>(components_skm);
		if (components_skm.Num() > 0)
		{
			LSkeletal.Emplace(*(*itr)->GetName(), (*itr));
		}
		
		TArray<UCameraComponent*> components_cam;
		(*itr)->GetComponents<UCameraComponent>(components_cam);
		if (components_cam.Num() > 0)
		{
			LCamera.Emplace(*(*itr)->GetName(), (*itr));
		}

		LActor.Emplace(*(*itr)->GetName(), (*itr));
	}
	CreateAssetsList();
	
}

// Called every frame
void AROXServer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateConnectionListener();

	if (ModifyingActors.Num()!=0)
		UpdateTransform(DeltaTime);
}

//
void AROXServer::UpdateTransform(float DeltaTime)
{
	for (int i = 0; i< ModifyingActors.Num();i++)
	{
		TimeModifyingActor& m_a = ModifyingActors[i];
		m_a.CurrentTime = m_a.CurrentTime + DeltaTime;
		float percent = m_a.CurrentTime / m_a.TotalTime;
		if (m_a.Rotating) 
		{
			if (m_a.CurrentTime < m_a.TotalTime)
			{
				FVector r = m_a.Initial + m_a.Difference * percent;
				if (m_a.Socket != "")
					Cast<AROXTestBasePawn>(m_a.Act)->EmplaceRotation(*m_a.Socket,FRotator(r.X, r.Y, r.Z));
				else
					m_a.Act->SetActorRotation(FRotator(r.X, r.Y, r.Z));
			}
			else
			{
				FVector r = m_a.Initial + m_a.Difference;
				if(m_a.Socket != "")
					Cast<AROXTestBasePawn>(m_a.Act)->EmplaceRotation(*m_a.Socket,FRotator(r.X, r.Y, r.Z));
				else
					m_a.Act->SetActorRotation(FRotator(r.X, r.Y, r.Z));
				
				//Remove item
				ModifyingActors.RemoveAt(i);
				i--;
			}
		}
		else
		{
			if (m_a.CurrentTime < m_a.TotalTime)
			{
				if (m_a.Socket != "")
					Cast<AROXTestBasePawn>(m_a.Act)->EmplaceTranslation(*m_a.Socket, m_a.Initial + m_a.Difference * percent);
				else
					m_a.Act->SetActorLocation(m_a.Initial + m_a.Difference * percent);
			}
			else
			{
				if (m_a.Socket != "")
					Cast<AROXTestBasePawn>(m_a.Act)->EmplaceTranslation(*m_a.Socket, m_a.Initial + m_a.Difference);
				else
					m_a.Act->SetActorLocation(m_a.Initial + m_a.Difference);
				
				//Remove Item
				ModifyingActors.RemoveAt(i);
				i--;
			}
		}
	}
}

/////////////////////////////
//   CONECTION FUNCTIONS ///
///////////////////////////
FSocket* AROXServer::CreateListener()
{
	FString name = "ServerSocket";

	// IP from string to number
	TArray<FString> ip_array_s;
	TArray<uint8> ip_array_n;
	Ip.ParseIntoArray(ip_array_s, TEXT("."), true);

	// Check if there are 4 numbers
	if (ip_array_s.Num() != 4)
		return nullptr;

	for (int32 i = 0; i < 4; ++i)
	{
		ip_array_n.Add(FCString::Atoi(*ip_array_s[i]));
	}

	// Create socket
	FIPv4Endpoint end_point(FIPv4Address(ip_array_n[0], ip_array_n[1], ip_array_n[2], ip_array_n[3]), Port);
	FSocket* new_socket = FTcpSocketBuilder(*name).AsReusable().BoundToEndpoint(end_point).Listening(1);

	return new_socket;
}

void AROXServer::UpdateConnectionListener()
{
	bool pending;
	uint32 connection_size;
	ListenerSocket->HasPendingConnection(pending);

	if (ConnectionSocket && ConnectionSocket->HasPendingData(connection_size))
	{
		// Listen to the message of the connection
		ListenConnection();
	}
	else if (pending)
	{
		//Receive connection
		TSharedRef <FInternetAddr> remote_address = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
		ConnectionSocket = ListenerSocket->Accept(*remote_address, TEXT(""));
	}
}

void AROXServer::ListenConnection()
{
	// Array with read data
	TArray<uint8> message_data;
	// Size of data not readed yet
	uint32 size_left;
	int32 read_data = 0;

	// Read loop
	while (ConnectionSocket->HasPendingData(size_left))
	{
		message_data.Init(0, size_left);
		ConnectionSocket->Recv(message_data.GetData(), message_data.Num(), read_data);
	}

	// Message data to string
	message_data.Add(0);
	FString data_string = FString(UTF8_TO_TCHAR(reinterpret_cast<const char*>(message_data.GetData())));

	//Split message
	TArray<FString> message_array;
	data_string.ParseIntoArray(message_array, TEXT(" "), true);

	// Check comands and go to the proper function
	bool found = false;
	for (int i = 0; i < Mapping.Num(); i++)
	{
		if (message_array[0] == Mapping[i].Name && (message_array.Num() - 1) >= Mapping[i].Parameters)
		{
			(this->*Mapping[i].PFunc)(message_array);
			found = true;
			break;
		}
	}

	if (!found)
	{
		SendReponse("command wasn't found");
	}

}

void AROXServer::SendReponse(FString reponse)
{
	TCHAR *reponse_char = reponse.GetCharArray().GetData();
	int32 reponse_size = FCString::Strlen(reponse_char);
	int32 sent = 0;
	if(ConnectionSocket)
		ConnectionSocket->Send((uint8*)TCHAR_TO_UTF8(reponse_char), reponse_size, sent);
}

//////////////////////////////
/////////////////////////////
//   COMAND FUNCTIONS //////
///////////////////////////
//////////////////////////


void AROXServer::FindActorInList(FName n)
{
	SelObj = nullptr;
	if (LObjMov.Contains(n)) // check if the object is in the map
	{
		SelObj = LObjMov[n]; // select object
	}
	else if (LSkeletal.Contains(n))
	{
		SelObj = LSkeletal[n];
	}
	else if (LCamera.Contains(n))
		SelObj = LCamera[n];
}

void AROXServer::ObjectList(TArray<FString>& message)
{
	FString reponse;
	LObjMov.Empty();
	for (TObjectIterator<AActor> itr; itr; ++itr)
	{
		//Check it the actor has a static mesh component
		TArray<UStaticMeshComponent*> components;
		(*itr)->GetComponents<UStaticMeshComponent>(components);

		if (components.Num() > 0 && (*itr)->IsRootComponentMovable())
		{
			LObjMov.Emplace(*(*itr)->GetName(), (*itr));
			reponse += FString::Printf(TEXT("%s\n"), *(*itr)->GetName());
		}
	}

	SendReponse(reponse);
}

void AROXServer::ActorList(TArray<FString>& message)
{
	FString reponse;
	LActor.Empty();
	for (TObjectIterator<AActor> itr; itr; ++itr)
	{
		LActor.Emplace(*(*itr)->GetName(), (*itr));
		reponse += FString::Printf(TEXT("%s\n"), *(*itr)->GetName());
	}
	SendReponse(reponse);
}

void AROXServer::Move(TArray<FString>& message)
{ 
	FindActorInList(FName(*message[1]));
	if (SelObj)
	{
		//check if there is time to reach the location
		if (message.Num() >= 6)
		{
			float time = FCString::Atof(*message[5]);
			FVector Initial = SelObj->GetActorLocation();
			FVector Difference = { FCString::Atof(*message[2]) - Initial.X, FCString::Atof(*message[3]) - Initial.Y, FCString::Atof(*message[4]) - Initial.Z};

			// Add it to modifying actors to change the transform with deltatime
			ModifyingActors.Emplace(SelObj, FString(""), time, Difference, Initial,false);
		}
		else
			SelObj->SetActorLocation({ FCString::Atof(*message[2]), FCString::Atof(*message[3]), FCString::Atof(*message[4]) });
	}
	else
		SendReponse("The object wasn't found");

}

void AROXServer::GetLocation(TArray<FString>& message)
{
	if (LActor.Contains(*message[1]))
	{
		FVector loc = LActor[*message[1]]->GetActorLocation();
		SendReponse(FString::Printf(TEXT("%f %f %f"), loc.X, loc.Y, loc.Z));
	}
	else
		SendReponse("Can't find object");
}

void AROXServer::Rotate(TArray<FString>& message)
{
	FindActorInList(FName(*message[1]));
	if (SelObj)
	{
		if (message.Num() >= 6)
		{
			float time = FCString::Atof(*message[5]);
			FRotator ini = SelObj->GetActorRotation();
			FVector Initial = FVector(ini.Pitch, ini.Yaw, ini.Roll);
			FVector Difference = {FCString::Atof(*message[2])-Initial.X, FCString::Atof(*message[3])-Initial.Y, FCString::Atof(*message[4])-Initial.Z};
			
			ModifyingActors.Emplace(SelObj, "", time, Difference, Initial, true);
		}
		else
			SelObj->SetActorRotation(FRotator(FCString::Atof(*message[2]), FCString::Atof(*message[3]), FCString::Atof(*message[4])));
	}
	else
		SendReponse("The object wasn't found");
}

void AROXServer::GetRotation(TArray<FString>& message)
{
	if (LActor.Contains(*message[1]))
	{
		FRotator rot = LActor[*message[1]]->GetActorRotation();
		SendReponse(FString::Printf(TEXT("%f %f %f"), rot.Pitch, rot.Yaw, rot.Roll));
	}
	else
		SendReponse("Can't find object");
}

void AROXServer::Scale(TArray<FString>& message)
{
	FindActorInList(FName(*message[1]));
	if(SelObj)
		SelObj->SetActorScale3D(FVector(FCString::Atof(*message[2]), FCString::Atof(*message[3]), FCString::Atof(*message[4])));
	else
		SendReponse("The object wasn't found");
}

void AROXServer::GetScale(TArray<FString>& message)
{
	if (LActor.Contains(*message[1]))
	{
		FVector sc = LActor[*message[1]]->GetActorScale();
		SendReponse(FString::Printf(TEXT("%f %f %f"), sc.X, sc.Y, sc.Z));
	}
	else
		SendReponse("Can't find object");
}


/////////////////////////////
//   ASSETS FUNCTIONS ///
///////////////////////////


void AROXServer::CreateAssetsList()
{
	/*FAssetRegistryModule& asset_rm = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(FName("AssetRegistry"));
	IAssetRegistry& asset_r = asset_rm.Get();

	// Scan specific path
	TArray<FString> path_scan;
	path_scan.Add(AssetsPath);
	asset_r.ScanPathsSynchronous(path_scan);

	// Get all assets in the path, does not load them
	TArray<FAssetData> assets_list;
	asset_r.GetAssetsByPath(FName(*AssetsPath), assets_list);

	// Put the Assets in the map
	for (FAssetData asset : assets_list)
	{
		LAssets.Emplace(asset.AssetName.ToString(), asset);
	}*/
}

void AROXServer::ChangeAssetsPath(TArray<FString>& message)
{
	AssetsPath = message[1];
}

void AROXServer::AssetsList(TArray<FString>& message)
{
	LAssets.Empty();
	CreateAssetsList();
	FString reponse;

	for (auto& elem : LAssets)
	{
		reponse += FString::Printf(TEXT("%s "), *elem.Key);
	}

	SendReponse(reponse);
}

void AROXServer::ChangeTexture(TArray<FString>& message)
{
	FindActorInList(*message[1]);

	//Check if the name is in the assets map
	if (SelObj != nullptr && LAssets.Contains(message[2]))
	{
		// Load texture
		FSoftObjectPath texture_path = AssetsPath + "/" + LAssets[message[2]].AssetName.ToString();

		UTexture* texture = dynamic_cast<UTexture*>(texture_path.TryLoad());
		if (texture) // check if the asset is a texture
		{
			// Get mesh of actual actor
			UMeshComponent* mesh_component = dynamic_cast<UMeshComponent*>(SelObj->GetComponentByClass(UMeshComponent::StaticClass()));

			// Create Dynamic material to change parameters
			UMaterialInstanceDynamic* dynamicMaterial;

			if (mesh_component->GetMaterial(0)->GetName() == "DynamicMaterial")
			{
				dynamicMaterial = dynamic_cast<UMaterialInstanceDynamic*>(mesh_component->GetMaterial(0));
			}
			else
			{
				dynamicMaterial = mesh_component->CreateDynamicMaterialInstance(0, mesh_component->GetMaterial(0));
				mesh_component->SetMaterial(0, dynamicMaterial);
			}
			//Set Texture Parameter of the material
			dynamicMaterial->SetTextureParameterValue("Dynamic", texture);

		}
		else
			SendReponse("Asset isn't a texture");
	}
	else
		SendReponse("Can't find actor or asset");
}

void AROXServer::SpawnActor(TArray<FString>& message)
{
	if (LActor.Contains(*message[1]))
	{
		SendReponse("This name is already use");
	}
	else
	{
		UWorld* world = GetWorld();

		if (LAssets.Contains(message[2]))
		{
			FSoftObjectPath actor_path = AssetsPath + "/" + LAssets[message[2]].AssetName.ToString();
			UObject* asset = Cast<UObject>(StaticLoadObject(UObject::StaticClass(), NULL, *actor_path.ToString()));
			UBlueprint* GeneratedBP = Cast<UBlueprint>(asset);
			if (GeneratedBP || asset)
			{
				AActor* new_actor;

				if (GeneratedBP)
					new_actor = world->SpawnActor<AActor>(GeneratedBP->GeneratedClass, FVector(FCString::Atof(*message[3]), FCString::Atof(*message[4]), FCString::Atof(*message[5])), FRotator(0, 0, 0));
				else
					new_actor = world->SpawnActor<AActor>(asset->GetClass(), FVector(FCString::Atof(*message[3]), FCString::Atof(*message[4]), FCString::Atof(*message[5])), FRotator(0, 0, 0));

				new_actor->SetActorLabel(message[1]);
				new_actor->GetRootComponent()->SetMobility(EComponentMobility::Movable);
				LActor.Emplace(FName(*new_actor->GetName()), new_actor);

				TArray<UStaticMeshComponent*> components_sm;
				new_actor->GetComponents<UStaticMeshComponent>(components_sm);
				if (components_sm.Num() > 0 && new_actor->IsRootComponentMovable())
				{
					LObjMov.Emplace(*new_actor->GetName(), new_actor);
				}

				TArray<USkeletalMeshComponent*> components_skm;
				new_actor->GetComponents<USkeletalMeshComponent>(components_skm);
				if (components_skm.Num() > 0)
				{
					LSkeletal.Emplace(*new_actor->GetName(), new_actor);
				}

				LActor.Emplace(FName(*new_actor->GetName()), new_actor);
			}
			else
				SendReponse("Can't spawn asset");
		}
		else
			SendReponse("Can't find asset");
	}
}

/////////////////////////////
//   SKELETAL FUNCTIONS ////
///////////////////////////

void AROXServer::SkeletalList(TArray<FString>& message)
{
	FString reponse;
	LSkeletal.Empty();
	for (TObjectIterator<AActor> itr; itr; ++itr)
	{
		TArray<USkeletalMeshComponent*> components;
		(*itr)->GetComponents<USkeletalMeshComponent>(components);
		if (components.Num() > 0)
		{
			LSkeletal.Emplace(*(*itr)->GetName(), (*itr));
			reponse += FString::Printf(TEXT("%s "), *(*itr)->GetName());
		}
	}
	if (reponse.Len() != 0)
		SendReponse(reponse);
	else
		SendReponse("There isn't any SkeletalMeshActor");
}

void AROXServer::SocketList(TArray<FString>& message)
{
	if (LSkeletal.Contains(FName(*message[1])))
	{
		TArray<USkeletalMeshComponent*> components;
		LSkeletal[FName(*message[1])]->GetComponents<USkeletalMeshComponent>(components);
		SocketNames.Empty();
		SocketNames = components[0]->GetAllSocketNames();
		FString reponse;
		for (FName elem : SocketNames)
		{
			reponse += FString::Printf(TEXT("%s "), *elem.ToString());
		}
		if (reponse.Len() != 0)
			SendReponse(reponse);
		else
			SendReponse("This isn't a skeletal mesh");
	}
	else
		SendReponse("Skeletal mesh wasn't found");
}

void AROXServer::MoveSocket(TArray<FString>& message)
{
	if (LSkeletal.Contains(FName(*message[1])))
	{
		AROXTestBasePawn* pawn = Cast<AROXTestBasePawn>(LSkeletal[FName(*message[1])]);
		if (pawn)
		{
			SocketNames.Empty();
			TArray<USkeletalMeshComponent*> components;
			pawn->GetComponents<USkeletalMeshComponent>(components);
			SocketNames = components[0]->GetAllSocketNames();

			FName n_socket = FName(*message[2]);
			if (SocketNames.Contains(n_socket))
			{
				if (message.Num() >= 7)
				{
					float time = FCString::Atof(*message[6]);
					SelObj = LSkeletal[FName(*message[1])];
					FString SelSocket = message[2];

					FVector initial = components[0]->GetSocketLocation(n_socket);
					FVector difference = FVector(FCString::Atof(*message[3]) - initial.X, FCString::Atof(*message[4]) - initial.Y, FCString::Atof(*message[5]) - initial.Z);

					ModifyingActors.Emplace(SelObj, SelSocket, time, difference, initial, false);
				}
				else
					pawn->EmplaceTranslation(n_socket, FVector(FCString::Atof(*message[3]), FCString::Atof(*message[4]), FCString::Atof(*message[5])));
			}
			else
				SendReponse("Socket wasn't found");
		}
	}
	else
		SendReponse("Skeletal mesh wasn't found");
}

void AROXServer::GetSocketLocation(TArray<FString>& message)
{
	FindActorInList(FName(*message[1]));
	if (SelObj)
	{
		SocketNames.Empty();
		TArray<USkeletalMeshComponent*> components;
		SelObj->GetComponents<USkeletalMeshComponent>(components);
		SocketNames = components[0]->GetAllSocketNames();

		FName n_socket = FName(*message[2]);
		if (SocketNames.Contains(n_socket))
		{
			FVector loc = components[0]->GetSocketLocation(n_socket);
			SendReponse(FString::Printf(TEXT("%f %f %f"), loc.X, loc.Y, loc.Z));
		}
		else
			SendReponse("Can't find the socket");
	}
}

void AROXServer::RotateSocket(TArray<FString>& message)
{
	if (LSkeletal.Contains(FName(*message[1])))
	{
		AROXTestBasePawn* pawn = Cast<AROXTestBasePawn>(LSkeletal[FName(*message[1])]);
		if (pawn)
		{
			SocketNames.Empty();
			TArray<USkeletalMeshComponent*> components;
			pawn->GetComponents<USkeletalMeshComponent>(components);
			SocketNames = components[0]->GetAllSocketNames();

			FName n_socket = FName(*message[2]);
			if (SocketNames.Contains(n_socket))
			{
				if (message.Num() >= 7)
				{
					float time = FCString::Atof(*message[6]);
					SelObj = LSkeletal[FName(*message[1])];
					FString SelSocket = message[2];

					FRotator ini = components[0]->GetSocketRotation(n_socket);
					FVector initial = FVector(ini.Pitch, ini.Yaw, ini.Roll);
					FVector difference = FVector(FCString::Atof(*message[3]) - initial.X, FCString::Atof(*message[4]) - initial.Y, FCString::Atof(*message[5]) - initial.Z);
					ModifyingActors.Emplace(SelObj, SelSocket, time, difference, initial, true);
				}
				else
					pawn->EmplaceRotation(n_socket, FRotator(FCString::Atof(*message[3]), FCString::Atof(*message[4]), FCString::Atof(*message[5])));
			}
			else
				SendReponse("Socket wasn't found");
		}
	}
	else
		SendReponse("Skeletal mesh wasn't found");
}

void AROXServer::GetSocketRotation(TArray<FString>& message)
{
	FindActorInList(FName(*message[1]));
	if (SelObj)
	{
		SocketNames.Empty();
		TArray<USkeletalMeshComponent*> components;
		SelObj->GetComponents<USkeletalMeshComponent>(components);
		SocketNames = components[0]->GetAllSocketNames();

		FName n_socket = FName(*message[2]);
		if (SocketNames.Contains(n_socket))
		{
			FRotator rot = components[0]->GetSocketRotation(n_socket);
			SendReponse(FString::Printf(TEXT("%f %f %f"), rot.Pitch, rot.Yaw, rot.Roll));
		}
		else
			SendReponse("Can't find the socket");
	}
}


/////////////////////////////
///////// CAMERAS //////////
///////////////////////////



void AROXServer::CameraList(TArray<FString>& message)
{
	FString reponse;
	LCamera.Empty();
	for (TObjectIterator<AActor> itr; itr; ++itr)
	{
		TArray<UCameraComponent*> components;
		(*itr)->GetComponents<UCameraComponent>(components);

		if (components.Num() > 0)
		{
			LCamera.Emplace(*(*itr)->GetName(), (*itr));
			reponse += FString::Printf(TEXT("%s "), *(*itr)->GetName());
		}
	}

	SendReponse(reponse);
}


void AROXServer::SpawnCamera(TArray<FString>& message)
{
	if (LCamera.Contains(FName(*message[1])))
	{
		SendReponse("This name is already use");
	}
	else
	{
		UWorld* world = GetWorld();
		ACameraActor* new_camera = world->SpawnActor<ACameraActor>();
		new_camera->SetActorLocation(FVector(FCString::Atof(*message[2]), FCString::Atof(*message[3]), FCString::Atof(*message[4])));
		new_camera->SetActorLabel(message[1]);
		LCamera.Emplace(FName(*new_camera->GetName()), new_camera);
	}
}

void AROXServer::CameraLookAt(TArray<FString>& message)
{
	if (LCamera.Contains(FName(*message[1])))
	{
		if (message.Num() == 3)
		{
			if (LActor.Contains(*message[2]))
			{
				FVector Forward = LActor[*message[2]]->GetActorLocation() - LCamera[*message[1]]->GetActorLocation();
				FRotator Rot = UKismetMathLibrary::MakeRotFromXZ(Forward, FVector::UpVector);
				LCamera[*message[1]]->SetActorRotation(Rot);
			}
			else
				SendReponse("Actor wasn't found");
		}
		else if (message.Num() == 5)
		{
			FVector Forward = FVector(FCString::Atof(*message[2]), FCString::Atof(*message[3]), FCString::Atof(*message[4])) - LCamera[*message[1]]->GetActorLocation();
			FRotator Rot = UKismetMathLibrary::MakeRotFromXZ(Forward, FVector::UpVector);
			LCamera[*message[1]]->SetActorRotation(Rot);
		}
		else
			SendReponse("Invalid number of arguments");
	}
	else
		SendReponse("Camera wasn't found");
}



/////////////////////////////////
/////////// UNREALROX //////////
///////////////////////////////

void AROXServer::GetBoundingBox(TArray<FString>& message)
{
	if (LActor.Contains(*message[1]))
	{
		UMeshComponent* mesh_component = dynamic_cast<UMeshComponent*>(LActor[*message[1]]->GetComponentByClass(UMeshComponent::StaticClass()));
		if (mesh_component)
		{
			FBox bounding = LActor[*message[1]]->GetComponentsBoundingBox(true);
			FVector bounding_list[8];
			bounding_list[0] = bounding.Min;
			Integrate(&bounding_list[1], bounding.Min, bounding.Max);
			Integrate(&bounding_list[4], bounding.Max, bounding.Min);
			bounding_list[7] = bounding.Max;
			
			FString reponse;
			for (FVector v : bounding_list)
			{
				reponse += FString::Printf(TEXT("%f %f %f \n"), v.X, v.Y, v.Z);
			}
			SendReponse(reponse);
		}
		else
			SendReponse("It doesn't have a StaticMeshComponent");
	}
}

void AROXServer::Integrate(FVector* pointer, FVector& min, FVector& max) {
	for (size_t i = 0; i < 3; i++)
	{
		*pointer = min;
		(*pointer)[i] = max[i];
		pointer++;
	}
}

void AROXServer::ScenePhysicsEnabled(TArray<FString>& message)
{
	if (Physics)
		SendReponse("true");
	else
		SendReponse("false");
}

void AROXServer::ToggleScenePhysics(TArray<FString>& message)
{
	if (message[1] == "true")
		Physics = true;
	else
		Physics = false;

	for (auto act : LObjMov)
	{
		UMeshComponent* mesh_c = Cast<UMeshComponent>(act.Value->GetComponentByClass(UMeshComponent::StaticClass()));
		mesh_c->SetEnableGravity(Physics);
		mesh_c->SetSimulatePhysics(Physics);
	}
}

void AROXServer::SetCameraStereo(TArray<FString>& message)
{
	if (LCamera.Contains(FName(*message[1])))
	{

	}
}

void AROXServer::IsCameraStereo(TArray<FString>& message)
{
	if (LCamera.Contains(FName(*message[1])))
	{

	}
}

void AROXServer::SetOutFrameResolution(TArray<FString>& message)
{

}

void AROXServer::SetOutFramePath(TArray<FString>& message)
{

}

void AROXServer::GetRGB(TArray<FString>& message)
{

}

void AROXServer::GetDepth(TArray<FString>& message)
{

}

void AROXServer::GetNormal(TArray<FString>& message)
{

}

void AROXServer::GetInstanceMask(TArray<FString>& message)
{

}