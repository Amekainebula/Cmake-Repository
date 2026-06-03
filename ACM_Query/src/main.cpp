#include <QApplication>
#include <QMainWindow>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStatusBar>
#include <QFont>
#include <QFrame>
#include <QGraphicsDropShadowEffect>
#include <QPixmap>
#include <QButtonGroup>
#include <QSettings>
#include <QVector>
#include <QSettings>
#include <QVector>
#include <windows.h>
#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")
#include "cfclient.h"
#include "atclient.h"
#include "ncclient.h"
#include "qojclient.h"

enum Lang { EN, CN };
static Lang g_lang = EN;

struct Str {
    QString en, cn;
    QString get() const { return g_lang == CN ? cn : en; }
};

static const Str S_TITLE       = {"ACM Query", "ACM 查询"};
static const Str S_PLACEHOLDER = {"Enter handle...", "输入用户名..."};
static const Str S_QUERY       = {"Query", "查询"};
static const Str S_READY       = {"Ready", "就绪"};
static const Str S_QUERYING    = {"Querying...", "查询中..."};
static const Str S_FETCHING    = {"Fetching data for ", "正在获取 "};
static const Str S_DONE        = {"Done", "完成"};
static const Str S_FAILED      = {"Query failed", "查询失败"};
static const Str S_EMPTY_PROMPT= {"Enter a handle and click Query", "输入用户名并点击查询"};
static const Str S_EMPTY_AC    = {"Enter AtCoder handle...", "输入 AtCoder 用户名..."};
static const Str S_EMPTY_NC    = {"Enter NowCoder numeric UID...", "输入牛客用户数字 ID..."};
static const Str S_EMPTY_QOJ   = {"Enter QOJ handle...", "输入 QOJ 用户名..."};
static const Str S_MAX_RATING  = {"Max Rating", "最高分"};
static const Str S_SOLVED      = {"Problems Solved", "刷题数量"};
static const Str S_LANG_BTN    = {"CN", "EN"};

// ===== CF rating =====
static QString cfColor(int r) {
    if (r >= 3000) return "#AA0000"; if (r >= 2600) return "#FF0000";
    if (r >= 2400) return "#FF0000"; if (r >= 2300) return "#FF8C00";
    if (r >= 2100) return "#FF8C00"; if (r >= 1900) return "#AA00AA";
    if (r >= 1600) return "#0000FF"; if (r >= 1400) return "#03A89E";
    if (r >= 1200) return "#008000"; return "#808080";
}
static QString cfTitle(int r) {
    if (g_lang == CN) {
        if (r >= 3000) return QString::fromUtf8("传奇特级大师");
        if (r >= 2600) return QString::fromUtf8("国际特级大师");
        if (r >= 2400) return QString::fromUtf8("特级大师");
        if (r >= 2300) return QString::fromUtf8("国际大师");
        if (r >= 2100) return QString::fromUtf8("大师");
        if (r >= 1900) return QString::fromUtf8("候选大师");
        if (r >= 1600) return QString::fromUtf8("专家");
        if (r >= 1400) return QString::fromUtf8("Specialist");
        if (r >= 1200) return QString::fromUtf8("入门");
        return QString::fromUtf8("新手");
    }
    if (r >= 3000) return "Legendary Grandmaster"; if (r >= 2600) return "International Grandmaster";
    if (r >= 2400) return "Grandmaster"; if (r >= 2300) return "International Master";
    if (r >= 2100) return "Master"; if (r >= 1900) return "Candidate Master";
    if (r >= 1600) return "Expert"; if (r >= 1400) return "Specialist";
    if (r >= 1200) return "Pupil"; return "Newbie";
}

// ===== AC rating =====
static QString acColor(int r) {
    if (r >= 2800) return "#FF0000"; if (r >= 2400) return "#FF8000";
    if (r >= 2000) return "#C0C000"; if (r >= 1600) return "#0000FF";
    if (r >= 1200) return "#00C0C0"; if (r >= 800) return "#008000";
    if (r >= 400) return "#804000"; if (r >= 1) return "#808080"; return "#BBBBBB";
}
static QString acTitle(int r) {
    if (g_lang == CN) {
        if (r >= 2800) return QString::fromUtf8("红"); if (r >= 2400) return QString::fromUtf8("橙");
        if (r >= 2000) return QString::fromUtf8("黄"); if (r >= 1600) return QString::fromUtf8("蓝");
        if (r >= 1200) return QString::fromUtf8("青"); if (r >= 800) return QString::fromUtf8("绿");
        if (r >= 400) return QString::fromUtf8("棕"); if (r >= 1) return QString::fromUtf8("灰"); return "-";
    }
    if (r >= 2800) return "Red"; if (r >= 2400) return "Orange";
    if (r >= 2000) return "Yellow"; if (r >= 1600) return "Blue";
    if (r >= 1200) return "Cyan"; if (r >= 800) return "Green";
    if (r >= 400) return "Brown"; if (r >= 1) return "Gray"; return "Unrated";
}

// ===== NC rating =====
static QString ncColor(int r) {
    if (r >= 2400) return "#FF0000"; if (r >= 2000) return "#FF8000";
    if (r >= 1600) return "#AA00AA"; if (r >= 1200) return "#0000FF";
    if (r >= 800) return "#008000"; if (r >= 1) return "#808080"; return "#BBBBBB";
}
static QString ncTitle(int r) {
    if (g_lang == CN) {
        if (r >= 2400) return QString::fromUtf8("红"); if (r >= 2000) return QString::fromUtf8("橙");
        if (r >= 1600) return QString::fromUtf8("紫"); if (r >= 1200) return QString::fromUtf8("蓝");
        if (r >= 800) return QString::fromUtf8("绿"); if (r >= 1) return QString::fromUtf8("灰"); return "-";
    }
    if (r >= 2400) return "Red"; if (r >= 2000) return "Orange";
    if (r >= 1600) return "Purple"; if (r >= 1200) return "Blue";
    if (r >= 800) return "Green"; if (r >= 1) return "Gray"; return "Unrated";
}

enum Platform { PLAT_CF, PLAT_ATCODER, PLAT_QOJ, PLAT_NOWCODER, PLAT_STATS };
static Platform g_platform = PLAT_CF;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr) : QMainWindow(parent) {
        setWindowTitle(S_TITLE.get());
        setFixedSize(760, 560);
        setStyleSheet("QMainWindow { background: #F5F7FA; }");

        auto *central = new QWidget(this);
        central->setStyleSheet("background: #F5F7FA;");
        setCentralWidget(central);

        auto *mainLayout = new QVBoxLayout(central);
        mainLayout->setContentsMargins(0, 0, 0, 0);
        mainLayout->setSpacing(0);

        // Accent bar
        auto *accent = new QFrame();
        accent->setFixedHeight(4);
        accent->setStyleSheet("QFrame { background: qlineargradient(x1:0,y1:0,x2:1,y2:0, stop:0 #4A90D9, stop:1 #357ABD); border: none; }");
        mainLayout->addWidget(accent);

        // Tab bar
        auto *tabBar = new QFrame();
        tabBar->setFixedHeight(52);
        tabBar->setStyleSheet("QFrame#tabBar { background: #F5F7FA; border: none; }");
        tabBar->setObjectName("tabBar");

        auto *tabLayout = new QHBoxLayout(tabBar);
        tabLayout->setContentsMargins(20, 8, 20, 8);
        tabLayout->setSpacing(10);

        auto makeTab = [&](const QString &icon, const QString &name, Platform plat) {
            auto *btn = new QPushButton(icon + "  " + name);
            btn->setCheckable(true);
            btn->setFont(QFont("Segoe UI", 11));
            btn->setCursor(Qt::PointingHandCursor);
            btn->setFixedHeight(36);
            btn->setProperty("plat", (int)plat);
            platformBtns[(int)plat] = btn;
            tabGroup.addButton(btn);
            tabLayout->addWidget(btn);
        };
        makeTab("\xF0\x9F\x93\x8A", "Stats", PLAT_STATS);
        makeTab("\xF0\x9F\x94\xB5", "Codeforces", PLAT_CF);
        makeTab("\xF0\x9F\x9F\xA2", "AtCoder", PLAT_ATCODER);
        makeTab("\xF0\x9F\x94\xB6", "QOJ", PLAT_QOJ);
        makeTab("\xF0\x9F\x90\xAE", "NowCoder", PLAT_NOWCODER);
        tabLayout->addStretch();

        langBtn = new QPushButton(S_LANG_BTN.get());
        langBtn->setFixedSize(40, 28);
        langBtn->setFont(QFont("Segoe UI", 9, QFont::Bold));
        langBtn->setCursor(Qt::PointingHandCursor);
        langBtn->setStyleSheet(
            "QPushButton { background: #E8ECF1; color: #7F8C8D; border-radius: 8px; border: 1px solid #D5DCE4; }"
            "QPushButton:hover { background: #4A90D9; color: white; border-color: #4A90D9; }");
        tabLayout->addWidget(langBtn);
        mainLayout->addWidget(tabBar);

        updateTabStyles();

        // Body
        auto *body = new QWidget();
        body->setStyleSheet("background: #F5F7FA;");
        auto *bodyLayout = new QVBoxLayout(body);
        bodyLayout->setContentsMargins(24, 12, 24, 16);
        bodyLayout->setSpacing(14);

        searchCard = new QFrame();
        searchCard->setStyleSheet("QFrame#searchCard { background: white; border-radius: 10px; border: 1px solid #E0E6ED; }");
        searchCard->setObjectName("searchCard");
        addShadow(searchCard);

        auto *searchLayout = new QHBoxLayout(searchCard);
        searchLayout->setContentsMargins(16, 14, 16, 14);
        searchLayout->setSpacing(12);

        handleEdit = new QLineEdit();
        handleEdit->setPlaceholderText(S_PLACEHOLDER.get());
        handleEdit->setFont(QFont("Segoe UI", 13));
        handleEdit->setStyleSheet("QLineEdit { border: none; background: transparent; color: #2C3E50; padding: 4px 0; }");
        handleEdit->setMinimumHeight(36);

        queryBtn = new QPushButton(S_QUERY.get());
        queryBtn->setFont(QFont("Segoe UI", 12, QFont::Bold));
        queryBtn->setFixedSize(90, 40);
        queryBtn->setCursor(Qt::PointingHandCursor);
        queryBtn->setDefault(true);
        queryBtn->setStyleSheet(
            "QPushButton { background-color: #4A90D9; color: white; border-radius: 8px; border: none; }"
            "QPushButton:hover  { background-color: #357ABD; }"
            "QPushButton:pressed{ background-color: #2C5F8A; }"
            "QPushButton:disabled{ background-color: #B0C4DE; }");

        searchLayout->addWidget(handleEdit, 1);
        searchLayout->addWidget(queryBtn);
        bodyLayout->addWidget(searchCard);

        // Stats card (multi-input, hidden by default)
        statsCard = new QFrame();
        statsCard->setStyleSheet("QFrame#statsCard { background: white; border-radius: 10px; border: 1px solid #E0E6ED; }");
        statsCard->setObjectName("statsCard");
        addShadow(statsCard);

        auto *statsLayout = new QVBoxLayout(statsCard);
        statsLayout->setContentsMargins(16, 12, 16, 12);
        statsLayout->setSpacing(8);

        auto makeStatsInput = [&](const QString& icon, const QString& name, const QString& ph, QLineEdit*& edit) {
            auto *row = new QHBoxLayout();
            auto *ico = new QLabel(icon);
            ico->setFont(QFont("Segoe UI Emoji", 14));
            ico->setStyleSheet("background: transparent; border: none;");
            ico->setFixedWidth(28);
            auto *nm = new QLabel(name);
            nm->setFont(QFont("Segoe UI", 11));
            nm->setStyleSheet("color: #7F8C8D; background: transparent; border: none;");
            nm->setFixedWidth(90);
            edit = new QLineEdit();
            edit->setPlaceholderText(ph);
            edit->setFont(QFont("Segoe UI", 11));
            edit->setStyleSheet("QLineEdit { border: 1px solid #E0E6ED; border-radius: 6px; background: #FAFBFC; color: #2C3E50; padding: 5px 10px; }"
                                "QLineEdit:focus { border-color: #4A90D9; background: white; }");
            edit->setMinimumHeight(32);
            row->addWidget(ico); row->addWidget(nm); row->addWidget(edit, 1);
            statsLayout->addLayout(row);
        };

        makeStatsInput("\xF0\x9F\x94\xB5", "Codeforces", "CF handle...", cfStatsEdit);
        makeStatsInput("\xF0\x9F\x9F\xA2", "AtCoder", "AC handle...", acStatsEdit);
        makeStatsInput("\xF0\x9F\x94\xB6", "QOJ", "QOJ handle...", qojStatsEdit);
        makeStatsInput("\xF0\x9F\x90\xAE", "NowCoder", "NC uid...", ncStatsEdit);

        statsQueryBtn = new QPushButton("Query All");
        statsQueryBtn->setFont(QFont("Segoe UI", 12, QFont::Bold));
        statsQueryBtn->setCursor(Qt::PointingHandCursor);
        statsQueryBtn->setFixedHeight(40);
        statsQueryBtn->setStyleSheet(
            "QPushButton { background-color: #4A90D9; color: white; border-radius: 8px; border: none; }"
            "QPushButton:hover  { background-color: #357ABD; }"
            "QPushButton:pressed{ background-color: #2C5F8A; }");
        statsLayout->addWidget(statsQueryBtn);

        bodyLayout->addWidget(statsCard);
        statsCard->hide();

        resultCard = new QFrame();
        resultCard->setStyleSheet("QFrame#resultCard { background: white; border-radius: 10px; border: 1px solid #E0E6ED; }");
        resultCard->setObjectName("resultCard");
        addShadow(resultCard);

        resultInnerLayout = new QVBoxLayout(resultCard);
        resultInnerLayout->setContentsMargins(20, 16, 20, 16);
        resultInnerLayout->setSpacing(0);

        showPlaceholder();
        bodyLayout->addWidget(resultCard, 1);
        mainLayout->addWidget(body, 1);

        statusBar()->setStyleSheet("QStatusBar { background: #E8ECF1; color: #7F8C8D; font-size: 11px; border-top: 1px solid #D5DCE4; }");
        statusBar()->showMessage(S_READY.get());

        connect(queryBtn, &QPushButton::clicked, this, &MainWindow::doQuery);
        connect(handleEdit, &QLineEdit::returnPressed, this, &MainWindow::doQuery);
        connect(langBtn, &QPushButton::clicked, this, &MainWindow::toggleLanguage);
        connect(&tabGroup, QOverload<QAbstractButton*>::of(&QButtonGroup::buttonClicked), this, &MainWindow::switchPlatform);
        connect(statsQueryBtn, &QPushButton::clicked, this, &MainWindow::queryStatsWithInputs);

        platformBtns[PLAT_CF]->setChecked(true);
    }

private slots:
    void toggleLanguage() {
        g_lang = (g_lang == EN) ? CN : EN;
        langBtn->setText(S_LANG_BTN.get());
        setWindowTitle(S_TITLE.get());
        handleEdit->setPlaceholderText(placeholderForPlatform());
        queryBtn->setText(S_QUERY.get());
        statusBar()->showMessage(S_READY.get());
        showPlaceholder();
    }

    void switchPlatform(QAbstractButton *btn) {
        g_platform = (Platform)btn->property("plat").toInt();
        updateTabStyles();
        clearResultLayout();
        handleEdit->setEnabled(true);
        queryBtn->setEnabled(true);
        handleEdit->setFocus();
        if (g_platform == PLAT_STATS) {
            searchCard->hide();
            statsCard->show();
            queryBtn->hide();
        } else {
            searchCard->show();
            statsCard->hide();
            queryBtn->show();
            QString saved = m_settings.value(keyForPlatform(g_platform), "").toString();
            handleEdit->setText(saved);
            handleEdit->setPlaceholderText(placeholderForPlatform());
        }
        showPlaceholder();
        statusBar()->showMessage(S_READY.get());
    }

    void doQuery() {
        QString handle = handleEdit->text().trimmed();
        if (handle.isEmpty()) return;
        showLoading();
        if (g_platform == PLAT_STATS) { queryStatsWithInputs(); return; }
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
    }

private:
    void addShadow(QWidget *w) {
        auto *s = new QGraphicsDropShadowEffect(this);
        s->setBlurRadius(20); s->setOffset(0, 2); s->setColor(QColor(0, 0, 0, 25));
        w->setGraphicsEffect(s);
    }
    QString keyForPlatform(Platform p) {
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
    void queryStatsWithInputs() {
        statsQueryBtn->setEnabled(false);
        clearResultLayout();
        auto* loading = new QLabel("Querying all platforms...");
        loading->setFont(QFont("Segoe UI", 12));
        loading->setStyleSheet("color: #4A90D9; background: transparent; border: none;");
        loading->setAlignment(Qt::AlignCenter);
        loading->setMinimumHeight(120);
        resultInnerLayout->addWidget(loading);
        repaint();

        struct Row { QString icon; QString name; int solved; bool ok; };
        QVector<Row> rows;
        int total = 0;

        auto query = [&](QLineEdit* ed, const QString& icon, const QString& name, auto fn) {
            QString h = ed->text().trimmed();
            if (h.isEmpty()) return;
            auto d = fn(h.toStdString());
            rows.push_back({icon, name, d.solvedCount, d.error.empty()});
            if (d.error.empty()) total += d.solvedCount;
        };

        query(cfStatsEdit, "\xF0\x9F\x94\xB5", "Codeforces", queryCodeforces);
        query(acStatsEdit, "\xF0\x9F\x9F\xA2", "AtCoder", queryAtCoder);
        query(qojStatsEdit, "\xF0\x9F\x94\xB6", "QOJ", queryQOJ);
        query(ncStatsEdit, "\xF0\x9F\x90\xAE", "NowCoder", queryNowCoder);

        clearResultLayout();
        statsQueryBtn->setEnabled(true);

        if (rows.isEmpty()) {
            auto* p = new QLabel("Fill at least one handle and click Query All.");
            p->setFont(QFont("Segoe UI", 11));
            p->setStyleSheet("color: #AAB7C4; background: transparent; border: none;");
            p->setAlignment(Qt::AlignCenter); p->setMinimumHeight(160);
            resultInnerLayout->addWidget(p);
            return;
        }

        resultInnerLayout->addSpacing(4);
        for (auto& r : rows) {
            auto* row = new QHBoxLayout();
            auto* ico = new QLabel();
            ico->setFont(QFont("Segoe UI Emoji", 14));
            ico->setStyleSheet("background: transparent; border: none;");
            ico->setFixedWidth(30);
            auto* nm = new QLabel(r.name);
            nm->setFont(QFont("Segoe UI", 12));
            nm->setStyleSheet("color: #2C3E50; background: transparent; border: none;");
            auto* val = new QLabel(r.ok ? QString::number(r.solved) : "-");
            val->setFont(QFont("Consolas", 14, QFont::Bold));
            val->setStyleSheet(QString("color: %1; background: transparent; border: none;").arg(r.ok ? "#27AE60" : "#BDC3C7"));
            row->addWidget(ico); row->addWidget(nm); row->addStretch(); row->addWidget(val);
            resultInnerLayout->addLayout(row);
        }

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

        statusBar()->showMessage(S_DONE.get());
    }

    
    void updateTabStyles() {
        for (int i = 0; i < 5; i++) {
            bool sel = (i == (int)g_platform);
            platformBtns[i]->setStyleSheet(QString(
                "QPushButton { background-color: %1; color: %2; border-radius: 18px; border: %3; padding: 0 20px; font-weight: %4; }"
                "QPushButton:hover { background-color: %5; color: %6; }"
            ).arg(sel && (int)g_platform == PLAT_STATS ? "#16A085" : sel ? "#4A90D9" : "transparent")
             .arg(sel ? "white" : "#7F8C8D")
             .arg(sel ? "none" : "1px solid #D5DCE4")
             .arg(sel ? "bold" : "normal")
             .arg(sel ? "#357ABD" : "rgba(74,144,217,0.10)")
             .arg(sel ? "white" : "#4A90D9"));
            if (sel) {
                auto *sh = new QGraphicsDropShadowEffect(this);
                sh->setBlurRadius(12); sh->setOffset(0, 2); sh->setColor(QColor(74, 144, 217, 60));
                platformBtns[i]->setGraphicsEffect(sh);
            } else platformBtns[i]->setGraphicsEffect(nullptr);
        }
    }
    void clearLayoutRecursive(QLayout *l) {
        QLayoutItem *item;
        while ((item = l->takeAt(0)) != nullptr) {
            if (item->widget()) delete item->widget();
            else if (item->layout()) clearLayoutRecursive(item->layout());
            delete item;
        }
    }
    void clearResultLayout() { clearLayoutRecursive(resultInnerLayout); }
    void showPlaceholder() {
        clearResultLayout();
        auto *p = new QLabel(S_EMPTY_PROMPT.get());
        p->setFont(QFont("Segoe UI", 11));
        p->setStyleSheet("color: #AAB7C4; background: transparent; border: none;");
        p->setAlignment(Qt::AlignCenter); p->setMinimumHeight(160);
        resultInnerLayout->addWidget(p);
    }
    void showLoading() {
        clearResultLayout();
        handleEdit->setEnabled(false); queryBtn->setEnabled(false);
        auto *l = new QLabel(S_QUERYING.get());
        l->setFont(QFont("Segoe UI", 13));
        l->setStyleSheet("color: #4A90D9; background: transparent; border: none;");
        l->setAlignment(Qt::AlignCenter); l->setMinimumHeight(160);
        resultInnerLayout->addWidget(l);
        statusBar()->showMessage(S_FETCHING.get() + handleEdit->text() + "...");
        repaint();
    }
    void showError(const std::string &err) {
        auto *e = new QLabel(QString::fromUtf8("\xE2\x9A\xA0  %1").arg(QString::fromStdString(err)));
        e->setFont(QFont("Segoe UI", 12));
        e->setStyleSheet("color: #E74C3C; background: transparent; border: none; padding: 10px;");
        e->setAlignment(Qt::AlignCenter); e->setWordWrap(true); e->setMinimumHeight(160);
        resultInnerLayout->addWidget(e);
        statusBar()->showMessage(S_FAILED.get());
    }
    void addHandleRankRow(const std::string &handle, const std::string &rank, const QString &color) {
        auto *row = new QHBoxLayout();
        auto *hl = new QLabel(QString::fromStdString(handle));
        hl->setFont(QFont("Segoe UI", 16, QFont::Bold));
        hl->setStyleSheet("color: #2C3E50; background: transparent; border: none;");
        row->addWidget(hl); row->addStretch();
        if (!rank.empty()) {
            auto *rb = new QLabel(QString::fromStdString(rank));
            rb->setFont(QFont("Segoe UI", 9, QFont::Bold));
            rb->setStyleSheet(QString("color: white; background-color: %1; border-radius: 4px; padding: 3px 10px; border: none;").arg(color));
            row->addWidget(rb);
        }
        resultInnerLayout->addLayout(row); resultInnerLayout->addSpacing(8);
    }
    void addBigRating(int rating, const QString &color, const QString &title) {
        auto *big = new QLabel(QString::number(rating));
        big->setFont(QFont("Consolas", 40, QFont::Bold));
        big->setStyleSheet(QString("color: %1; background: transparent; border: none;").arg(color));
        big->setAlignment(Qt::AlignCenter);
        resultInnerLayout->addWidget(big);
        if (!title.isEmpty()) {
            auto *rtl = new QLabel(title);
            rtl->setFont(QFont("Segoe UI", 11));
            rtl->setStyleSheet(QString("color: %1; background: transparent; border: none;").arg(color));
            rtl->setAlignment(Qt::AlignCenter);
            resultInnerLayout->addWidget(rtl);
        }
    }
    void addSpacer(int h) { resultInnerLayout->addSpacing(h); }
    void addStat(const QString &icon, const QString &label, const QString &value) {
        auto *row = new QHBoxLayout();
        auto *ico = new QLabel(icon);
        ico->setFont(QFont("Segoe UI Emoji", 13)); ico->setStyleSheet("background: transparent; border: none;"); ico->setFixedWidth(28);
        auto *lbl = new QLabel(label);
        lbl->setFont(QFont("Segoe UI", 11)); lbl->setStyleSheet("color: #7F8C8D; background: transparent; border: none;");
        auto *val = new QLabel(value);
        val->setFont(QFont("Segoe UI", 13, QFont::Bold)); val->setStyleSheet("color: #2C3E50; background: transparent; border: none;");
        row->addWidget(ico); row->addWidget(lbl); row->addStretch(); row->addWidget(val);
        resultInnerLayout->addLayout(row);
    }

    void showCFResult(const CFUserData &data) {
        clearResultLayout();
        handleEdit->setEnabled(true); queryBtn->setEnabled(true); handleEdit->setFocus(); handleEdit->selectAll();
        if (!data.error.empty()) { showError(data.error); return; }
        QString rc = cfColor(data.rating);
        addHandleRankRow(data.handle, data.rank, rc);
        addBigRating(data.rating, rc, cfTitle(data.rating));
        addSpacer(12);
        addStat("\xF0\x9F\x93\x88", S_MAX_RATING.get(), QString::number(data.maxRating));
        addStat("\xE2\x9C\x85", S_SOLVED.get(), QString::number(data.solvedCount));
        statusBar()->showMessage(S_DONE.get() + "  \xe2\x80\xa2  " + QString::fromStdString(data.handle));
    }

    void showACResult(const ACUserData &data) {
        clearResultLayout();
        handleEdit->setEnabled(true); queryBtn->setEnabled(true); handleEdit->setFocus(); handleEdit->selectAll();
        if (!data.error.empty()) { showError(data.error); return; }
        QString rc = acColor(data.rating);
        addHandleRankRow(data.handle, data.rank, rc);
        addBigRating(data.rating, rc, acTitle(data.rating));
        addSpacer(12);
        addStat("\xF0\x9F\x93\x88", S_MAX_RATING.get(), QString::number(data.maxRating));
        statusBar()->showMessage(S_DONE.get() + "  \xe2\x80\xa2  " + QString::fromStdString(data.handle));
    }

    void showQOJResult(const QOJUserData &data) {
        clearResultLayout();
        handleEdit->setEnabled(true); queryBtn->setEnabled(true); handleEdit->setFocus(); handleEdit->selectAll();
        if (!data.error.empty()) { showError(data.error); return; }
        addHandleRankRow(data.handle, "QOJ", "#4A90D9");
        addSpacer(16);
        addStat("\xE2\x9C\x85", S_SOLVED.get(), QString::number(data.solvedCount));
        statusBar()->showMessage(S_DONE.get() + "  \xe2\x80\xa2  " + QString::fromStdString(data.handle));
    }

    void showNCResult(const NCUserData &data) {
        clearResultLayout();
        handleEdit->setEnabled(true); queryBtn->setEnabled(true); handleEdit->setFocus(); handleEdit->selectAll();
        if (!data.error.empty()) { showError(data.error); return; }
        if (data.rating > 0) {
            QString rc = ncColor(data.rating);
            addHandleRankRow(data.handle, data.rank, rc);
            addBigRating(data.rating, rc, ncTitle(data.rating));
            addSpacer(12);
            addStat("\xF0\x9F\x93\x88", S_MAX_RATING.get(), QString::number(data.maxRating));
        } else {
            addHandleRankRow(data.handle, "", "#E67E22");
        }
        addStat("\xE2\x9C\x85", S_SOLVED.get(), QString::number(data.solvedCount));
        statusBar()->showMessage(S_DONE.get() + "  \xe2\x80\xa2  " + QString::fromStdString(data.handle));
    }

    QLineEdit *handleEdit;
    QLineEdit *cfStatsEdit, *acStatsEdit, *qojStatsEdit, *ncStatsEdit;
    QPushButton *queryBtn, *statsQueryBtn, *langBtn;
    QFrame *searchCard, *statsCard, *resultCard;
    QVBoxLayout *resultInnerLayout;
    QPushButton *platformBtns[5];
    QButtonGroup tabGroup;
    QSettings m_settings{"ACM_Query", "ACM_Query"};

};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setStyle("Fusion");
    app.setFont(QFont("Segoe UI", 10));

    MainWindow w;
    w.show();

    HWND hwnd = (HWND)w.winId();
    DWORD dark = TRUE;
    DwmSetWindowAttribute(hwnd, 20, &dark, sizeof(dark));
    COLORREF cc = RGB(0x2C, 0x5F, 0x8A);
    DwmSetWindowAttribute(hwnd, 35, &cc, sizeof(cc));
    DwmSetWindowAttribute(hwnd, 34, &cc, sizeof(cc));

    return app.exec();
}

#include "main.moc"

