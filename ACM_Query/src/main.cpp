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
#include <QButtonGroup>
#include <QMap>
#include <QPropertyAnimation>
#include "cfclient.h"

// ====================== Language strings ======================
enum Lang { EN, CN };
static Lang g_lang = EN;

struct Str {
    QString en, cn;
    QString get() const { return g_lang == CN ? cn : en; }
};

static const Str S_TITLE       = {"ACM Query", "ACM 查询"};
static const Str S_SUBTITLE    = {"Competitive Programming Profile", "竞赛选手数据查询"};
static const Str S_PLACEHOLDER = {"Enter handle...", "输入用户名..."};
static const Str S_QUERY       = {"Query", "查询"};
static const Str S_READY       = {"Ready", "就绪"};
static const Str S_QUERYING    = {"Querying...", "查询中..."};
static const Str S_FETCHING    = {"Fetching data for ", "正在获取 "};
static const Str S_DONE        = {"Done", "完成"};
static const Str S_FAILED      = {"Query failed", "查询失败"};
static const Str S_EMPTY_PROMPT= {"Enter a handle and click Query", "输入用户名并点击查询"};
static const Str S_MAX_RATING  = {"Max Rating", "最高分"};
static const Str S_SOLVED      = {"Problems Solved", "刷题数量"};
static const Str S_COMING_SOON = {"Coming Soon", "即将上线"};
static const Str S_COMING_DESC = {"This platform will be available soon.", "该平台即将支持，敬请期待。"};
static const Str S_LANG_BTN    = {"CN", "EN"};

// ====================== Codeforces rating ======================
static QString ratingColor(int rating) {
    if (rating >= 3000) return "#AA0000";
    if (rating >= 2600) return "#FF0000";
    if (rating >= 2400) return "#FF0000";
    if (rating >= 2300) return "#FF8C00";
    if (rating >= 2100) return "#FF8C00";
    if (rating >= 1900) return "#AA00AA";
    if (rating >= 1600) return "#0000FF";
    if (rating >= 1400) return "#03A89E";
    if (rating >= 1200) return "#008000";
    return "#808080";
}

static QString ratingTitle(int rating) {
    if (g_lang == CN) {
        if (rating >= 3000) return QString::fromUtf8("传奇特级大师");
        if (rating >= 2600) return QString::fromUtf8("国际特级大师");
        if (rating >= 2400) return QString::fromUtf8("特级大师");
        if (rating >= 2300) return QString::fromUtf8("国际大师");
        if (rating >= 2100) return QString::fromUtf8("大师");
        if (rating >= 1900) return QString::fromUtf8("候选大师");
        if (rating >= 1600) return QString::fromUtf8("专家");
        if (rating >= 1400) return QString::fromUtf8(" Specialist");
        if (rating >= 1200) return QString::fromUtf8("入门");
        return QString::fromUtf8("新手");
    }
    if (rating >= 3000) return "Legendary Grandmaster";
    if (rating >= 2600) return "International Grandmaster";
    if (rating >= 2400) return "Grandmaster";
    if (rating >= 2300) return "International Master";
    if (rating >= 2100) return "Master";
    if (rating >= 1900) return "Candidate Master";
    if (rating >= 1600) return "Expert";
    if (rating >= 1400) return "Specialist";
    if (rating >= 1200) return "Pupil";
    return "Newbie";
}

// ====================== Platform ======================
enum Platform { PLAT_CF, PLAT_ATCODER, PLAT_NOWCODER };
static Platform g_platform = PLAT_CF;

// ====================== MainWindow ======================
class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr) : QMainWindow(parent) {
        setWindowTitle("ACM Query");
        setFixedSize(520, 510);
        setStyleSheet("QMainWindow { background: #F5F7FA; }");

        auto *central = new QWidget(this);
        central->setStyleSheet("background: #F5F7FA;");
        setCentralWidget(central);

        auto *mainLayout = new QVBoxLayout(central);
        mainLayout->setContentsMargins(0, 0, 0, 0);
        mainLayout->setSpacing(0);

        // ===== Header =====
        auto *header = new QFrame();
        header->setFixedHeight(80);
        header->setStyleSheet(
            "QFrame#header {"
            "  background: qlineargradient(x1:0,y1:0,x2:1,y2:1,"
            "    stop:0 #4A90D9, stop:1 #357ABD);"
            "  border-bottom: 3px solid #2C5F8A;"
            "}"
        );
        header->setObjectName("header");

        auto *headerHLayout = new QHBoxLayout(header);
        headerHLayout->setContentsMargins(20, 8, 12, 8);

        auto *titleBox = new QVBoxLayout();
        titleLabel = new QLabel(S_TITLE.get());
        titleLabel->setFont(QFont("Segoe UI", 20, QFont::Bold));
        titleLabel->setStyleSheet("color: white; background: transparent;");
        subtitleLabel = new QLabel(S_SUBTITLE.get());
        subtitleLabel->setFont(QFont("Segoe UI", 9));
        subtitleLabel->setStyleSheet("color: rgba(255,255,255,0.75); background: transparent;");
        titleBox->addWidget(titleLabel);
        titleBox->addWidget(subtitleLabel);

        // Language toggle
        langBtn = new QPushButton(S_LANG_BTN.get());
        langBtn->setFixedSize(44, 28);
        langBtn->setFont(QFont("Segoe UI", 9, QFont::Bold));
        langBtn->setCursor(Qt::PointingHandCursor);
        langBtn->setStyleSheet(
            "QPushButton {"
            "  background: rgba(255,255,255,0.2);"
            "  color: white;"
            "  border-radius: 6px;"
            "  border: 1px solid rgba(255,255,255,0.3);"
            "}"
            "QPushButton:hover { background: rgba(255,255,255,0.3); }"
        );

        headerHLayout->addLayout(titleBox);
        headerHLayout->addStretch();
        headerHLayout->addWidget(langBtn, 0, Qt::AlignTop);
        mainLayout->addWidget(header);

        // ===== Platform tabs =====
        auto *tabBar = new QFrame();
        tabBar->setFixedHeight(44);
        tabBar->setStyleSheet(
            "QFrame#tabBar {"
            "  background: white;"
            "  border-bottom: 1px solid #E0E6ED;"
            "}"
        );
        tabBar->setObjectName("tabBar");

        auto *tabLayout = new QHBoxLayout(tabBar);
        tabLayout->setContentsMargins(20, 6, 20, 6);
        tabLayout->setSpacing(8);

        auto makeTab = [&](const QString &name, Platform plat) {
            auto *btn = new QPushButton(name);
            btn->setCheckable(true);
            btn->setFont(QFont("Segoe UI", 11));
            btn->setCursor(Qt::PointingHandCursor);
            btn->setFixedHeight(32);
            btn->setProperty("plat", (int)plat);
            platformBtns[(int)plat] = btn;
            tabGroup.addButton(btn);
            tabLayout->addWidget(btn);
        };

        makeTab("Codeforces", PLAT_CF);
        makeTab("AtCoder", PLAT_ATCODER);
        makeTab(QString::fromUtf8("\xE7\x89\x9B\xE5\xAE\xA2"), PLAT_NOWCODER); // 牛客

        tabLayout->addStretch();
        mainLayout->addWidget(tabBar);

        updateTabStyles();

        // ===== Body =====
        auto *body = new QWidget();
        body->setStyleSheet("background: #F5F7FA;");
        auto *bodyLayout = new QVBoxLayout(body);
        bodyLayout->setContentsMargins(24, 16, 24, 16);
        bodyLayout->setSpacing(14);

        // -- Search card --
        searchCard = new QFrame();
        searchCard->setStyleSheet(
            "QFrame#searchCard {"
            "  background: white;"
            "  border-radius: 10px;"
            "  border: 1px solid #E0E6ED;"
            "}"
        );
        searchCard->setObjectName("searchCard");
        addShadow(searchCard);

        auto *searchLayout = new QHBoxLayout(searchCard);
        searchLayout->setContentsMargins(16, 14, 16, 14);
        searchLayout->setSpacing(12);

        handleEdit = new QLineEdit();
        handleEdit->setPlaceholderText(S_PLACEHOLDER.get());
        handleEdit->setFont(QFont("Segoe UI", 13));
        handleEdit->setStyleSheet(
            "QLineEdit {"
            "  border: none; background: transparent;"
            "  color: #2C3E50; padding: 4px 0;"
            "}"
        );
        handleEdit->setMinimumHeight(36);

        queryBtn = new QPushButton(S_QUERY.get());
        queryBtn->setFont(QFont("Segoe UI", 12, QFont::Bold));
        queryBtn->setFixedSize(90, 40);
        queryBtn->setCursor(Qt::PointingHandCursor);
        queryBtn->setDefault(true);
        queryBtn->setStyleSheet(
            "QPushButton {"
            "  background-color: #4A90D9; color: white;"
            "  border-radius: 8px; border: none;"
            "}"
            "QPushButton:hover  { background-color: #357ABD; }"
            "QPushButton:pressed{ background-color: #2C5F8A; }"
            "QPushButton:disabled{ background-color: #B0C4DE; }"
        );

        searchLayout->addWidget(handleEdit, 1);
        searchLayout->addWidget(queryBtn);
        bodyLayout->addWidget(searchCard);

        // -- Result card --
        resultCard = new QFrame();
        resultCard->setStyleSheet(
            "QFrame#resultCard {"
            "  background: white; border-radius: 10px;"
            "  border: 1px solid #E0E6ED;"
            "}"
        );
        resultCard->setObjectName("resultCard");
        addShadow(resultCard);

        resultInnerLayout = new QVBoxLayout(resultCard);
        resultInnerLayout->setContentsMargins(20, 16, 20, 16);
        resultInnerLayout->setSpacing(0);

        showPlaceholder();
        bodyLayout->addWidget(resultCard, 1);
        bodyLayout->addStretch();
        mainLayout->addWidget(body, 1);

        // ===== Status bar =====
        statusBar()->setStyleSheet(
            "QStatusBar {"
            "  background: #E8ECF1; color: #7F8C8D;"
            "  font-size: 11px; border-top: 1px solid #D5DCE4;"
            "}"
        );
        statusBar()->showMessage(S_READY.get());

        // ===== Connections =====
        connect(queryBtn, &QPushButton::clicked, this, &MainWindow::doQuery);
        connect(handleEdit, &QLineEdit::returnPressed, this, &MainWindow::doQuery);
        connect(langBtn, &QPushButton::clicked, this, &MainWindow::toggleLanguage);
        connect(&tabGroup, QOverload<QAbstractButton*>::of(&QButtonGroup::buttonClicked),
                this, &MainWindow::switchPlatform);

        // Select CF by default
        platformBtns[PLAT_CF]->setChecked(true);
        switchPlatform(platformBtns[PLAT_CF]);
    }

private slots:
    void toggleLanguage() {
        g_lang = (g_lang == EN) ? CN : EN;
        langBtn->setText(S_LANG_BTN.get());
        titleLabel->setText(S_TITLE.get());
        subtitleLabel->setText(S_SUBTITLE.get());
        handleEdit->setPlaceholderText(S_PLACEHOLDER.get());
        queryBtn->setText(S_QUERY.get());

        if (g_platform == PLAT_CF) {
            statusBar()->showMessage(S_READY.get());
            showPlaceholder();
        }
    }

    void switchPlatform(QAbstractButton *btn) {
        g_platform = (Platform)btn->property("plat").toInt();
        updateTabStyles();
        clearResultLayout();
        handleEdit->clear();
        handleEdit->setFocus();

        if (g_platform == PLAT_CF) {
            handleEdit->setEnabled(true);
            queryBtn->setEnabled(true);
            handleEdit->setPlaceholderText(S_PLACEHOLDER.get());
            showPlaceholder();
            statusBar()->showMessage(S_READY.get());
        } else {
            handleEdit->setEnabled(false);
            queryBtn->setEnabled(false);
            handleEdit->setPlaceholderText("");
            showComingSoon();
            statusBar()->showMessage("");
        }
    }

    void doQuery() {
        if (g_platform != PLAT_CF) return;

        QString handle = handleEdit->text().trimmed();
        if (handle.isEmpty()) return;

        showLoading();

        CFUserData data = queryCodeforces(handle.toStdString());
        showResult(data);
    }

private:
    void addShadow(QWidget *w) {
        auto *shadow = new QGraphicsDropShadowEffect(this);
        shadow->setBlurRadius(20);
        shadow->setOffset(0, 2);
        shadow->setColor(QColor(0, 0, 0, 25));
        w->setGraphicsEffect(shadow);
    }

    void updateTabStyles() {
        for (int i = 0; i < 3; i++) {
            bool sel = (i == (int)g_platform);
            platformBtns[i]->setStyleSheet(QString(
                "QPushButton {"
                "  background: %1; color: %2;"
                "  border-radius: 6px; border: none;"
                "  padding: 0 16px;"
                "}"
                "QPushButton:hover { background: %3; }"
            ).arg(sel ? "#4A90D9" : "transparent")
             .arg(sel ? "white" : "#7F8C8D")
             .arg(sel ? "#357ABD" : "#E8ECF1"));
        }
    }

    void clearResultLayout() {
        QLayoutItem *item;
        while ((item = resultInnerLayout->takeAt(0)) != nullptr) {
            delete item->widget();
            delete item;
        }
    }

    void showPlaceholder() {
        clearResultLayout();
        auto *ph = new QLabel(S_EMPTY_PROMPT.get());
        ph->setFont(QFont("Segoe UI", 11));
        ph->setStyleSheet("color: #AAB7C4; background: transparent; border: none;");
        ph->setAlignment(Qt::AlignCenter);
        ph->setMinimumHeight(140);
        resultInnerLayout->addWidget(ph);
    }

    void showComingSoon() {
        clearResultLayout();
        auto *icon = new QLabel(QString::fromUtf8("\xF0\x9F\x9A\x80"));
        icon->setFont(QFont("Segoe UI Emoji", 36));
        icon->setAlignment(Qt::AlignCenter);
        icon->setStyleSheet("background: transparent; border: none;");
        auto *msg = new QLabel(S_COMING_SOON.get());
        msg->setFont(QFont("Segoe UI", 16, QFont::Bold));
        msg->setStyleSheet("color: #BDC3C7; background: transparent; border: none;");
        msg->setAlignment(Qt::AlignCenter);
        auto *desc = new QLabel(S_COMING_DESC.get());
        desc->setFont(QFont("Segoe UI", 10));
        desc->setStyleSheet("color: #BDC3C7; background: transparent; border: none;");
        desc->setAlignment(Qt::AlignCenter);
        resultInnerLayout->addStretch();
        resultInnerLayout->addWidget(icon);
        resultInnerLayout->addWidget(msg);
        resultInnerLayout->addWidget(desc);
        resultInnerLayout->addStretch();
    }

    void showLoading() {
        clearResultLayout();
        handleEdit->setEnabled(false);
        queryBtn->setEnabled(false);

        auto *loading = new QLabel(S_QUERYING.get());
        loading->setFont(QFont("Segoe UI", 13));
        loading->setStyleSheet("color: #4A90D9; background: transparent; border: none;");
        loading->setAlignment(Qt::AlignCenter);
        loading->setMinimumHeight(140);
        resultInnerLayout->addWidget(loading);

        statusBar()->showMessage(S_FETCHING.get() + handleEdit->text() + "...");
        repaint();
    }

    void showResult(const CFUserData &data) {
        clearResultLayout();
        handleEdit->setEnabled(true);
        queryBtn->setEnabled(true);
        handleEdit->setFocus();
        handleEdit->selectAll();

        if (!data.error.empty()) {
            auto *err = new QLabel(QString::fromUtf8("\xE2\x9A\xA0  %1")
                .arg(QString::fromStdString(data.error)));
            err->setFont(QFont("Segoe UI", 12));
            err->setStyleSheet("color: #E74C3C; background: transparent; border: none; padding: 10px;");
            err->setAlignment(Qt::AlignCenter);
            err->setWordWrap(true);
            err->setMinimumHeight(140);
            resultInnerLayout->addWidget(err);
            statusBar()->showMessage(S_FAILED.get());
            return;
        }

        // Handle + rank badge
        auto *hr = new QHBoxLayout();
        auto *hl = new QLabel(QString::fromStdString(data.handle));
        hl->setFont(QFont("Segoe UI", 16, QFont::Bold));
        hl->setStyleSheet("color: #2C3E50; background: transparent; border: none;");

        auto *rb = new QLabel(QString::fromStdString(data.rank));
        rb->setFont(QFont("Segoe UI", 9, QFont::Bold));
        QString rc = ratingColor(data.rating);
        rb->setStyleSheet(QString(
            "color: white; background-color: %1; border-radius: 4px;"
            "padding: 3px 10px; border: none;"
        ).arg(rc));

        hr->addWidget(hl);
        hr->addStretch();
        hr->addWidget(rb);
        resultInnerLayout->addLayout(hr);
        resultInnerLayout->addSpacing(8);

        // Rating big number
        auto *big = new QLabel(QString::number(data.rating));
        big->setFont(QFont("Consolas", 40, QFont::Bold));
        big->setStyleSheet(QString("color: %1; background: transparent; border: none;").arg(rc));
        big->setAlignment(Qt::AlignCenter);
        resultInnerLayout->addWidget(big);

        auto *rtl = new QLabel(ratingTitle(data.rating));
        rtl->setFont(QFont("Segoe UI", 11));
        rtl->setStyleSheet(QString("color: %1; background: transparent; border: none;").arg(rc));
        rtl->setAlignment(Qt::AlignCenter);
        resultInnerLayout->addWidget(rtl);
        resultInnerLayout->addSpacing(12);

        // Stats
        auto addStat = [&](const QString &icon, const QString &label, const QString &value) {
            auto *row = new QHBoxLayout();
            auto *ico = new QLabel(icon);
            ico->setFont(QFont("Segoe UI Emoji", 13));
            ico->setStyleSheet("background: transparent; border: none;");
            ico->setFixedWidth(28);
            auto *lbl = new QLabel(label);
            lbl->setFont(QFont("Segoe UI", 11));
            lbl->setStyleSheet("color: #7F8C8D; background: transparent; border: none;");
            auto *val = new QLabel(value);
            val->setFont(QFont("Segoe UI", 13, QFont::Bold));
            val->setStyleSheet("color: #2C3E50; background: transparent; border: none;");
            row->addWidget(ico);
            row->addWidget(lbl);
            row->addStretch();
            row->addWidget(val);
            resultInnerLayout->addLayout(row);
        };

        addStat(QString::fromUtf8("\xF0\x9F\x93\x88"), S_MAX_RATING.get(), QString::number(data.maxRating));
        addStat(QString::fromUtf8("\xE2\x9C\x85"), S_SOLVED.get(), QString::number(data.solvedCount));

        statusBar()->showMessage(S_DONE.get() + "  \xe2\x80\xa2  " + QString::fromStdString(data.handle));
    }

    // Widgets
    QLabel *titleLabel, *subtitleLabel;
    QLineEdit *handleEdit;
    QPushButton *queryBtn, *langBtn;
    QFrame *searchCard, *resultCard;
    QVBoxLayout *resultInnerLayout;
    QPushButton *platformBtns[3];
    QButtonGroup tabGroup;
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setStyle("Fusion");

    QFont defaultFont("Segoe UI", 10);
    app.setFont(defaultFont);

    MainWindow w;
    w.show();
    return app.exec();
}

#include "main.moc"
