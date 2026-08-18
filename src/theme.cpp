#include "theme.h"

#include <QApplication>
#include <QFile>
#include <QPalette>
#include <QSettings>
#include <QStyle>

namespace Theme {

namespace {
bool darkCached = false;
bool darkInit = false;

bool readDark()
{
    if (!darkInit) {
        QSettings s(QStringLiteral("jocala"), QStringLiteral("Glucocalc"));
        darkCached = s.value(QStringLiteral("darkMode"), false).toBool();
        darkInit = true;
    }
    return darkCached;
}
} // namespace

bool isDark()
{
    return readDark();
}

void setDark(bool dark)
{
    darkCached = dark;
    darkInit = true;
    QSettings s(QStringLiteral("jocala"), QStringLiteral("Glucocalc"));
    s.setValue(QStringLiteral("darkMode"), dark);
}

QString styleSheet()
{
    const QString path = isDark() ? QStringLiteral(":/style-dark.qss")
                                  : QStringLiteral(":/style.qss");
    QFile f(path);
    if (f.open(QFile::ReadOnly))
        return QString::fromUtf8(f.readAll());
    return QString();
}

Colors colors()
{
    if (isDark()) {
        return { QStringLiteral("#2C2C2E"),  // cardBg (theme-adaptive dark card)
                 QStringLiteral("#3A3A3C"),  // segTrack / digit
                 QStringLiteral("#545458"),  // keyUtility
                 QStringLiteral("#48484A"),  // keyPressed
                 QStringLiteral("#0A84FF"),  // accent
                 QStringLiteral("#FFFFFF"),  // textPrimary
                 QStringLiteral("#AEAEB2"),  // textSecondary
                 QStringLiteral("#545458"),  // border
                 QStringLiteral("#3A3A3C"),  // helpBg
                 QStringLiteral("#2C2C2E") };// helpPressed
    }
    return { QStringLiteral("#9A9A9A"),      // cardBg (theme-adaptive light card)
             QStringLiteral("#E5E5EA"),      // segTrack / digit
             QStringLiteral("#AEAEB2"),      // keyUtility
             QStringLiteral("#C7C7CC"),      // keyPressed
             QStringLiteral("#007AFF"),      // accent
             QStringLiteral("#000000"),      // textPrimary
             QStringLiteral("#333333"),      // textSecondary
             QStringLiteral("#CCCCCC"),      // border
             QStringLiteral("#FFFFFF"),      // helpBg
             QStringLiteral("#E5E5EA") };// helpPressed
}

void apply(QApplication *app)
{
    app->setStyleSheet(styleSheet());

    if (isDark()) {
        QPalette p;
        p.setColor(QPalette::Window, QColor(0x1C, 0x1C, 0x1E));
        p.setColor(QPalette::WindowText, QColor(0xEB, 0xEB, 0xF5));
        p.setColor(QPalette::Base, QColor(0x1C, 0x1C, 0x1E));
        p.setColor(QPalette::AlternateBase, QColor(0x2C, 0x2C, 0x2E));
        p.setColor(QPalette::ToolTipBase, QColor(0x2C, 0x2C, 0x2E));
        p.setColor(QPalette::ToolTipText, QColor(0xEB, 0xEB, 0xF5));
        p.setColor(QPalette::Text, QColor(0xEB, 0xEB, 0xF5));
        p.setColor(QPalette::Button, QColor(0x3A, 0x3A, 0x3C));
        p.setColor(QPalette::ButtonText, QColor(0xEB, 0xEB, 0xF5));
        p.setColor(QPalette::BrightText, QColor(0xFF, 0xFF, 0xFF));
        p.setColor(QPalette::Highlight, QColor(0x0A, 0x84, 0xFF));
        p.setColor(QPalette::HighlightedText, QColor(0xFF, 0xFF, 0xFF));
        p.setColor(QPalette::Link, QColor(0x0A, 0x84, 0xFF));
        p.setColor(QPalette::PlaceholderText, QColor(0x8E, 0x8E, 0x93));
        app->setPalette(p);
    } else {
        app->setPalette(app->style()->standardPalette());
    }
}

} // namespace Theme