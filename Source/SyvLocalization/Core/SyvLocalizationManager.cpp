// Copyright (c) Rogalev Nikita (Syvtyvkar)


#include "SyvLocalizationManager.h"
#include "Internationalization/StringTableRegistry.h"
#include "Internationalization/StringTableCoreFwd.h"
#include "Internationalization/StringTableCore.h"
#include "Engine/AssetManager.h"

/* The localization settings category in the configuration ini file. */
#define LOCALIZATION_CATEGORY_CONFIG FString("LocalizationSystem")

/* The name of the text localization config in the configuration file */
#define LOCALIZATION_TEXT_NAME_CONFIG FString("Language")

/* This singletone*/
USyvLocalizationManager* USyvLocalizationManager::Instance = nullptr;

/* To debug, change the current text localization. */
static FAutoConsoleCommandWithWorldAndArgs DebugC_ChangeLocalizationTextCommand(
    TEXT("Localization.Text"),
    TEXT("Localization.Text [Code culture]"),
    FConsoleCommandWithWorldAndArgsDelegate::CreateLambda([](const TArray<FString>& Args, UWorld* World)
        {
            if (!Args.IsValidIndex(0)) return;

            USyvLocalizationManager::GetLocalizationManager()->ApplyLocaleText(*Args[0]);
        })
);

const TArray<FString> USyvLocalizationManager::CheckAllLocaleText()
{
    const FString Path = GetPathLocalizationDir();
    TArray<FString> LAllLocale;
    TArray<FString> LAllLocaleResult;

    IFileManager::Get().FindFilesRecursive(LAllLocale, *Path, *FString("*" + USyvLocalizationSetting::Get().FormatTextLocalization), true, false);

    for (FString LST : LAllLocale)
    {
        FString LSTR = LST;
        LSTR = LSTR.Replace(*Path,TEXT(""),ESearchCase::IgnoreCase);
        LSTR = LSTR.Replace(*USyvLocalizationSetting::Get().FormatTextLocalization, TEXT(""), ESearchCase::IgnoreCase);
        LAllLocaleResult.Add(LSTR);
    }

    return LAllLocaleResult;
}

const FString USyvLocalizationManager::GetLocaleText()
{
    FString LLocalText;
    bool LResult = GConfig->GetString(
        *LOCALIZATION_CATEGORY_CONFIG,
        *LOCALIZATION_TEXT_NAME_CONFIG,
        LLocalText,
        GGameIni
    );

    if (!LResult)
    {
        return GetDefaultLocale();
    }

    return LLocalText;
}

const FText USyvLocalizationManager::GetLocaleTextDisplay(FString InLocale)
{
    const FString Path = GetPathLocalizationDir() + InLocale + USyvLocalizationSetting::Get().FormatTextLocalization;
    FString JsonString;

    if (FFileHelper::LoadFileToString(JsonString, *Path))
    {
        TSharedPtr<FJsonObject> JsonObject;
        TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);

        if (FJsonSerializer::Deserialize(Reader, JsonObject))
        {
            for (auto& Entry : JsonObject->Values)
            {
                if (Entry.Value->Type == EJson::String) // Read string localization
                {
                    if (FTextKey(Entry.Key) == USyvLocalizationSetting::Get().NameLocalizationKey)
                    {
                        return FText::FromString(Entry.Value->AsString());
                    }
                }
            }
        }
    }
    return FText();
}

void USyvLocalizationManager::ApplyLocaleText(const FString& CultureCode)
{
    const FString Path = GetPathLocalizationDir() + CultureCode + USyvLocalizationSetting::Get().FormatTextLocalization;
    FString JsonString;

    FStringTableRef LLocalizationDynamicTable = FStringTable::NewStringTable();

    if (FFileHelper::LoadFileToString(JsonString, *Path))
    {
        TSharedPtr<FJsonObject> JsonObject;
        TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);

        if (FJsonSerializer::Deserialize(Reader, JsonObject))
        {
            for (auto& Entry : JsonObject->Values)
            {
                if (Entry.Value->Type == EJson::String) // Read string localization
                {
                    LLocalizationDynamicTable.Get().SetSourceString(FTextKey(Entry.Key), Entry.Value->AsString());
                }
            }
        }
        FStringTableRegistry::Get().UnregisterStringTable(GameTextStringTableName);
        FStringTableRegistry::Get().RegisterStringTable(GameTextStringTableName, LLocalizationDynamicTable);
        FTextLocalizationManager::Get().RefreshResources();

        GConfig->SetString(
            *LOCALIZATION_CATEGORY_CONFIG,
            *LOCALIZATION_TEXT_NAME_CONFIG,
            *CultureCode,
            GGameIni
        );
    }
}

USyvLocalizationManager* USyvLocalizationManager::GetLocalizationManager()
{
    if (!Instance)
    {
        Instance = NewObject<USyvLocalizationManager>(GetTransientPackage(), NAME_None);
        Instance->AddToRoot();
    }
    return Instance;
}
