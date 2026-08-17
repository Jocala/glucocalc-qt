#include "helpdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QLabel>
#include <QPushButton>
#include <QApplication>
#include <QDesktopServices>
#include <QUrl>

static QLabel* makeText(const QString &t, bool bold=false, bool underline=false, bool link=false) {
    QLabel *l = new QLabel(t);
    l->setWordWrap(true);
    l->setTextInteractionFlags(Qt::TextBrowserInteraction);
    if(link) l->setOpenExternalLinks(true);
    QFont f = l->font();
    if(bold) f.setBold(true);
    f.setUnderline(underline);
    l->setFont(f);
    return l;
}

static QLabel* sectionTitle(const QString &t) {
    QLabel *l = new QLabel(t);
    QFont f = l->font();
    f.setBold(true);
    f.setUnderline(true);
    l->setFont(f);
    return l;
}

HelpDialog::HelpDialog(QWidget *parent) : QDialog(parent) {
    setWindowTitle("Help");
    setMinimumSize(480, 520);
    resize(520, 600);

    QVBoxLayout *outer = new QVBoxLayout(this);

    QScrollArea *scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    outer->addWidget(scroll);

    QWidget *content = new QWidget;
    QVBoxLayout *lay = new QVBoxLayout(content);
    lay->setSpacing(14);
    lay->setContentsMargins(16,16,16,16);

    // Header
    lay->addWidget(makeText("Glucocalc — Glucose/HbA1c Calculator", true));
    QString ver = QApplication::applicationVersion();
    if(ver.isEmpty()) ver = "2.0";
    lay->addWidget(new QLabel(QString("Version %1").arg(ver)));
    lay->addWidget(new QLabel("(c) 2018-2026 jocala"));
    QLabel *mail = new QLabel("<a href=\"mailto:jocala@jocala.com\">jocala@jocala.com</a>");
    mail->setOpenExternalLinks(true);
    mail->setTextInteractionFlags(Qt::TextBrowserInteraction);
    lay->addWidget(mail);
    QLabel *web = new QLabel("<a href=\"https://www.jocala.com\">https://www.jocala.com</a>");
    web->setOpenExternalLinks(true);
    web->setTextInteractionFlags(Qt::TextBrowserInteraction);
    lay->addWidget(web);

    // Usage
    lay->addWidget(sectionTitle("Usage"));
    lay->addWidget(makeText("Glucocalc calculates estimated HbA1c and estimated average blood glucose. It accepts numeric entries via a single input field. The calculations performed vary based on the state of the checkbox/radiobuttons described below."));
    lay->addWidget(makeText("UK (IFCC) checkbox: Causes Glucocalc to input/output values in mmol/L and mmol/mol."));
    lay->addWidget(makeText("Calculate eAG radiobutton: Calculate eAG from input HbA1c value. The result will be mg/dl or mmol/L depending on the state of the UK (IFCC) checkbox."));
    lay->addWidget(makeText("Calculate HbA1c radiobutton: Calculate HbA1c from input eAG value. The result will be in percent or mmol/mol depending on the position of the UK (IFCC) checkbox."));

    // Terminology
    lay->addWidget(sectionTitle("Terminology"));
    lay->addWidget(makeText("NGSP: National Glycohemoglobin Standardization Program"));
    lay->addWidget(makeText("IFCC: The International Federation of Clinical Chemistry and Laboratory Medicine"));
    lay->addWidget(makeText("mg/dl: milligrams per deciliter (weight)"));
    lay->addWidget(makeText("mmol/L: millimoles per litre (volume)"));
    lay->addWidget(makeText("eAG: estimated average glucose"));
    lay->addWidget(makeText("ADAG: A1c-derived average glucose"));
    lay->addWidget(makeText("HbA1c: % Glycated hemoglobin (NGSP)"));
    lay->addWidget(makeText("HbA1c: mmol/mol Glycated hemoglobin (IFCC)"));

    // Formulas
    lay->addWidget(sectionTitle("Formulas"));
    lay->addWidget(makeText("This software uses the 2008 ADAG Study Group formulas"));
    lay->addWidget(makeText("Compute eAG: (28.7 × A1c) – 46.7"));
    lay->addWidget(makeText("Compute A1c: (eAG + 46.7) / 28.7"));
    lay->addWidget(makeText("mg/dl to mmol/L: mg/dl / 18"));
    lay->addWidget(makeText("mmol/L to mg/dl: mmol/L × 18"));
    lay->addWidget(makeText("NGSP = (0.09148 × IFCC) + 2.152"));
    lay->addWidget(makeText("IFCC = (10.93 × NGSP) - 23.50 (mmol/mol)"));

    // About HbA1c
    lay->addWidget(sectionTitle("About HbA1c"));
    lay->addWidget(makeText("Glycated hemoglobin (hemoglobin A1c) is a form of hemoglobin that is measured primarily to identify the average plasma glucose concentration over prolonged periods of time."));
    lay->addWidget(makeText("A high A1c represents poor glucose control."));
    lay->addWidget(makeText("However, a good HbA1c still hides a history of recent hypoglycemia, or even spikes of hyperglycemia."));
    lay->addWidget(makeText("Regular blood glucose monitoring is still the best method for the analysis of overall vascular health with respect to blood sugar control."));

    lay->addStretch();
    scroll->setWidget(content);

    QHBoxLayout *btnRow = new QHBoxLayout;
    btnRow->addStretch();
    QPushButton *done = new QPushButton("Done", this);
    done->setDefault(true);
    connect(done, &QPushButton::clicked, this, &QDialog::accept);
    btnRow->addWidget(done);
    outer->addLayout(btnRow);
}
