// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

/* USE THIS IF DEVELOPING A PLUGIN */
// #include "UnrealROX_Plus.h"

// #define LOCTEXT_NAMESPACE "FUnrealROX_PlusModule"

// void FUnrealROX_PlusModule::StartupModule()
// {
// 	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
// }

// void FUnrealROX_PlusModule::ShutdownModule()
// {
// 	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
// 	// we call this function before unloading the module.
// }

// #undef LOCTEXT_NAMESPACE
	
// IMPLEMENT_MODULE(FUnrealROX_PlusModule, UnrealROX_Plus)

/* USE THIS IF DEVELOPING A GAME MODULE */

#include "UnrealROX_Plus.h"

IMPLEMENT_PRIMARY_GAME_MODULE(FDefaultGameModuleImpl, UnrealROX_Plus, "UnrealROX_Plus");