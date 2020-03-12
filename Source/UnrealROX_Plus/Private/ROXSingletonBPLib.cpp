// Fill out your copyright notice in the Description page of Project Settings.

#include "ROXSingletonBPLib.h"

UROXSingleton* UROXSingletonBPLib::GetROXEvents() {
	UROXSingleton* DataInstance = Cast<UROXSingleton>(GEngine->GameSingleton);

	if (!DataInstance) return NULL;
	if (!DataInstance->IsValidLowLevel()) return NULL;

	return DataInstance;
}


