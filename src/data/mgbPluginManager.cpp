#include "mgbPluginManager.h"
#include "mgbPluginInterface.h"
#include <QPluginLoader>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QCoreApplication>
#include <QDebug>

namespace mgb {

PluginManager& PluginManager::instance() {
    static PluginManager _instance;
    return _instance;
}

void PluginManager::loadPlugins() {
    if (_isLoaded) return;
    _plugins.clear();
    _interfaces.clear();
    
    QString pluginDirPath = QCoreApplication::applicationDirPath() + "/plugins";
    QDir dir(pluginDirPath);
    if (!dir.exists()) {
        _isLoaded = true;
        return;
    }

    // 1. Load JSON Plugins
    QStringList jsonFilters; jsonFilters << "*.json";
    foreach (QFileInfo fileInfo, dir.entryInfoList(jsonFilters, QDir::Files)) {
        QFile file(fileInfo.absoluteFilePath());
        if (!file.open(QIODevice::ReadOnly)) continue;
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        file.close();

        if (doc.isNull() || !doc.isObject()) continue;
        QJsonObject obj = doc.object();
        
        PluginElement plugin;
        plugin.name = obj["name"].toString();
        plugin.type = obj["type"].toString("node");
        plugin.category = obj["category"].toString("User Plugins");
        QString iconName = obj["icon"].toString();
        plugin.iconPath = !iconName.isEmpty() ? dir.absoluteFilePath(iconName) : ":/images/tikzit-tool-node.svg";
        plugin.tooltip = obj["tooltip"].toString();

        QJsonObject props = obj["tikz_properties"].toObject();
        for (auto it = props.begin(); it != props.end(); ++it) {
            plugin.properties.insert(it.key(), it.value().toString());
        }

        if (!plugin.name.isEmpty()) _plugins.append(plugin);
    }

    // 2. Load Compiled C++ Plugins
    QStringList binaryFilters; binaryFilters << "*.dll" << "*.so" << "*.dylib";
    foreach (QFileInfo fileInfo, dir.entryInfoList(binaryFilters, QDir::Files)) {
        QString dummyError;
        loadCompiledPlugin(fileInfo.absoluteFilePath(), dummyError);
    }

    _isLoaded = true;
}

bool PluginManager::loadCompiledPlugin(const QString &filePath, QString &errorMessage) {
    QPluginLoader loader(filePath);
    QObject *pluginInstance = loader.instance();
    
    if (pluginInstance) {
        ElementPluginInterface *interface = qobject_cast<ElementPluginInterface*>(pluginInstance);
        if (interface) {
            _interfaces.append(interface); // STORE THE C++ INTERFACE
            _plugins.append(interface->getElements()); // STORE THE MENU DATA
            qDebug() << "SUCCESS: Loaded compiled C++ plugin:" << interface->pluginName();
            return true;
        } else {
            errorMessage = "File is a Qt Plugin, but does not match the MGB-UML Interface.";
            loader.unload();
            return false;
        }
    }
    
    errorMessage = loader.errorString();
    return false;
}

QList<PluginElement> PluginManager::getLoadedPlugins() {
    if (!_isLoaded) loadPlugins();
    return _plugins;
}

QList<ElementPluginInterface*> PluginManager::getCompiledInterfaces() {
    if (!_isLoaded) loadPlugins();
    return _interfaces;
}

} // namespace mgb