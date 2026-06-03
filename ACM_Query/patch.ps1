$file = "D:\Cmake\ACM_Query\src\main.cpp"
$txt = [System.IO.File]::ReadAllText($file)

# QSettings include
$txt = $txt.Replace("#include <QButtonGroup>", "#include <QButtonGroup>`n#include <QSettings>")

# Stats in enum
$txt = $txt.Replace("PLAT_NOWCODER };", "PLAT_NOWCODER, PLAT_STATS };")

# Stats tab (emoji + "  Stats")
$old1 = '        makeTab(QString::fromUtf8("\xF0\x9F\x90\xAE"), QString::fromUtf8("\xE7\x89\x9B\xE5\xAE\xA2"), PLAT_NOWCODER);'
$new1 = '        makeTab(QString::fromUtf8("\xF0\x9F\x90\xAE"), QString::fromUtf8("\xE7\x89\x9B\xE5\xAE\xA2"), PLAT_NOWCODER);
        makeTab(QString::fromUtf8("\xF0\x9F\x93\x8A"), "Stats", PLAT_STATS);'
$txt = $txt.Replace($old1, $new1)

# platformBtns[4] -> [5]
$txt = $txt.Replace("platformBtns[4];", "platformBtns[5];")

# for loop 4 -> 5
$txt = $txt.Replace("for (int i = 0; i < 4; i++)", "for (int i = 0; i < 5; i++)")

# switchPlatform - save/load handles
$old2 = '    void switchPlatform(QAbstractButton *btn) {
        g_platform = (Platform)btn->property("plat").toInt();
        updateTabStyles();
        clearResultLayout();
        handleEdit->clear();
        handleEdit->setFocus();
        handleEdit->setEnabled(true);
        queryBtn->setEnabled(true);
        handleEdit->setPlaceholderText(placeholderForPlatform());
        showPlaceholder();
        statusBar()->showMessage(S_READY.get());
    }'
$new2 = '    void switchPlatform(QAbstractButton *btn) {
        g_platform = (Platform)btn->property("plat").toInt();
        updateTabStyles();
        clearResultLayout();
        handleEdit->setEnabled(true);
        queryBtn->setEnabled(true);
        handleEdit->setFocus();
        if (g_platform == PLAT_STATS) {
            handleEdit->setText("");
            handleEdit->setPlaceholderText("Enter handle to query all platforms...");
        } else {
            QString saved = m_settings.value(keyForPlatform(g_platform), "").toString();
            handleEdit->setText(saved);
            handleEdit->setPlaceholderText(placeholderForPlatform());
        }
        showPlaceholder();
        statusBar()->showMessage(S_READY.get());
    }'
$txt = $txt.Replace($old2, $new2)

# doQuery - save on success + handle stats
$old3 = '    void doQuery() {
        QString handle = handleEdit->text().trimmed();
        if (handle.isEmpty()) return;
        showLoading();

        if (g_platform == PLAT_CF)
            showCFResult(queryCodeforces(handle.toStdString()));
        else if (g_platform == PLAT_ATCODER)
            showACResult(queryAtCoder(handle.toStdString()));
        else if (g_platform == PLAT_QOJ)
            showQOJResult(queryQOJ(handle.toStdString()));
        else
            showNCResult(queryNowCoder(handle.toStdString()));
    }'
$new3 = '    void doQuery() {
        QString handle = handleEdit->text().trimmed();
        if (handle.isEmpty()) return;
        showLoading();
        if (g_platform == PLAT_STATS) { queryAllStats(handle); return; }
        if (g_platform == PLAT_CF) {
            auto d = queryCodeforces(handle.toStdString());
            if (d.error.empty()) m_settings.setValue(keyForPlatform(PLAT_CF), handle);
            showCFResult(d);
        } else if (g_platform == PLAT_ATCODER) {
            auto d = queryAtCoder(handle.toStdString());
            if (d.error.empty()) m_settings.setValue(keyForPlatform(PLAT_ATCODER), handle);
            showACResult(d);
        } else if (g_platform == PLAT_QOJ) {
            auto d = queryQOJ(handle.toStdString());
            if (d.error.empty()) m_settings.setValue(keyForPlatform(PLAT_QOJ), handle);
            showQOJResult(d);
        } else {
            auto d = queryNowCoder(handle.toStdString());
            if (d.error.empty()) m_settings.setValue(keyForPlatform(PLAT_NOWCODER), handle);
            showNCResult(d);
        }
    }'
$txt = $txt.Replace($old3, $new3)

# placeholderForPlatform - add keyForPlatform + queryAllStats
$old4 = '    QString placeholderForPlatform() {
        if (g_platform == PLAT_CF) return S_PLACEHOLDER.get();
        if (g_platform == PLAT_ATCODER) return S_EMPTY_AC.get();
        if (g_platform == PLAT_QOJ) return S_EMPTY_QOJ.get();
        return S_EMPTY_NC.get();
    }'
$new4 = '    QString keyForPlatform(Platform p) {
        switch (p) {
            case PLAT_CF: return "handle_cf";
            case PLAT_ATCODER: return "handle_ac";
            case PLAT_QOJ: return "handle_qoj";
            case PLAT_NOWCODER: return "handle_nc";
            default: return "";
        }
    }
    QString placeholderForPlatform() {
        if (g_platform == PLAT_CF) return S_PLACEHOLDER.get();
        if (g_platform == PLAT_ATCODER) return S_EMPTY_AC.get();
        if (g_platform == PLAT_QOJ) return S_EMPTY_QOJ.get();
        return S_EMPTY_NC.get();
    }
    void queryAllStats(const QString& handle) {
        auto h = handle.toStdString();
        auto cf = queryCodeforces(h);
        auto ac = queryAtCoder(h);
        auto qj = queryQOJ(h);
        auto nc = queryNowCoder(h);
        clearResultLayout();
        handleEdit->setEnabled(true); queryBtn->setEnabled(true);
        handleEdit->setFocus(); handleEdit->selectAll();
        int total = 0;
        auto addRow = [&](const QString& icon, const QString& name, int solved, bool ok) {
            auto* row = new QHBoxLayout();
            auto* ico = new QLabel(icon);
            ico->setFont(QFont("Segoe UI Emoji", 14));
            ico->setStyleSheet("background: transparent; border: none;");
            ico->setFixedWidth(30);
            auto* nm = new QLabel(name);
            nm->setFont(QFont("Segoe UI", 12));
            nm->setStyleSheet("color: #2C3E50; background: transparent; border: none;");
            auto* val = new QLabel(ok ? QString::number(solved) : "-");
            val->setFont(QFont("Consolas", 14, QFont::Bold));
            val->setStyleSheet(QString("color: %1; background: transparent; border: none;").arg(ok ? "#27AE60" : "#BDC3C7"));
            row->addWidget(ico); row->addWidget(nm); row->addStretch(); row->addWidget(val);
            resultInnerLayout->addLayout(row);
            if (ok) total += solved;
        };
        resultInnerLayout->addSpacing(4);
        addRow(QString::fromUtf8("\xF0\x9F\x94\xB5"), "Codeforces", cf.solvedCount, cf.error.empty());
        addRow(QString::fromUtf8("\xF0\x9F\x87\xAF\xF0\x9F\x87\xB5"), "AtCoder", ac.solvedCount, ac.error.empty());
        addRow(QString::fromUtf8("\xF0\x9F\x94\xB6"), "QOJ", qj.solvedCount, qj.error.empty());
        addRow(QString::fromUtf8("\xF0\x9F\x90\xAE"), QString::fromUtf8("\xE7\x89\x9B\xE5\xAE\xA2"), nc.solvedCount, nc.error.empty());
        resultInnerLayout->addSpacing(8);
        auto* sep = new QFrame(); sep->setFixedHeight(1);
        sep->setStyleSheet("background: #E0E6ED; border: none;");
        resultInnerLayout->addWidget(sep);
        resultInnerLayout->addSpacing(8);
        auto* totalRow = new QHBoxLayout();
        auto* tl = new QLabel("Total Solved");
        tl->setFont(QFont("Segoe UI", 14, QFont::Bold));
        tl->setStyleSheet("color: #2C3E50; background: transparent; border: none;");
        auto* tv = new QLabel(QString::number(total));
        tv->setFont(QFont("Consolas", 22, QFont::Bold));
        tv->setStyleSheet("color: #4A90D9; background: transparent; border: none;");
        totalRow->addWidget(tl); totalRow->addStretch(); totalRow->addWidget(tv);
        resultInnerLayout->addLayout(totalRow);
        statusBar()->showMessage(S_DONE.get() + "  \xe2\x80\xa2  " + handle);
    }'
$txt = $txt.Replace($old4, $new4)

# m_settings member
$txt = $txt.Replace('QButtonGroup tabGroup;', 'QButtonGroup tabGroup;' + "`n    QSettings m_settings{`"ACM_Query`", `"ACM_Query`"};")

[System.IO.File]::WriteAllText($file, $txt)
Write-Host "All replacements done"
