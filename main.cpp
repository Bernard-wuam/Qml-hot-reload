#include <QCoreApplication>
#include "filegenerationclass.h"

int main(int argc, char *argv[])
{
  QCoreApplication a(argc, argv);
  FileGenerationClass d("learingGit");
  d.writeToProject ();
  return a.exec();
}
