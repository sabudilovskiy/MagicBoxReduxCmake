#include <MainWindow.h>
#include <QApplication>
#include <MBReduxApplication.h>
int main(int argc, char *argv[])
{
    MBReduxApplication app(argc, argv);
    return app.exec();
}
