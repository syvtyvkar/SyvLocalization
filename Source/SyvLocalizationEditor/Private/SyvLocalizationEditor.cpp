// Copyright (c) Rogalev Nikita (Syvtyvkar)

#include "SyvLocalizationEditor.h"

// SyvLocalization files
#include "SyvLocalization/Core/SyvLocalizationManager.h"

// Add buttons inclues
#include "ToolMenus.h"
#include "Framework/Commands/UICommandList.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Styling/AppStyle.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"
//#include "LevelEditor.h"
#include "Styling/AppStyle.h"
#include "ToolMenuSection.h"

// Macros img
#define BOX_BRUSH( RelativePath, ... ) FSlateBoxBrush( RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define IMAGE_BRUSH( RelativePath, ... ) FSlateImageBrush( RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )

// Namespace loctext
#define LOCTEXT_NAMESPACE "FSyvLocalizationEditorModule"

void FSyvLocalizationEditorModule::StartupModule()
{
    // Added to ToolMenu
    StartupHandle = UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FSyvLocalizationEditorModule::RegisterMenus));
}

void FSyvLocalizationEditorModule::ShutdownModule()
{	
    UToolMenus::UnRegisterStartupCallback(StartupHandle);   // Clear delegate button update
    UToolMenus::UnregisterOwner(this);                   // Unregister owners
}

void FSyvLocalizationEditorModule::AddToolbarButton(FToolBarBuilder& Builder)
{
    UE_LOG(LogTemp, Warning, TEXT("SyvLocalization: Reload button clicked!"));  // Add logs
    Builder.AddToolBarButton(
        FUIAction(FExecuteAction::CreateRaw(this, &FSyvLocalizationEditorModule::OnReloadLocalesClicked)),
        NAME_None,
        LOCTEXT("Update localization file", "Reload Locales"),
        LOCTEXT("Reload localization file", "Force reload all localization files"),
        FSlateIcon(FAppStyle::GetAppStyleSetName(), "LevelEditor.OpenConsole")
    );
}

void FSyvLocalizationEditorModule::OnReloadLocalesClicked()
{
    if (USyvLocalizationManager* Manager = USyvLocalizationManager::GetLocalizationManager())
    {
        Manager->RegisterLocalization();
        UE_LOG(LogTemp, Log, TEXT("Locale update!"));   // Add logs

        // Notification
        FNotificationInfo Info = FNotificationInfo(FText::FromString("Localization file reloaded!"));
        Info.ExpireDuration = 3.0f;
        FSlateNotificationManager::Get().AddNotification(Info);
    }
}

void FSyvLocalizationEditorModule::RegisterMenus()
{
    UE_LOG(LogTemp, Display, TEXT("SyvLocalization: Starting menu registration")); // Add logs

    FToolMenuOwnerScoped OwnerScoped(this);
    
    // Extend the "File" section of the main toolbar
    UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.ModesToolBar");
    FToolMenuSection& ToolbarSection = ToolbarMenu->FindOrAddSection("Tools");

    // Create button reload localization
    ToolbarSection.AddEntry(FToolMenuEntry::InitToolBarButton(TEXT("Reload Localization"),
        FExecuteAction::CreateLambda([&](){OnReloadLocalesClicked();}),
        INVTEXT("Syv Reload Localization"),
        INVTEXT("Force reload all localization data"),
        FSlateIcon(FAppStyle::GetAppStyleSetName(), "Icons.Documentation")));
}

// stop namespace
#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FSyvLocalizationEditorModule, SyvLocalizationEditor)