// Copyright (c) Rogalev Nikita (Syvtyvkar)

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "SyvLocalizationSetting.generated.h"

/**
 * @brif Class localization setting
 */

UCLASS(Config = Game, DefaultConfig)
class SYVLOCALIZATION_API USyvLocalizationSetting : public UDeveloperSettings
{
    GENERATED_BODY()

public:

    /* Constructor */
    USyvLocalizationSetting();

    /* Get access to the settings class */
    static const USyvLocalizationSetting& Get() { return *GetDefault<USyvLocalizationSetting>(); }

    /* Get access to the settings class */
    //UFUNCTION(BlueprintPure, Category = "Story|Settings", DisplayName = "Get Storylines Settings")
    //static const USyvLocalizationSetting* K2_Get() { return GetDefault<USyvLocalizationSetting>(); }

    /* Specifies the path to the localization files. In the final build game, it will look like "/Game/YourPath" */
    UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = Localization)
    FString TextLocalizationPath = FString("Localization/");

    /* The text localization format. Helps the localization system to find all localization files.
    This allows users to write text localization themselves without touching pak files. */
    UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = Localization)
    FString FormatTextLocalization = FString("_text.json");

    /* The key to find the localization name in the json file */
    UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = Localization)
    FString NameLocalizationKey = FString("Localization");

    /* Default localization */
    UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = Localization)
    FString DefaultLocalization = FString("en");
};