// Fill out your copyright notice in the Description page of Project Settings.

#include "ROXSingleton.h"

UROXSingleton::UROXSingleton(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer) {
	eventsHandler = ObjectInitializer.CreateDefaultSubobject<UROXEventHandler>(this, TEXT("EventHandler"));
	//CALL OUTSIDE
	//eventsHandler = Cast<UROXTrackerEventHandler>(GEngine->GameSingleton);
}


