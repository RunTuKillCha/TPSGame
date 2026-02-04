// Copyright Epic Games, Inc. All Rights Reserved.

#include "MyFPSGame.h"
#include "Modules/ModuleManager.h"
#include "Misc/Paths.h"





void FMyFPSGameModule::StartupModule()
{

	FString ShaderDirectory = FPaths::Combine(FPaths::ProjectDir(), TEXT("Shaders"));
	AddShaderSourceDirectoryMapping(TEXT("/Project/Shaders"), ShaderDirectory);

}

void FMyFPSGameModule::ShutdownModule()
{
}

IMPLEMENT_PRIMARY_GAME_MODULE(FDefaultGameModuleImpl, MyFPSGame, "MyFPSGame");
