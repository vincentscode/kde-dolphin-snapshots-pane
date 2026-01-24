# Localization (i18n)

This plugin now supports localization through KDE's i18n framework.

## Supported Languages

- German (de) - Only "Date" is translated to "Datum"; "snapshot/snapshots" terms remain in English
- US English (en_US)

## Translation Files

Translation files are located in the `po/` directory:

- `kde-dolphin-snapshots-pane.pot` - Template file with all translatable strings
- `de.po` - German translation
- `en_US.po` - US English translation

## Adding New Translations

1. Copy the template file:
   ```bash
   cp po/kde-dolphin-snapshots-pane.pot po/LANG.po
   ```
   Replace `LANG` with your language code (e.g., `fr`, `es`, `it`)

2. Edit the new `.po` file and translate the strings in the `msgstr` fields

3. **Important**: Do NOT translate the words "snapshot" or "snapshots" - keep them in English

## Building with Translations

Translations are automatically compiled and installed when building the plugin:

```bash
mkdir build
cd build
cmake ..
cmake --build .
sudo cmake --build . --target install
```

## Testing Translations

To test translations, set your system locale to the desired language:

```bash
LANGUAGE=de dolphin
```

Then open the properties dialog for a file and navigate to the "Snapshots" tab.
