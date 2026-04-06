// Copyright (c) Rogalev Nikita (Syvtyvkar)


#include "SyvLocalizationManager.h"
#include "Internationalization/StringTableRegistry.h"
#include "Internationalization/StringTableCoreFwd.h"
#include "Internationalization/StringTableCore.h"
#include "Engine/AssetManager.h"

/** 
 * @brief The section in the configuration INI file responsible for localization settings
 * @details Values are stored in DefaultGame.ini / GameUserSettings.ini
 */
#define LOCALIZATION_CATEGORY_CONFIG FString("LocalizationSystem")

/** 
 * @brief Configuration key to store the currently selected language
 * @example Language=ru
 */
#define LOCALIZATION_TEXT_NAME_CONFIG FString("Language")

/** 
 * @brief Static instance of the localization manager singleton
 * @note is initialized lazily via GetLocalizationManager()
 */
USyvLocalizationManager* USyvLocalizationManager::Instance = nullptr;

/**
* @brief Console debugging command: language change in runtime
 * @usage Localization.Text [culture_code]
 * @example Localization.Text ru | Localization.Text en
 * @param Args Command arguments (culture code was expected, for example "ru", "en")
* @param World Pointer to the world (not used in this implementation)
 */
static FAutoConsoleCommandWithWorldAndArgs DebugC_ChangeLocalizationTextCommand(
    TEXT("Localization.Text"),
    TEXT("Localization.Text [Code culture]"),
    FConsoleCommandWithWorldAndArgsDelegate::CreateLambda([](const TArray<FString>& Args, UWorld* World)
        {
            // Checking for at least one argument
            if (!Args.IsValidIndex(0)) return;

            // Applying the selected locale through the manager
            USyvLocalizationManager::GetLocalizationManager()->ApplyLocaleText(*Args[0]);
        })
);

/**
 * @brief Сканирует директорию локализации и возвращает список доступных языковых кодов
 * @details 
 * - Рекурсивно ищет файлы по шаблону из настроек (например, "*.json")
 * - Извлекает имя файла без пути и расширения, предполагая, что это код культуры
 * @return Массив строк с кодами культур (например: ["ru", "en", "de"])
 * @note Требует корректной настройки FormatTextLocalization в SyvLocalizationSetting
 */
const TArray<FString> USyvLocalizationManager::CheckAllLocaleText()
{
    // Getting the root path to the localization files
    const FString Path = GetPathLocalizationDir();
    TArray<FString> LAllLocale;                    // Temporary array for full file paths
    TArray<FString> LAllLocaleResult;              // Final array with crop codes

    // Recursive file mask search (for example, "*.json")
    // Parameters: recursion=true, exclude directories=false
    IFileManager::Get().FindFilesRecursive(LAllLocale, *Path, *FString("*" + USyvLocalizationSetting::Get().FormatTextLocalization), true, false);

    // Processing of found paths: extracting the culture code from the file name
    for (FString LST : LAllLocale)
    {
        FString LSTR = LST;

        // Removing the root path from the full file path
        LSTR = LSTR.Replace(*Path,TEXT(""),ESearchCase::IgnoreCase);
        // Deleting the file extension (for example, ".json")
        LSTR = LSTR.Replace(*USyvLocalizationSetting::Get().FormatTextLocalization, TEXT(""), ESearchCase::IgnoreCase);
        // Adding the cleared culture code to the result
        LAllLocaleResult.Add(LSTR);
    }

    return LAllLocaleResult;
}

/**
* @brief Gets the current active culture code from the configuration
 * @details
* - Reads the value from GGameIni in the LOCALIZATION_CATEGORY_CONFIG section
* - If the key is not found, returns the default language via GetDefaultLocale()
* @return A string with the culture code (for example: "ru", "en") or an empty string in case of an error
 */
const FString USyvLocalizationManager::GetLocaleText()
{
    FString LLocalText;

    // Attempt to read the value from the configuration file
    bool LResult = GConfig->GetString(
        *LOCALIZATION_CATEGORY_CONFIG,
        *LOCALIZATION_TEXT_NAME_CONFIG,
        LLocalText,
        GGameIni
    );

    // If the reading failed, we return the default localization
    if (!LResult)
    {
        return GetDefaultLocale();
    }

    return LLocalText;
}

/**
* @brief Extracts the displayed language name from its localization JSON file
 * @param InLocale Culture code to get a name for (for example, "ru")
* @return FText with the localized language name or an empty FText in case of an error
 * @details
* - Downloads a JSON file along the path: [LocalizationDir]/[InLocale][Format]
 * - Searches for the key specified in the NameLocalizationKey of the settings (for example, "LanguageName")
* - Returns the value as FText for correct localization in the interface
 */
const FText USyvLocalizationManager::GetLocaleTextDisplay(FString InLocale)
{
    // Creating the full path to the localization file
    const FString Path = GetPathLocalizationDir() + InLocale + USyvLocalizationSetting::Get().FormatTextLocalization;
    FString JsonString;

    // Loading the contents of the file into a string
    if (FFileHelper::LoadFileToString(JsonString, *Path))
    {
        TSharedPtr<FJsonObject> JsonObject;
        TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);

        // Deserializing JSON into the object model
        if (FJsonSerializer::Deserialize(Reader, JsonObject))
        {
            // Iterating through all key-value pairs in the JSON root
            for (auto& Entry : JsonObject->Values)
            {
                // Processing only string values (skipping arrays/objects)
                if (Entry.Value->Type == EJson::String) // Read string localization
                {
                    // Comparing the key with the expected name for the displayed language name
                    if (FTextKey(Entry.Key) == USyvLocalizationSetting::Get().NameLocalizationKey)
                    {
                        return FText::FromString(Entry.Value->AsString());
                    }
                }
            }
        }
    }
    
    // We return empty text if the file is not found or the key is missing
    return FText();
}

/**
* @brief Applies a new localization: loads rows, registers a table, updates resources
 * @param CultureCode The culture code to activate (for example, "ru", "en", "fr")
* @details
* 1. Downloads a JSON file with translations for the specified culture
* 2. Parses JSON and fills in the FStringTable with key-value pairs
* 3. Cancels registration of the old row table (if was)
* 4. Registers a new table in the global registry
* 5. Updates the resources of the text system to apply the changes
* 6. Saves the selected language to the configuration for saving between sessions
 * @warning The function does not verify the existence of the file — it is recommended to validate the CultureCode in advance
 */
void USyvLocalizationManager::ApplyLocaleText(const FString& CultureCode)
{
    // Creating the path to the localization file for a given culture
    const FString Path = GetPathLocalizationDir() + CultureCode + USyvLocalizationSetting::Get().FormatTextLocalization;
    FString JsonString;

    // Creating a new dynamic row table for storing translations
    FStringTableRef LLocalizationDynamicTable = FStringTable::NewStringTable();

    // Uploading a JSON file with translations
    if (FFileHelper::LoadFileToString(JsonString, *Path))
    {
        TSharedPtr<FJsonObject> JsonObject;
        TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);

        // Parsing the JSON and filling in the row table
        if (FJsonSerializer::Deserialize(Reader, JsonObject))
        {
            // Processing only string localization values
            for (auto& Entry : JsonObject->Values)
            {
                // Adding a pair (key, translation) to the table
                if (Entry.Value->Type == EJson::String)
                {
                    LLocalizationDynamicTable.Get().SetSourceString(FTextKey(Entry.Key), Entry.Value->AsString());
                }
            }
        }
        // Cancel registration of the previous table (if it exists)
        FStringTableRegistry::Get().UnregisterStringTable(GameTextStringTableName);
        // Registering a new table with translations by global name
        FStringTableRegistry::Get().RegisterStringTable(GameTextStringTableName, LLocalizationDynamicTable);
        // Forcibly updating all text resources to apply new translations
        FTextLocalizationManager::Get().RefreshResources();

        // Updating the configuration file
        GConfig->SetString(
            *LOCALIZATION_CATEGORY_CONFIG,
            *LOCALIZATION_TEXT_NAME_CONFIG,
            *CultureCode,
            GGameIni
        );
    }
}

/**
* @brief Gets an instance of the localization manager singleton
 * @return Pointer to the active instance of USyvLocalizationManager
 * @details
* - Implements the "lazy initialization" pattern: creates an object on the first call
* - Adds the object to the UObject root to prevent garbage
collection * - Uses GetTransientPackage() for objects that do not require serialization
 * @threadsafe Partially: not protected from multithreaded access at first initialization
 */
USyvLocalizationManager* USyvLocalizationManager::GetLocalizationManager()
{
    if (!Instance)
    {
        Instance = NewObject<USyvLocalizationManager>(GetTransientPackage(), NAME_None);
        Instance->AddToRoot();
    }
    return Instance;
}
