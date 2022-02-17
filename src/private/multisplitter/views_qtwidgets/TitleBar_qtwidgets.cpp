/*
  This file is part of KDDockWidgets.

  SPDX-FileCopyrightText: 2019-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Sérgio Martins <sergio.martins@kdab.com>

  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "TitleBar_qtwidgets.h"
#include "../controllers/TitleBar.h"
#include "private/Utils_p.h"
#include "kddockwidgets/FrameworkWidgetFactory.h"

#include <QPainter>
#include <QStyle>
#include <QStyleOptionDockWidget>
#include <QHBoxLayout>
#include <QLabel>

using namespace KDDockWidgets;
using namespace KDDockWidgets::Views;

Button::~Button()
{
}

void Button::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    QStyleOptionToolButton opt;
    opt.initFrom(this);

    if (isEnabled() && underMouse()) {
        if (isDown()) {
            opt.state |= QStyle::State_Sunken;
        } else {
            opt.state |= QStyle::State_Raised;
        }
        style()->drawPrimitive(QStyle::PE_PanelButtonTool, &opt, &p, this);
    }

    opt.subControls = QStyle::SC_None;
    opt.features = QStyleOptionToolButton::None;
    opt.icon = icon();

    // The first icon size is for scaling 1x, and is what QStyle expects. QStyle will pick ones
    // with higher resolution automatically when needed.
    const QList<QSize> iconSizes = opt.icon.availableSizes();
    if (!iconSizes.isEmpty()) {
        opt.iconSize = iconSizes.constFirst();

        const qreal logicalFactor = logicalDpiX() / 96.0;

        // On Linux there's dozens of window managers and ways of setting the scaling.
        // Some window managers will just change the font dpi (which affects logical dpi), while
        // others will only change the device pixel ratio. Take care of both cases.
        // macOS is easier, as it never changes logical DPI.
        // On Windows, with AA_EnableHighDpiScaling, logical DPI is always 96 and physical is manipulated instead.
#if defined(Q_OS_LINUX)
        const qreal dpr = devicePixelRatioF();
        const qreal combinedFactor = logicalFactor * dpr;

        if (scalingFactorIsSupported(combinedFactor)) // Older Qt has rendering bugs with fractional factors
            opt.iconSize = opt.iconSize * combinedFactor;
#elif defined(Q_OS_WIN) && QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        // Probably Windows could use the same code path as Linux, but I'm seeing too thick icons on Windows...
        if (!QGuiApplication::testAttribute(Qt::AA_EnableHighDpiScaling)
            && scalingFactorIsSupported(logicalFactor)) // Older Qt has rendering bugs with fractional factors
            opt.iconSize = opt.iconSize * logicalFactor;
#else
        Q_UNUSED(logicalFactor);
#endif
    }

    style()->drawComplexControl(QStyle::CC_ToolButton, &opt, &p, this);
}

QSize Button::sizeHint() const
{
    // Pass an opt so it scales against the logical dpi of the correct screen (since Qt 5.14) even if the HDPI Qt::AA_ attributes are off.
    QStyleOption opt;
    opt.initFrom(this);

    const int m = style()->pixelMetric(QStyle::PM_SmallIconSize, &opt, this);
    return QSize(m, m);
}

TitleBar_qtwidgets::TitleBar_qtwidgets(Controllers::TitleBar *controller, QWidget *parent)
    : View_qtwidgets(controller, parent)
    , m_controller(controller)
    , m_layout(new QHBoxLayout(this))
{
    if (m_controller->titleBarIsFocusable())
        setFocusPolicy(Qt::StrongFocus);
}

void TitleBar_qtwidgets::paintEvent(QPaintEvent *)
{
    QPainter p(this);

    QStyleOptionDockWidget titleOpt;
    titleOpt.initFrom(this);
    style()->drawPrimitive(QStyle::PE_Widget, &titleOpt, &p, this);
    titleOpt.title = m_controller->title();
    titleOpt.rect = iconRect().isEmpty() ? rect().adjusted(2, 0, -buttonAreaWidth(), 0)
                                         : rect().adjusted(iconRect().right(), 0, -buttonAreaWidth(), 0);

    if (m_controller->isMDI()) {
        const QColor c = palette().color(QPalette::Base);
        p.fillRect(rect().adjusted(1, 1, -1, 0), c);
    }

    style()->drawControl(QStyle::CE_DockWidgetTitle, &titleOpt, &p, this);
}

void TitleBar_qtwidgets::updateMinimizeButton()
{
    m_minimizeButton->setVisible(m_controller->supportsMinimizeButton());
}

void TitleBar_qtwidgets::updateAutoHideButton()
{
    if (Config::self().flags() & Config::Flag_AutoHideSupport) {
        auto factory = Config::self().frameworkWidgetFactory();
        if (const Frame *f = m_controller->frame()) {
            if (f->isInMainWindow()) {
                m_autoHideButton->setIcon(factory->iconForButtonType(TitleBarButtonType::AutoHide, devicePixelRatioF()));
                m_autoHideButton->setToolTip(tr("Auto-hide"));
            } else if (f->isOverlayed()) {
                m_autoHideButton->setIcon(factory->iconForButtonType(TitleBarButtonType::UnautoHide, devicePixelRatioF()));
                m_autoHideButton->setToolTip(tr("Disable auto-hide"));
            }
        }
    }

    m_autoHideButton->setVisible(m_controller->supportsAutoHideButton());
}

void TitleBar_qtwidgets::updateMaximizeButton()
{
    if (auto fw = m_controller->floatingWindow()) {
        auto factory = Config::self().frameworkWidgetFactory();
        const TitleBarButtonType iconType = fw->isMaximizedOverride() ? TitleBarButtonType::Normal
                                                                      : TitleBarButtonType::Maximize;
        m_maximizeButton->setIcon(factory->iconForButtonType(iconType, devicePixelRatioF()));

        m_maximizeButton->setVisible(m_controller->supportsMaximizeButton());
        m_maximizeButton->setToolTip(fw->isMaximizedOverride() ? tr("Restore") : tr("Maximize"));
    } else {
        m_maximizeButton->setVisible(false);
    }
}

QRect TitleBar_qtwidgets::iconRect() const
{
    if (m_controller->icon().isNull()) {
        return QRect(0, 0, 0, 0);
    } else {
        return QRect(3, 3, 30, 30);
    }
}

int TitleBar_qtwidgets::buttonAreaWidth() const
{
    int smallestX = width();

    for (auto button : { m_autoHideButton, m_minimizeButton, m_floatButton, m_maximizeButton, m_closeButton }) {
        if (button->isVisible() && button->x() < smallestX)
            smallestX = button->x();
    }

    return width() - smallestX;
}

void TitleBar_qtwidgets::updateMargins()
{
    const qreal factor = logicalDpiFactor(this);
    m_layout->setContentsMargins(QMargins(2, 2, 2, 2) * factor);
    m_layout->setSpacing(int(2 * factor));
}

void TitleBar_qtwidgets::mouseDoubleClickEvent(QMouseEvent *e)
{
    if (!m_controller)
        return;

    if (e->button() == Qt::LeftButton)
        m_controller->onDoubleClicked();
}

QSize TitleBar_qtwidgets::sizeHint() const
{
    // Pass an opt so it scales against the logical dpi of the correct screen (since Qt 5.14) even if the HDPI Qt::AA_ attributes are off.
    QStyleOption opt;
    opt.initFrom(this);

    const int height =
        style()->pixelMetric(QStyle::PM_HeaderDefaultSectionSizeVertical, &opt, this);

    return QSize(0, height);
}

void TitleBar_qtwidgets::focusInEvent(QFocusEvent *ev)
{
    if (!freed())
        return;

    QWidget::focusInEvent(ev);
    m_controller->focusInEvent(ev);
}

Controllers::TitleBar *TitleBar_qtwidgets::titleBar() const
{
    return m_controller;
}

#ifdef DOCKS_DEVELOPER_MODE

bool TitleBar_qtwidgets::isCloseButtonVisible() const
{
    return m_closeButton->isVisible();
}

bool TitleBar_qtwidgets::isCloseButtonEnabled() const
{
    return m_closeButton->isEnabled();
}

bool TitleBar_qtwidgets::isFloatButtonVisible() const
{
    return m_floatButton->isVisible();
}

bool TitleBar_qtwidgets::isFloatButtonEnabled() const
{
    return m_floatButton->isEnabled();
}

#endif
