#include "filegenerationclass.h"

FileGenerationClass::FileGenerationClass(QString fileName) : fileName{fileName}
{

}

void FileGenerationClass::writeFile(const std::string& file_Name,const QString & d_file) const
{
  QDir u (QDir::currentPath ());
  u.cdUp ();

  const auto& p = u.absolutePath () + QDir::separator () + fileName +  QDir::separator () + d_file;

  QFile file_p(p);

  if(file_p.open (QFile::WriteOnly)){
    QTextStream t(&file_p);
    t << (QString::fromStdString (file_Name)).arg (fileName);

    file_p.flush ();
    file_p.close ();
    return;
  }

  qInfo() << "the file does not exist...";
}

void FileGenerationClass::writeToProject() const
{
  writeFile (componentCreatorEngineHpp,"componentcreatorengine.h");
  writeFile (componentCreatorEngineCpp,"componentcreatorengine.cpp");
  writeFile(mainCpp,"main.cpp");
  writeFile (fileMonitoringclassHpp,"filemonitoringclass.h");
  writeFile (fileMonitoringclassCpp,"filemonitoringclass.cpp");
  writeFile (mainQml,"main.qml");
  writeFile (mainWindow,"MainWindow.qml");
  writeFile (cmakeListTxt,"CMakeLists.txt");
}
