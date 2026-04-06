// Copyright (c) Rogalev Nikita (Syvtyvkar)

#pragma once

// System search text
#include "Containers/Map.h"
#include "Internationalization/Text.h"
#include "Engine/DeveloperSettings.h"
#include "SyvLocalizationSetting.h"

// Custom localization
#include "CoreMinimal.h"
#include "SyvLocalizationManager.generated.h"

/**
 * @brif Class singltone localization
 */

UCLASS()
class SYVLOCALIZATION_API USyvLocalizationManager : public UObject
{
	GENERATED_BODY()

    /*This Singletone*/
    static USyvLocalizationManager* Instance;
	
public:

    /* Registers the current localization after the project is launched. In fact, it applies the default localization. */
    void RegisterLocalization() { ApplyLocaleText(GetLocaleText()); };

    // ---- Text Localization

    /* For users. Specifies which current text localization is set. */
    UFUNCTION(BlueprintCallable, Category = "Localization")
    const FString GetPathLocalizationDir() { return FPaths::ProjectContentDir() / USyvLocalizationSetting::Get().TextLocalizationPath; };

    /* If we want to get a list of all text localization files that are currently available in the folder */
    UFUNCTION(BlueprintCallable, Category = "Localization")
    const TArray<FString> CheckAllLocaleText();

    /* Current text localization. Works with configuration files */
    UFUNCTION(BlueprintCallable, Category = "Localization")
    const FString GetLocaleText();

    /* Allows you to get the "localized" text of the localization name directly from the localization file itself. 
    The Localization key is used for this. It is convenient that we store the name of the localization in it.*/
    UFUNCTION(BlueprintCallable, Category = "Localization")
    const FText GetLocaleTextDisplay(FString InLocale);

    /* Indicates which localization is used by default. */
    UFUNCTION(BlueprintCallable, Category = "Localization")
    const FString GetDefaultLocale() { return USyvLocalizationSetting::Get().DefaultLocalization; };

    /* Apply localization by cultural code (For example, en, ru...) */
    UFUNCTION(BlueprintCallable, Category = "Localization")
    void ApplyLocaleText(const FString& CultureCode);

    // ---- Manager procedures

    /* Get access to the manager. Implements a singleton */
    UFUNCTION(BlueprintPure, Category = "Localization")
    static USyvLocalizationManager* GetLocalizationManager();

private:

    /* In this version, all texts have the same Namespace, 
    but nothing prevents it from being separated after a small modification of the code! */
    FName GameTextStringTableName = "GameText";
};
