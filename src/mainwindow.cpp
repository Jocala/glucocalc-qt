#include "mainwindow.h"
#include "helpdialog.h"
#include <QWidget>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QCheckBox>
#include <QLabel>
#include <QDebug>
#include <QFontMetrics>
#include <QButtonGroup>
#include <QMenuBar>
#include <QMenu>
#include <QAction>

MainWindow::MainWindow(QWidget* parent): QMainWindow(parent){
    setWindowIcon(QIcon(":/AppIcon-512.png"));
    QWidget *central = new QWidget(this);
    qDebug() << "MainWindow constructed";
    QVBoxLayout *layout = new QVBoxLayout(central);
    layout->setSpacing(16);
    layout->setContentsMargins(16,16,16,16);

    // Top bar with help button (visible on all platforms, esp. Linux)
    QHBoxLayout *topBar = new QHBoxLayout;
    topBar->addStretch();
    QPushButton *helpBtn = new QPushButton("?", central);
    helpBtn->setObjectName("helpButton");
    helpBtn->setFixedSize(32,32);
    helpBtn->setToolTip("Help");
    QFont hbFont("DejaVu Sans", 18, QFont::Bold);
    helpBtn->setFont(hbFont);
    helpBtn->setStyleSheet("#helpButton { border-radius: 16px; border: 1px solid #CCCCCC; background-color: white; color: #007AFF; padding: 0px; } #helpButton:pressed { background-color: #E5E5EA; } #helpButton:hover { border-color: #007AFF; }");
    connect(helpBtn, &QPushButton::clicked, this, &MainWindow::showHelp);
    topBar->addWidget(helpBtn);
    layout->addLayout(topBar);

    // Also add Help to menubar for Linux/Windows accessibility
    if(QMenuBar *mb = menuBar()){
        QMenu *helpMenu = mb->addMenu("Help");
        QAction *helpAct = helpMenu->addAction("Help...");
        helpAct->setShortcut(QKeySequence::HelpContents);
        connect(helpAct, &QAction::triggered, this, &MainWindow::showHelp);
    }

    // Display card (rounded gray)
    displayCard = new QWidget(central);
    displayCard->setObjectName("displayCard");
    displayCard->setStyleSheet("#displayCard { background-color: #8E8E93; background: #9A9A9A; border-radius: 16px; } QLabel { background: transparent; }");
    QVBoxLayout *cardLay = new QVBoxLayout(displayCard);
    cardLay->setSpacing(10);
    cardLay->setContentsMargins(16,16,16,16);

    // Row: prompt + input
    {
        QHBoxLayout *row = new QHBoxLayout;
        promptLabel = new QLabel("Enter eAG below (mg/dl)", displayCard);
        promptLabel->setStyleSheet("color: #333333; font-size: 13px;");
        inputLabel = new QLabel("0", displayCard);
        QFont f = inputLabel->font(); f.setPointSize(34); f.setBold(true);
        inputLabel->setFont(f);
        inputLabel->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
        inputLabel->setStyleSheet("color: #000000; font-size: 34px; font-weight: bold;");
        row->addWidget(promptLabel);
        row->addStretch();
        row->addWidget(inputLabel);
        cardLay->addLayout(row);
    }
    // Spacer
    {
        QWidget *spacer = new QWidget(displayCard);
        spacer->setFixedHeight(8);
        spacer->setStyleSheet("background: transparent;");
        cardLay->addWidget(spacer);
    }
    // Result row 1
    {
        QHBoxLayout *row = new QHBoxLayout;
        resultLabel1 = new QLabel("NGSP HbA1c (%)", displayCard);
        resultLabel1->setStyleSheet("color: #333333;");
        resultValue1 = new QLabel("0", displayCard);
        QFont f = resultValue1->font(); f.setPointSize(20); f.setWeight(QFont::Medium);
        resultValue1->setFont(f);
        resultValue1->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
        row->addWidget(resultLabel1);
        row->addStretch();
        row->addWidget(resultValue1);
        cardLay->addLayout(row);
    }
    // Result row 2
    {
        QHBoxLayout *row = new QHBoxLayout;
        resultLabel2 = new QLabel("IFCC HbA1c (mmol/mol)", displayCard);
        resultLabel2->setStyleSheet("color: #333333;");
        resultValue2 = new QLabel("0", displayCard);
        QFont f = resultValue2->font(); f.setPointSize(20); f.setWeight(QFont::Medium);
        resultValue2->setFont(f);
        resultValue2->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
        row->addWidget(resultLabel2);
        row->addStretch();
        row->addWidget(resultValue2);
        cardLay->addLayout(row);
    }

    // Geometry is fixed via layout constraint; values use minimum width to prevent jump but not fixed double (Linux HiDPI fix)
    {
        QFontMetrics fm(inputLabel->font());
        const int wInput = fm.horizontalAdvance(QStringLiteral("999.99"));
        const int pad = fm.horizontalAdvance(QStringLiteral("   "));
        inputLabel->setMinimumWidth(wInput + pad);
        QFontMetrics fm2(resultValue1->font());
        const int wRes = fm2.horizontalAdvance(QStringLiteral("999.99"));
        resultValue1->setMinimumWidth(wRes + pad);
        resultValue2->setMinimumWidth(wRes + pad);
    }

    layout->addWidget(displayCard);

    // Calculation segmented control
    {
        QHBoxLayout *calcRow = new QHBoxLayout;
        QLabel *calcLabel = new QLabel("Calculation", central);
        calcLabel->setStyleSheet("color: #000; font-size: 13px;");
        calcRow->addWidget(calcLabel);
        calcRow->addStretch();

        QWidget *seg = new QWidget(central);
        seg->setStyleSheet("background: #E5E5EA; border-radius: 8px;");
        QHBoxLayout *segLay = new QHBoxLayout(seg);
        segLay->setContentsMargins(2,2,2,2);
        segLay->setSpacing(2);

        modeEAGBtn = new QPushButton("Calculate eAG", seg);
        modeA1cBtn = new QPushButton("Calculate HbA1c", seg);
        for(auto b: {modeEAGBtn, modeA1cBtn}){
            b->setCheckable(true);
            b->setFixedHeight(28);
            b->setStyleSheet("QPushButton { border: none; border-radius: 6px; padding: 4px 12px; background: transparent; } QPushButton:checked { background: #007AFF; color: white; }");
        }
        modeGroup = new QButtonGroup(this);
        modeGroup->setExclusive(true);
        modeGroup->addButton(modeEAGBtn, 0);
        modeGroup->addButton(modeA1cBtn, 1);
        modeA1cBtn->setChecked(true); // default matches Swift .a1c
        mode = EAGMode;

        segLay->addWidget(modeEAGBtn);
        segLay->addWidget(modeA1cBtn);
        connect(modeGroup, &QButtonGroup::idClicked, this, &MainWindow::modeChanged);

        calcRow->addWidget(seg);
        layout->addLayout(calcRow);
    }

    // UK toggle
    ukBox = new QCheckBox("UK (IFCC)",central);
    connect(ukBox,&QCheckBox::toggled,this,&MainWindow::toggleUKUS);
    layout->addWidget(ukBox);

    // Keypad: 4 columns as in Swift
    keypad = new QGridLayout;
    keypad->setHorizontalSpacing(10);
    keypad->setVerticalSpacing(10);
    auto makeKey = [&](const QString &label, bool utility=false, bool accent=false, const QString &sysIcon=QString()){
        QPushButton *btn = new QPushButton(label, central);
        btn->setFixedHeight(54);
        btn->setMinimumWidth(60);
        QFont f = btn->font(); f.setPointSize(16); f.setWeight(QFont::Medium);
        btn->setFont(f);
        QString bg = accent ? "#007AFF" : (utility ? "#AEAEB2" : "#E5E5EA");
        QString fg = accent ? "white" : "black";
        btn->setStyleSheet(QString("QPushButton { background: %1; color: %2; border-radius: 14px; border: none; } QPushButton:pressed { background: #C7C7CC; }").arg(bg, fg));
        if(accent) btn->setStyleSheet(QString("QPushButton { background: #007AFF; color: white; border-radius: 14px; border: none; }"));
        return btn;
    };

    // Row 0
    QPushButton *b7 = makeKey("7"); connect(b7, &QPushButton::clicked, this, &MainWindow::digitPressed); keypad->addWidget(b7,0,0);
    QPushButton *b8 = makeKey("8"); connect(b8, &QPushButton::clicked, this, &MainWindow::digitPressed); keypad->addWidget(b8,0,1);
    QPushButton *b9 = makeKey("9"); connect(b9, &QPushButton::clicked, this, &MainWindow::digitPressed); keypad->addWidget(b9,0,2);
    QPushButton *bC = makeKey("C", true); connect(bC, &QPushButton::clicked, this, &MainWindow::clearPressed); keypad->addWidget(bC,0,3);
    // Row 1
    QPushButton *b4 = makeKey("4"); connect(b4, &QPushButton::clicked, this, &MainWindow::digitPressed); keypad->addWidget(b4,1,0);
    QPushButton *b5 = makeKey("5"); connect(b5, &QPushButton::clicked, this, &MainWindow::digitPressed); keypad->addWidget(b5,1,1);
    QPushButton *b6 = makeKey("6"); connect(b6, &QPushButton::clicked, this, &MainWindow::digitPressed); keypad->addWidget(b6,1,2);
    QPushButton *bDel = makeKey("⌫", true); 
    // Fallback glyph for Linux where ⌫ may not render
    if(bDel->fontMetrics().horizontalAdvance("⌫") == 0 || bDel->text().isEmpty()){
        bDel->setText("<-");
        QFont f = bDel->font(); f.setPixelSize(14); bDel->setFont(f);
    }
    connect(bDel, &QPushButton::clicked, this, &MainWindow::backspacePressed); keypad->addWidget(bDel,1,3);
    // Row 2
    QPushButton *b1 = makeKey("1"); connect(b1, &QPushButton::clicked, this, &MainWindow::digitPressed); keypad->addWidget(b1,2,0);
    QPushButton *b2 = makeKey("2"); connect(b2, &QPushButton::clicked, this, &MainWindow::digitPressed); keypad->addWidget(b2,2,1);
    QPushButton *b3 = makeKey("3"); connect(b3, &QPushButton::clicked, this, &MainWindow::digitPressed); keypad->addWidget(b3,2,2);
    QPushButton *bEq = makeKey("=", false, true); connect(bEq, &QPushButton::clicked, this, &MainWindow::equalsPressed); keypad->addWidget(bEq,2,3);
    // Row 3
    QPushButton *b0 = makeKey("0"); connect(b0, &QPushButton::clicked, this, &MainWindow::digitPressed); keypad->addWidget(b0,3,0);
    QPushButton *bDot = makeKey(".", false, false); bDot->setText("."); connect(bDot, &QPushButton::clicked, this, &MainWindow::dotPressed); keypad->addWidget(bDot,3,1);
    // two empty cells
    QWidget *empty1 = new QWidget(central); empty1->setFixedSize(60,54); empty1->setStyleSheet("background: transparent;"); keypad->addWidget(empty1,3,2);
    QWidget *empty2 = new QWidget(central); empty2->setFixedSize(60,54); empty2->setStyleSheet("background: transparent;"); keypad->addWidget(empty2,3,3);

    layout->addLayout(keypad);

    central->setLayout(layout); setCentralWidget(central);
    setWindowTitle("Glucocalc");
    updateResultLabels();
    updateDisplay();
    // Lock window geometry to its sizeHint (includes pre-sized labels + keypad)
    layout->setSizeConstraint(QLayout::SetFixedSize);
    setFixedSize(sizeHint());
    setMinimumWidth(360);
}

void MainWindow::digitPressed(){
    QObject *obj = sender();
    QPushButton *btn = qobject_cast<QPushButton*>(obj);
    if (!btn) return;
    QString txt = btn->text();
    if (txt == ".") dotPressed();
    else if (txt == "=") equalsPressed();
    else {
        if(didCompute){ currentInput.clear(); didCompute=false; }
        // max 3 chars before decimal, 2 after (Swift limits)
        if(currentInput.contains(".")){
            auto parts = currentInput.split(".");
            if(parts.size()>1 && parts.last().size()>=2) return;
        } else {
            if(currentInput.size()>=3) return;
        }
        if(currentInput=="0") currentInput = txt;
        else currentInput += txt;
        updateDisplay();
    }
}

void MainWindow::dotPressed(){
    if(didCompute){ currentInput.clear(); didCompute=false; }
    if(currentInput.isEmpty()){ currentInput="0."; updateDisplay(); return; }
    if(!currentInput.contains('.')) currentInput+='.';
    updateDisplay();
}
void MainWindow::backspacePressed(){ if(!currentInput.isEmpty()) currentInput.chop(1); didCompute=false; updateDisplay(); }
void MainWindow::clearPressed(){ currentInput.clear(); didCompute=false; updateDisplay(); }
void MainWindow::equalsPressed(){
    if(currentInput.isEmpty()) return;
    double val=parseInput();
    if(val <= 0) return;
    if(mode==HbA1cMode){
        double ngsp, ifcc;
        if(uk){ ifcc = val; ngsp = Calculator::mmolMolToPercent(val); } else { ngsp = val; ifcc = Calculator::percentToMmolMol(val); }
        double eAG = Calculator::eAGFromHbA1c(val, uk?Calculator::UK:Calculator::US);
        double eAG_mgdl = uk ? Calculator::mmolLToMgDl(eAG) : eAG;
        double eAG_mmol = uk ? eAG : Calculator::mgDlToMmolL(eAG);
        Q_UNUSED(eAG_mgdl); Q_UNUSED(eAG_mmol);
        // Results are eAG
        double resMgdl = eAG_mgdl;
        double resMmol = eAG_mmol;
        resultValue1->setText(QString("%1").arg(resMgdl,0,'f',2));
        resultValue2->setText(QString("%1").arg(resMmol,0,'f',2));
        (void)ngsp; (void)ifcc;
    }else{
        double eAG_mgdl = uk ? Calculator::mmolLToMgDl(val) : val;
        double eAG_mmol = uk ? val : Calculator::mgDlToMmolL(val);
        Q_UNUSED(eAG_mgdl); Q_UNUSED(eAG_mmol);
        double a1c = Calculator::hbA1cFromEAG(val, uk?Calculator::UK:Calculator::US);
        double ngsp, ifcc;
        if(uk){ ifcc = a1c; ngsp = Calculator::mmolMolToPercent(ifcc); } else { ngsp = a1c; ifcc = Calculator::percentToMmolMol(ngsp); }
        resultValue1->setText(QString("%1").arg(ngsp,0,'f',2));
        resultValue2->setText(QString("%1").arg(ifcc,0,'f',2));
    }
    didCompute = true;
    updateDisplay();
}

double MainWindow::parseInput() const{ bool ok; double v=currentInput.toDouble(&ok); return ok? v:0.0; }

void MainWindow::modeChanged(int idx){
    mode = idx==0?HbA1cMode:EAGMode;
    didCompute = false;
    updateResultLabels();
    updateDisplay();
}
void MainWindow::toggleUKUS(){
    bool newUk = ukBox->isChecked();
    if(newUk == uk) return;
    double val = parseInput();
    if(currentInput.isEmpty() || val <= 0){
        uk = newUk;
        didCompute = false;
        updateResultLabels();
        updateDisplay();
        return;
    }
    double converted;
    if(mode == HbA1cMode){
        converted = newUk ? Calculator::percentToMmolMol(val) : Calculator::mmolMolToPercent(val);
    } else {
        converted = newUk ? Calculator::mgDlToMmolL(val) : Calculator::mmolLToMgDl(val);
    }
    currentInput = QString::number(converted, 'f', 2);
    uk = newUk;
    updateResultLabels();
    if(didCompute){
        equalsPressed();
    } else {
        updateDisplay();
    }
}

void MainWindow::updateResultLabels(){
    if(mode==HbA1cMode){
        promptLabel->setText(uk ? "Enter HbA1c below (mmol/mol)" : "Enter HbA1c below (%)");
        resultLabel1->setText("Calculated eAG (mg/dl)");
        resultLabel2->setText("Calculated eAG (mmol/L)");
    } else {
        promptLabel->setText(uk ? "Enter eAG below (mmol/L)" : "Enter eAG below (mg/dl)");
        resultLabel1->setText("NGSP HbA1c (%)");
        resultLabel2->setText("IFCC HbA1c (mmol/mol)");
    }
}

void MainWindow::updateDisplay(){
    inputLabel->setText(currentInput.isEmpty()?"0":currentInput);
    if(!didCompute){
        resultValue1->setText("0");
        resultValue2->setText("0");
    }
}

void MainWindow::showHelp(){
    HelpDialog dlg(this);
    dlg.exec();
}
