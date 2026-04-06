// Copyright Epic Games, Inc. All Rights Reserved.

#include "SyvLocalization.h"
#include "SyvLocalization/Core/SyvLocalizationManager.h"

#define LOCTEXT_NAMESPACE "FSyvLocalizationModule"

void FSyvLocalizationModule::StartupModule()
{
	USyvLocalizationManager::GetLocalizationManager()->RegisterLocalization();	// Init localization system
}

void FSyvLocalizationModule::ShutdownModule()
{	
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FSyvLocalizationModule, SyvLocalization)