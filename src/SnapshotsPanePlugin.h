#pragma once

#include <QString>

#include <KFileItem>
#include <KPropertiesDialogPlugin>

class SnapshotsPane;

class SnapshotsPanePlugin : public KPropertiesDialogPlugin
{
Q_OBJECT

public:
    SnapshotsPanePlugin(QObject *parent);
};
