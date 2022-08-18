//
// Created by MRV on 8/18/2022.
//

#ifndef MAGICBOXREDUXCMAKE_MBREDUXAPPLICATION_H
#define MAGICBOXREDUXCMAKE_MBREDUXAPPLICATION_H

#include <QApplication>
#include "AppSettings.h"
#include "Downloader.h"
#include "MainWindow.h"
class MBReduxApplication : public QApplication{
protected:
    Q_OBJECT
    AppSettings _app_settings;
    Downloader _downloader;
    MainWindow _main_window;
public:
    MBReduxApplication(int& argc, char** argv);
protected slots:
    void exitApp();
};


#endif //MAGICBOXREDUXCMAKE_MBREDUXAPPLICATION_H
