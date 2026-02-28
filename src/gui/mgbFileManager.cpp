#include "mgbFileManager.h"
#include "mainwindow.h"
#include "tikzdocument.h"
#include "tikzscene.h"

#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QDebug>

namespace mgb {

FileManager::FileManager(MainWindow *window, QObject *parent)
    : QObject(parent), mainWindow(window), autoSaveEnabled(true)
{
    watcher = new QFileSystemWatcher(this);
    
    reloadTimer = new QTimer(this);
    reloadTimer->setSingleShot(true);
    reloadTimer->setInterval(500); 
    
    saveTimer = new QTimer(this);
    saveTimer->setSingleShot(true);
    saveTimer->setInterval(1000); 

    lastSaveTimer.start(); // Start the stopwatch

    connect(watcher, &QFileSystemWatcher::fileChanged, this, &FileManager::onFileChangedOnDisk);
    connect(watcher, &QFileSystemWatcher::directoryChanged, this, &FileManager::onFileChangedOnDisk);
    
    connect(reloadTimer, &QTimer::timeout, this, &FileManager::executeReload);
    connect(saveTimer, &QTimer::timeout, this, &FileManager::executeAutoSave);
}

void FileManager::setMonitoredFile(const QString &filePath)
{
    if (filePath == currentFile) return; // Already watching

    if (!currentFile.isEmpty()) {
        if (watcher->files().contains(currentFile)) watcher->removePath(currentFile);
        QString dir = QFileInfo(currentFile).absolutePath();
        if (watcher->directories().contains(dir)) watcher->removePath(dir);
    }

    currentFile = filePath;

    if (!currentFile.isEmpty()) {
        watcher->addPath(currentFile);
        watcher->addPath(QFileInfo(currentFile).absolutePath()); 
        qDebug() << "[mgbFileManager] Now watching:" << currentFile;
    }
}

void FileManager::setAutoSaveEnabled(bool enable)
{
    autoSaveEnabled = enable;
}

void FileManager::onFileChangedOnDisk(const QString &path)
{
    // Ignore events if WE just saved the file less than 2 seconds ago
    if (lastSaveTimer.isValid() && lastSaveTimer.elapsed() < 2000) {
        return;
    }

    // Atomic Save Recovery (VS Code support)
    if (QFileInfo(path).isDir()) {
        if (QFile::exists(currentFile) && !watcher->files().contains(currentFile)) {
            watcher->addPath(currentFile);
            qDebug() << "[mgbFileManager] Recovered file after VS Code save.";
            reloadTimer->start();
        }
        return;
    }

    if (path != currentFile) return;
    
    qDebug() << "[mgbFileManager] Detected external change!";
    reloadTimer->start();
}

void FileManager::executeReload()
{
    if (currentFile.isEmpty()) return;

    QFile file(currentFile);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        QString newTikz = in.readAll();
        file.close();

        // Only reload if the code actually changed
        if (newTikz.trimmed() != mainWindow->tikzSource().trimmed()) {
            qDebug() << "[mgbFileManager] Updating visual canvas from external edit...";
            
            bool previousSaveState = autoSaveEnabled;
            autoSaveEnabled = false; // Prevent undo stack from triggering our autosave

            if (mainWindow->tikzScene()->parseTikz(newTikz)) {
                mainWindow->refreshTikz();
            } else {
                qDebug() << "[mgbFileManager] Failed to parse external TikZ code.";
            }

            autoSaveEnabled = previousSaveState;
        }
    }
    
    // Ensure the watcher is still attached
    if (QFile::exists(currentFile) && !watcher->files().contains(currentFile)) {
        watcher->addPath(currentFile);
    }
}

void FileManager::onAppDocumentModified()
{
    if (autoSaveEnabled && !currentFile.isEmpty()) {
        saveTimer->start();
    }
}

void FileManager::executeAutoSave()
{
    if (currentFile.isEmpty() || !mainWindow->tikzDocument()) return;
    
    qDebug() << "[mgbFileManager] Auto-saving changes to disk...";
    lastSaveTimer.restart(); // Reset the clock so we ignore the resulting file system events
    mainWindow->tikzDocument()->save();
}

} // namespace mgb