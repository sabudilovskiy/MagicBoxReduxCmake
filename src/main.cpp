#include "SettingsWindow.h"
#include "xml.h"
#include <QApplication>
#include "AppSettings.h"
#include "Downloader.h"
#include "Support.h"
#include "ModDownloader.h"
#include "MainWindow.h"
#include "Mod.h"
#include <string>
#include <magic_enum.hpp>
#include <iostream>
#include <QLibraryInfo>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    AppSettings app_settings;
    Downloader downloader;
    MainWindow main_window(app_settings, downloader);
    main_window.show();
    return a.exec();
}
