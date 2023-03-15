#ifndef FILEGENERATIONCLASS_H
#define FILEGENERATIONCLASS_H
#include <QDir>
#include <QFile>
#include <QTextStream>


class FileGenerationClass
{
  QString fileName;

  std::string mainCpp = R"(#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "componentcreatorengine.h"

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
  QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
  QGuiApplication app(argc, argv);
  qputenv ("MAIN_QML","../%1/main.qml");

  //QQmlApplicationEngine engine;
  ComponentCreatorEngine engine;

  engine.rootContext ()->setContextProperty("QQmlEngine",&engine);
  engine.addImportPath ("../%1/");
  const QUrl url(qgetenv("MAIN_QML"));
  QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                   &app, [url](QObject *obj, const QUrl &objUrl) {
    if (!obj && url == objUrl)
      QCoreApplication::exit(-1);
  }, Qt::QueuedConnection);
  engine.load(url);

  return app.exec();
})";

std::string fileMonitoringclassHpp = R"(#ifndef FILEMONITORINGCLASS_H
#define FILEMONITORINGCLASS_H

#include <QObject>
#include <QCoreApplication>
#include <iostream>
#include <QDir>
#include <QDebug>
#include <QFileSystemWatcher>
#include <QTimer>
#include <thread>


class FileMonitoringClass : public QObject
{
    Q_OBJECT

    QString fileRoot = "C:/Users/USER/source/QtRepos/";
    QString fileName;
    QFileSystemWatcher filewatcher;

    void beginWatchFile(const QString&);

  public:
    explicit FileMonitoringClass(const QString&, const QString& ,QObject *parent = nullptr);

  public slots:
    void onFileChanged(const QString&);
    void onDirectoryChanged(const QString&);

    signals:
    void reloadUI();
};
#endif // FILEMONITORINGCLASS_H)";

std::string fileMonitoringclassCpp = R"(#include "filemonitoringclass.h"

FileMonitoringClass::FileMonitoringClass(const QString& fileRoot,const QString& fileName ,QObject* parent):
  fileRoot{fileRoot},fileName{fileName},QObject{parent}{
  beginWatchFile (fileRoot + fileName);
//  m_time.setInterval (10);
//  m_time.setSingleShot (true);
 // m_time.start ();
  QObject::connect (&filewatcher, &QFileSystemWatcher::fileChanged,this,&FileMonitoringClass::onFileChanged);
  QObject::connect (&filewatcher, &QFileSystemWatcher::fileChanged,this,&FileMonitoringClass::reloadUI);
  QObject::connect (&filewatcher, &QFileSystemWatcher::directoryChanged,this,&FileMonitoringClass::onDirectoryChanged);
  //QObject::connect (&m_time, &QTimer::timeout,this, &FileMonitoringClass::reloadUI);

  //for testing purpose, remove after testing completed......
  QObject::connect (&filewatcher, &QFileSystemWatcher::fileChanged,this,[](const QString& fileName){ qInfo() << "file changed..." << fileName; });
}

//this function add the various file in a directory to the file watcher recurssively.
void FileMonitoringClass::beginWatchFile(const QString& fileName){

  QDir directory(fileName);

  if(!directory.exists ()){
    qInfo() << "This Directory " << directory << "does not exist";
    return;
  }

  QStringList qmlFileList = directory.entryList (QStringList() << "*.qml");
  if(qmlFileList.count () <= 0)
  {
    qInfo() << "no qml file found in this directory";
    return;
  }

  for(const auto& m: qmlFileList){
    auto u = directory.absoluteFilePath (m);
    qInfo() << u;
    filewatcher.addPath (u);
   // m_time.start ();
  }


  auto qmfileDirectory = directory.entryList (QStringList() << "*",QDir::Dirs | QDir::NoDot | QDir::NoDotAndDotDot | QDir::NoSymLinks);
  for( auto& m:  qmfileDirectory)
    beginWatchFile (directory.absolutePath () + "/" + m);

}

void FileMonitoringClass::onFileChanged(const QString& file)
{
  QFile f(file);
  std::this_thread::sleep_for(std::chrono::microseconds(1));
  if(!f.exists ())
  {
    qInfo() << "file does not exist..." << file;
   std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
  filewatcher.addPath (file);
}
void FileMonitoringClass::onDirectoryChanged(const QString& directory)
{
  Q_UNUSED(directory);
  filewatcher.removePaths (filewatcher.files ());
  beginWatchFile (fileRoot + fileName);
}
)";

std::string cmakeListTxt = R"(cmake_minimum_required(VERSION 3.14)

project(%1 VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Quick)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Quick)

set(PROJECT_SOURCES
        main.cpp
        componentcreatorengine.cpp
        componentcreatorengine.h
        filemonitoringclass.cpp
        filemonitoringclass.h
        qml.qrc
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(%1
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET %1 APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(%1 SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(%1
          ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(%1
  PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Quick)

set_target_properties(%1 PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

install(TARGETS %1
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

if(QT_VERSION_MAJOR EQUAL 6)
    qt_import_qml_plugins(%1)
    qt_finalize_executable(%1)
endif()
)";

std::string componentCreatorEngineHpp = R"(#ifndef COMPONENTCREATORENGINE_H
#define COMPONENTCREATORENGINE_H

#include <QObject>
#include <QQmlApplicationEngine>
#include "filemonitoringclass.h"


class ComponentCreatorEngine : public QQmlApplicationEngine
{
    Q_OBJECT

  public:
    explicit ComponentCreatorEngine(QObject *parent = nullptr);
    Q_INVOKABLE void clearCache();
  signals:
    void reloadUI();
  private:
    FileMonitoringClass* m_filemonitoringclass;
};

#endif // COMPONENTCREATORENGINE_H

)";


std::string componentCreatorEngineCpp = R"(#include "componentcreatorengine.h"

ComponentCreatorEngine::ComponentCreatorEngine(QObject *parent)
  : QQmlApplicationEngine{parent}
{
 m_filemonitoringclass = new  FileMonitoringClass("C:/Users/USER/source/QtRepos/","%1",this);
 QObject::connect (m_filemonitoringclass,&FileMonitoringClass::reloadUI,this,&ComponentCreatorEngine::reloadUI);
}

void ComponentCreatorEngine::clearCache()
{
  this->clearComponentCache ();
}
)";

std::string mainQml = R"(import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.5

Window {
    width: 400
    height: 480
    visible: true
    title: qsTr("Hello World")
    x:970
    y:30

    Loader{
        id:loader
        anchors.fill: parent
        source: "./MainWindow.qml"

        function reload(){
            loader.source = "";
            QQmlEngine.clearCache();
            loader.source = "./MainWindow.qml"
        }

        Connections{
            target: QQmlEngine
            function onReloadUI(){
                loader.reload()
            }
        }
    }
}
)";

std::string mainWindow = R"(import QtQuick 2.15
import Qt5Compat.GraphicalEffects
import QtQuick.Controls
import QtQuick.Layouts

Item {


}
)";

  public:
    FileGenerationClass(QString);
    void writeFile(const std::string&,const QString&) const;
    void writeToProject() const;
};

#endif // FILEGENERATIONCLASS_H
