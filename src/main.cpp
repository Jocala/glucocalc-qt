#include <QApplication>
#include "mainwindow.h"
#include <QIcon>
#include <QFontDatabase>
#include <QStyleFactory>
#include <QFile>
#include <QScreen>
#include <QStyle>

int main(int argc, char *argv[]){
    qputenv("QT_AUTO_SCREEN_SCALE_FACTOR", "0");
    qputenv("QT_ENABLE_HIGHDPI_SCALING", "0");
    QApplication app(argc, argv);
    app.setApplicationVersion("2.0");
    app.setWindowIcon(QIcon(":/AppIcon-512.png"));

    int fontId = QFontDatabase::addApplicationFont(":/assets/DejaVuSans.ttf");
    if(fontId != -1){
        QString family = QFontDatabase::applicationFontFamilies(fontId).at(0);
        QFont font(family);
        font.setPixelSize(12);
        app.setFont(font);
    }
    app.setStyle(QStyleFactory::create("Fusion"));
    QFile styleFile(":/style.qss");
    if(styleFile.open(QFile::ReadOnly)){
        QString styleSheet = QLatin1String(styleFile.readAll());
        app.setStyleSheet(styleSheet);
        styleFile.close();
    }

    MainWindow w;
    if(auto *screen = app.primaryScreen())
        w.setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, w.size(), screen->availableGeometry()));
    w.show();
    return app.exec();
}
