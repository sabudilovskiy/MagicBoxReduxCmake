#include "SettingsWindow.h"
#include "xml.h"
#include <QApplication>
#include "AppSettings.h"
#include "Downloader.h"
#include "ModDownloader.h"
#include "MainWindow.h"
#include "Mod.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    AppSettings app_settings;
    Downloader downloader;
    MainWindow main_window(app_settings, downloader);
    main_window.show();
    return a.exec();
}
