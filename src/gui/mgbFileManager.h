#ifndef MGB_FILEMANAGER_H
#define MGB_FILEMANAGER_H

#include <QObject>
#include <QFileSystemWatcher>
#include <QTimer>
#include <QString>
#include <QElapsedTimer>

class MainWindow;

namespace mgb {

class FileManager : public QObject {
    Q_OBJECT

public:
    explicit FileManager(MainWindow *window, QObject *parent = nullptr);
    void setMonitoredFile(const QString &filePath);
    void setAutoSaveEnabled(bool enable);

public slots:
    void onFileChangedOnDisk(const QString &path);
    void onAppDocumentModified();

private slots:
    void executeReload();
    void executeAutoSave();

private:
    MainWindow *mainWindow;
    QFileSystemWatcher *watcher;
    
    QTimer *reloadTimer;
    QTimer *saveTimer;
    QElapsedTimer lastSaveTimer; // Prevents infinite sync loops
    
    QString currentFile;
    bool autoSaveEnabled;
};

} // namespace mgb

#endif // MGB_FILEMANAGER_H