#ifndef CALCULATOR_H
#define CALCULATOR_H

#include <QtCore/QtGlobal>

class Calculator
{
public:
    enum Unit { US, UK };
    static double eAGFromHbA1c(double a1c, Unit unit);
    static double hbA1cFromEAG(double eAG, Unit unit);
    static double mgDlToMmolL(double mgdl);
    static double mmolLToMgDl(double mmol);
    static double percentToMmolMol(double pct);
    static double mmolMolToPercent(double mmolmol);
};

#endif // CALCULATOR_H
