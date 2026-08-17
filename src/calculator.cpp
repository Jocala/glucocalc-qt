#include "calculator.h"

double Calculator::eAGFromHbA1c(double a1c, Unit unit)
{
    if(unit==UK){
        a1c = 0.09148 * a1c + 2.152; // convert mmol/mol to US scale (percentage)
    }
    double eAG = 28.7 * a1c - 46.7; // mg/dL
    return (unit==UK) ? mgDlToMmolL(eAG) : eAG;
}

double Calculator::hbA1cFromEAG(double eAG, Unit unit)
{
    if(unit==UK){
        eAG = mmolLToMgDl(eAG); // convert mmol/L to mg/dL
    }
    double a1c = (eAG + 46.7)/28.7;
    return (unit==UK) ? (10.93 * a1c - 23.5) : a1c;
}

double Calculator::mgDlToMmolL(double mgdl){ return mgdl/18.0; }
double Calculator::mmolLToMgDl(double mmol){ return mmol*18.0; }
double Calculator::percentToMmolMol(double pct){ return 10.93 * pct - 23.5; }
double Calculator::mmolMolToPercent(double mmolmol){ return 0.09148 * mmolmol + 2.152; }

