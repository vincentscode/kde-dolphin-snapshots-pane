#pragma once

#include <QWidget>

class KPropertiesDialog;

class SnapshotsPane : public QWidget
{
    Q_OBJECT

public:
    SnapshotsPane(const QString &filePath, KPropertiesDialog *props);
};
