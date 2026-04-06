# SyvLocalization

<img width="128" height="128" alt="Icon128" src="https://github.com/user-attachments/assets/260aa515-3706-4d80-8b3e-a4957be7147b" />

Plugin custom Unreal Engine text localization system based JSON. When developing it, I wanted to create a text localization system that allows a simple and convenient system for expanding the localization list, including by users.

## Plugin settings

- **Text Localization Path** - specifies the path to the localization JSON files
- **Format Text Localization** - helps you set up the name format. This makes it easier for the plugin to find new localization packages and what they relate to. In the case below, it is indicated that all localization packages have the form "KEY_LOCALIZATION_text.json", which means, for example, the final name of the form "en_text.json", where en is the cultural code that the plugin uses in other places without opening the localization file itself.
- **Name Localization Key** - this key will be used to search for the "localized" localization name. For example, instead of the cultural key "en", "English" will already be specified inside it. This is convenient if we want to make the system easy for users to expand, which is exactly what I wanted to achieve during development.
- **Defult Localization** - it simply indicates which localization to download if the current localization is not specified in the user configuration.
<img width="812" height="247" alt="image" src="https://github.com/user-attachments/assets/62d466f1-fc0c-4bfe-9022-5761437f24d3" />

## Localization JSON file format

In the current version, the plugin supports the following type of file formatting:

> "KEY": "VALUE"

```
{

  "GameTittle": "MY GAME!",
  "LocalizationVersion": "0.1a",
  "Localization": "English",
  "LocalizationSettingText": "Localization (Text)",
  "LocalizationSettingVoice": "Localization (Voice)",
  "MenuText.Audio": "Audio",
  "MenuText.Back": "Back",
  "MenuText.Codex": "Codex",
  "MenuText.Continue": "Continue",
  "MenuText.Controll": "Controls",
  "MenuText.Exit": "Exit",
  "MenuText.Graphic": "Graphics",
  "MenuText.Load": "Load",
  ... And so on
}
```

## Console command and tools

Supports a console command to change the language, including the engine:

```
Localization.Text [CULTURE KEY]
```

Also, if you have updated the JSON files and want to use their data in the engine without having to change the language (to get new available keys), use the special convenient button shown in the screenshot. She will upload the updated data herself.

<img width="577" height="165" alt="image" src="https://github.com/user-attachments/assets/295214c0-1d40-4054-958b-e93eb4a18217" />

## Instructions for using the plugin

1. Decide in which folder your localization packages will be stored. (For example, create a Localization folder, as in the default settings. If you want a different folder, please update the configuration settings)
2. Create a JSON file of your default localization (for example, English). Pay attention to the format settings specified in the configuration. By default, the plugin accepts the name format "KEY_LOCALIZATION_text.json", which means that you need to create an "en_text" file.json". If you want to change the format, don't forget to change the configuration.
3. Fill in the JSON file with the keys and text you need. I recommend creating a "pseudolanguage" file where you specify only the keys and what they mean in a format that is convenient for you. You will base all subsequent localization packages on it. However, you can do whatever is convenient for you. The file format is specified below.
4. Click on the localization update button (see below).
5. If you copy your JSON file, and name it, for example, "fr_text.json", then you can translate it into French, and by clicking on the localization update button, you can immediately switch languages between "fr" and "en".
6. Now specify the keys where you need localization through the standard StringTable tool. In your case, a virtual StringTable of the localization system will appear.

<img width="470" height="249" alt="image" src="https://github.com/user-attachments/assets/fac4a8ad-ad19-4d8c-b8fe-f6a3fab3737a" />


In fact, everything is already working! But in order not to use the command line, you can use the static SyvLocalizationManager functions: 
- **ApplyLocaleText** - changing the language according to the cultural code
- **GetLocaleText** - get the current cultural code
- **CheckAllLocaleText** - if we want to get a list of all text localization files that are currently available in the folder

# License

You can view the license [here](https://github.com/syvtyvkar/SyvLocalization/blob/main/LICENSE.md)
