// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Animation/SkeletalMeshActor.h"
#include "AssetData.h"
#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "GameFramework/Actor.h"
#include "Sockets.h"
#include "ROXTestBasePawn.h"
#include "ROXServer.generated.h"

UCLASS()
class UNREALROX_PLUS_API AROXServer : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AROXServer();
	~AROXServer();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	// Create a socket that listen to a Port and a IP given
	FSocket* CreateListener();
	// Check if there is a new connection or if the actual
	// connection has received a new message
	void UpdateConnectionListener();
	// Listen to a new message of the connection
	void ListenConnection();
	void SendReponse(FString reponse);

	FSocket* ListenerSocket, *ConnectionSocket;

	// Server's port
	UPROPERTY(EditAnywhere)
		int Port = 8890 ;

	// Create list of assets in the specifiy path
	void CreateAssetsList();
	
	// Assets' route
	UPROPERTY(EditAnywhere)
		FString AssetsPath = "/Game";

	// Functions of list's comands
	void ObjectList(TArray<FString>& message);
	void AssetsList(TArray<FString>& message);
	void SkeletalList(TArray<FString>& message);
	void CameraList(TArray<FString>& message);
	void SocketList(TArray<FString>& message);
	void ActorList(TArray<FString>& message);

	// Lists variables
	TMap<FName, AActor*> LObjMov;
	TMap<FName, AActor*> LSkeletal;
	TMap<FName, AActor*> LActor;
	TMap<FName, AActor*> LCamera;
	TArray<FName> SocketNames;
	TMap<FString, FAssetData> LAssets;

	// Transformation's commands
	void Move(TArray<FString>& message);
	void MoveSocket(TArray<FString>& message);
	void Rotate(TArray<FString>& message);
	void RotateSocket(TArray<FString>& message);
	void Scale(TArray<FString>& message);

	void GetLocation(TArray<FString>& message);
	void GetRotation(TArray<FString>& message);
	void GetScale(TArray<FString>& message);
	void GetSocketLocation(TArray<FString>& message);
	void GetSocketRotation(TArray<FString>& message);


	void SpawnActor(TArray<FString>& message);
	void SpawnCamera(TArray<FString>& messsage);
	void CameraLookAt(TArray<FString>& messsage);
	void ChangeTexture(TArray<FString>& message);
	void ChangeAssetsPath(TArray<FString>& message);

	// UnrealROX's commands
	void GetBoundingBox(TArray<FString>& message);
	void ToggleScenePhysics(TArray<FString>& message);
	void ScenePhysicsEnabled(TArray<FString>& message);
	void SetCameraStereo(TArray<FString>& message);
	void IsCameraStereo(TArray<FString>& message);
	void SetOutFrameResolution(TArray<FString>& message);
	void SetOutFramePath(TArray<FString>& message);
	void GetRGB(TArray<FString>& message);
	void GetDepth(TArray<FString>& message);
	void GetNormal(TArray<FString>& message);
	void GetInstanceMask(TArray<FString>& message);


	AActor* SelObj;
	void FindActorInList(FName n);
	void Integrate(FVector* pointer, FVector& min, FVector& max);
	bool Physics = true;


	//Update the transform of actors in the modifying array
	void UpdateTransform(float dtime);

	class TimeModifyingActor
	{
		public:
			TimeModifyingActor(AActor* a, FString s, float tt, FVector d, FVector i, bool r)
				: Act(a), Socket(s), TotalTime(tt), CurrentTime(0.f), Difference(d), Initial(i), Rotating(r)
			{
			}

			AActor* Act;
			FString Socket;
			float TotalTime;
			float CurrentTime;
			FVector Difference;
			FVector Initial;
			bool Rotating;
	};
	TArray<TimeModifyingActor> ModifyingActors;

	struct NameToFunct
	{
		FString		Name; // Name of the command 
		void (AROXServer::*PFunc) (TArray<FString>& message); // Function that executes 
		uint8		Parameters; // Parameters that must take
	};
	TArray<NameToFunct> Mapping;


};
