#include "windowkitwidget.h"
#include <QEvent>
#include <QFile>
#include <QStyle>
#include <QWKWidgets/widgetwindowagent.h>
#include <widgetframe/windowbar.h>
#include <widgetframe/windowbutton.h>

WindowKitWidget::WindowKitWidget(QWidget *parent)
    : QWidget{parent}
{
    windowAgent = new QWK::WidgetWindowAgent(this);
    windowAgent->setup(this);

    auto iconButton = new QWK::WindowButton();
    iconButton->setObjectName(QStringLiteral("icon-button"));
    iconButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    auto minButton = new QWK::WindowButton();
    minButton->setObjectName(QStringLiteral("min-button"));
    minButton->setProperty("system-button", true);
    minButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    auto maxButton = new QWK::WindowButton();
    maxButton->setCheckable(true);
    maxButton->setObjectName(QStringLiteral("max-button"));
    maxButton->setProperty("system-button", true);
    maxButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    auto closeButton = new QWK::WindowButton();
    closeButton->setObjectName(QStringLiteral("close-button"));
    closeButton->setProperty("system-button", true);
    closeButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    auto pinButton = new QWK::WindowButton();
    pinButton->setObjectName(QStringLiteral("pin-button"));
    pinButton->setProperty("system-button", true);
    pinButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    auto menuBar = new QMenuBar();
    {
        auto file = new QMenu(tr("File(&F)"), menuBar);
        file->addAction(new QAction(tr("New(&N)"), menuBar));
        file->addAction(new QAction(tr("Open(&O)"), menuBar));
        file->addSeparator();
        auto edit = new QMenu(tr("Edit(&E)"), menuBar);
        edit->addAction(new QAction(tr("Undo(&U)"), menuBar));
        edit->addAction(new QAction(tr("Redo(&R)"), menuBar));
        auto darkAction = new QAction(tr("Enable dark theme"), menuBar);
        darkAction->setCheckable(true);
        connect(darkAction, &QAction::triggered, this,
                [this](bool checked) {
            loadStyleSheet(checked ? Dark : Light);
        });
        auto settings = new QMenu(tr("Settings(&S)"), menuBar);
        settings->addAction(darkAction);
        menuBar->addMenu(file);
        menuBar->addMenu(edit);
        menuBar->addMenu(settings);
    }
    auto titleLabel = new QLabel();
    titleLabel->setText("窗口测试");
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setObjectName(QStringLiteral("win-title-label"));

    QWK::WindowBar *windowBar = new QWK::WindowBar(this);
    windowBar->setIconButton(iconButton);
    windowBar->setMinButton(minButton);
    windowBar->setMaxButton(maxButton);
    windowBar->setCloseButton(closeButton);
    windowBar->setPinButton(pinButton);
    windowBar->setMenuBar(menuBar);
    windowBar->setTitleLabel(titleLabel);
    windowBar->setHostWidget(this);

    windowAgent->setTitleBar(windowBar);

    // style()->polish(this);
    loadStyleSheet(Light);
    windowAgent->setSystemButton(QWK::WindowAgentBase::WindowIcon, iconButton);
    windowAgent->setSystemButton(QWK::WindowAgentBase::Minimize,   minButton);
    windowAgent->setSystemButton(QWK::WindowAgentBase::Maximize,   maxButton);
    windowAgent->setSystemButton(QWK::WindowAgentBase::Close,      closeButton);
    windowAgent->setHitTestVisible(windowBar->menuBar(), true);

    // windowAgent->systemButton(QWK::WindowAgentBase::WindowIcon);

    connect(iconButton, &QAbstractButton::clicked, windowAgent,
            [this, iconButton] {
        iconButton->setProperty("double-click-close", false);
    });
    connect(iconButton, &QWK::WindowButton::doubleClicked, this,
            [iconButton, this]() {
        iconButton->setProperty("double-click-close", true);
        close();
    });
    connect(windowBar, &QWK::WindowBar::minimizeRequested, this,
            &QWidget::showMinimized);
    connect(windowBar, &QWK::WindowBar::maximizeRequested, this,
            [this, maxButton](bool max) {
        if (max) {
            showMaximized();
        } else {
            showNormal();
        }
    });
    connect(windowBar, &QWK::WindowBar::closeRequested, this, &QWidget::close);
    connect(pinButton, &QAbstractButton::clicked,       this, [ = ]() {
        static bool pin = false;
        pin = !pin;

        if (pin) setWindowFlags(Qt::WindowStaysOnTopHint);
        else setWindowFlags(Qt::Widget);
        qDebug();
    });

    connect(windowBar, &QWK::WindowBar::pinRequested, this, [ = ](bool pin)
    {
        if (pin) setWindowFlags(Qt::WindowStaysOnTopHint);
        else setWindowFlags(Qt::Widget);
        qDebug();
    });
}

void WindowKitWidget::loadStyleSheet(Theme theme)
{
    currentTheme = theme;
    QString qssfilename = ":/dark-style.qss";

    if (currentTheme != Dark) {
        qssfilename = ":/light-style.qss";
    }
    QFile qss(qssfilename);

    if (qss.open(QIODevice::ReadOnly)) {
        setStyleSheet(QString::fromUtf8(qss.readAll()));
        qss.close();
    }
}
