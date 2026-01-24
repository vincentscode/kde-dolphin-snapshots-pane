# Localization (i18n)

This plugin now supports localization through KDE's i18n framework.

## Supported Languages

- German (de)
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

3. Update the `Language` and `Plural-Forms` headers according to your language

## Updating Translation Template

After modifying source code strings, regenerate the template:

```bash
./Messages.sh
```

This requires the KDE gettext tools to be installed.

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
