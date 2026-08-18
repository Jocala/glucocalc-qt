#ifndef THEME_H
#define THEME_H

#include <QString>

class QApplication;

namespace Theme {

    // Per-theme colors used by MainWindow's inline stylesheets.
    struct Colors {
        QString cardBg;       // display card background
        QString segTrack;     // segmented control track / digit key background
        QString keyUtility;   // utility key (C, backspace) background
        QString keyPressed;   // keypad pressed state
        QString accent;       // accent blue (checked seg, "=" key)
        QString textPrimary;  // prominent text (input, result values, keypad digits)
        QString textSecondary;// descriptive text (prompt, result labels)
        QString border;       // control borders (help button)
        QString helpBg;       // help button background
        QString helpPressed;  // help button pressed background
    };

    bool isDark();
    void setDark(bool dark);
    QString styleSheet();   // full QSS for the current theme (":/style.qss" or ":/style-dark.qss")
    Colors colors();        // inline-style colors for the current theme
    void apply(QApplication *app);  // sets app stylesheet + Fusion palette

} // namespace Theme

#endif // THEME_H