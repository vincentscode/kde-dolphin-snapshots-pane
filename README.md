# KDE Dolphin Snapshots Pane

This plugin adds a pane with snapshot information to Dolphin's properties dialog.

![Dolphin Snapshots Pane](.github/assets/snapshots-pane-screenshot.png)

It currently checks for snapshots in the following directories:
- `.snap`
- `.snapshot`
- `.snapshots`
- `.zfs/snapshot`

## Usage

To use the Snapshots Pane in Dolphin, right-click on a file or folder and select "Properties".
In the properties dialog, navigate to the "Snapshots" tab to view snapshots of the selected file or folder.
You can open a snapshot by double-clicking on it in the list.

## Building

```bash
git clone https://github.com/vincentscode/kde-dolphin-snapshots-pane.git
cd kde-dolphin-snapshots-pane

mkdir build
cd build

cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .

# optional (install)
sudo cmake --build . --target install
```
