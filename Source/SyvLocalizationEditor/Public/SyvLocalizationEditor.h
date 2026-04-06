// Copyright (c) Rogalev Nikita (Syvtyvkar)

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "Framework/Commands/UICommandList.h"
#include "ToolMenus.h"

/*
 * The main task of the module is to add a localization update button. 
 * For example, if you have updated your JSON files and want to see the changes immediately.
 */

/* SyvLocation Editor module */
class FSyvLocalizationEditorModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;			// Start module procedure
	virtual void ShutdownModule() override;			// End module procedure

private:

	FDelegateHandle StartupHandle;					 // Added delegate button update localization

	UFUNCTION()
	void AddToolbarButton(FToolBarBuilder& Builder); // Add a localization update button

	UFUNCTION()
	void OnReloadLocalesClicked();					 // Clicking on the localization update button

	UFUNCTION()
	void RegisterMenus();							 // Custom Button Registration
};
