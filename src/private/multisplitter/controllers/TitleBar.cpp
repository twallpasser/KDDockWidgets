/*
  This file is part of KDDockWidgets.

  SPDX-FileCopyrightText: 2020-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Sérgio Martins <sergio.martins@kdab.com>

  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "TitleBar.h"
#include "Config.h"
#include "FrameworkWidgetFactory.h"
#include "private/multisplitter/View.h"
#include "private/multisplitter/views_qtwidgets/View_qtwidgets.h"
#include "private/multisplitter/views_qtwidgets/TitleBar_qtwidgets.h"
#include "private/WindowBeingDragged_p.h"

#include "kddockwidgets/private/FloatingWindow_p.h"
#include "kddockwidgets/private/Frame_p.h"
#include "kddockwidgets/FrameworkWidgetFactory.h"
#include "kddockwidgets/private/MDILayoutWidget_p.h"

#include <QTimer>

using namespace KDDockWidgets::Controllers;


TitleBar::TitleBar(Frame *parent)
    : Controller(Config::self().frameworkWidgetFactory()->createTitleBar(this, parent))
    , Draggable(view()->asQWidget()) // TODO
    , m_frame(parent)
    , m_floatingWindow(nullptr)
    , m_supportsAutoHide(Config::self().flags() & Config::Flag_AutoHideSupport)
{
    connect(m_frame, &Frame::numDockWidgetsChanged, this, &TitleBar::updateCloseButton);
    connect(m_frame, &Frame::isFocusedChanged, this, &TitleBar::isFocusedChanged);
    connect(m_frame, &Frame::isInMainWindowChanged, this, &TitleBar::updateAutoHideButton);

    init();
}

TitleBar::TitleBar(FloatingWindow *parent)
    : Controller(Config::self().frameworkWidgetFactory()->createTitleBar(this, parent))
    , Draggable(view()->asQWidget()) // TODO
    , m_frame(nullptr)
    , m_floatingWindow(parent)
    , m_supportsAutoHide(Config::self().flags() & Config::Flag_AutoHideSupport)
{
    connect(m_floatingWindow, &FloatingWindow::numFramesChanged, this, &TitleBar::updateButtons);
    connect(m_floatingWindow, &FloatingWindow::windowStateChanged, static_cast<Views::TitleBar_qtwidgets *>(view()), &Views::TitleBar_qtwidgets::updateMaximizeButton); // TODO
    connect(m_floatingWindow, &FloatingWindow::activatedChanged, this, &TitleBar::isFocusedChanged);

    init();
}

void TitleBar::init()
{
    view()->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed));

    connect(this, &TitleBar::isFocusedChanged, this, [this] {
        // repaint
        view()->update();
    });

    updateButtons();
    QTimer::singleShot(0, this, &TitleBar::updateAutoHideButton); // have to wait after the frame is
                                                                  // constructed
}

TitleBar::~TitleBar()
{
}

bool TitleBar::titleBarIsFocusable() const
{
    return Config::self().flags() & Config::Flag_TitleBarIsFocusable;
}


KDDockWidgets::MainWindowBase *TitleBar::mainWindow() const
{
    if (m_floatingWindow)
        return nullptr;

    if (m_frame)
        return m_frame->mainWindow();

    qWarning() << Q_FUNC_INFO << "null frame and null floating window";
    return nullptr;
}

bool TitleBar::isMDI() const
{
    QObject *p = view()->asQWidget();
    while (p) {
        if (qobject_cast<const QWindow *>(p)) {
            // Ignore QObject hierarchies spanning though multiple windows
            return false;
        }

        if (qobject_cast<MDILayoutWidget *>(p))
            return true;

        if (qobject_cast<DropArea *>(p)) {
            // Note that the TitleBar can be inside a DropArea that's inside a MDIArea
            // so we need this additional check
            return false;
        }

        p = p->parent();
    }

    return false;
}

QString TitleBar::title() const
{
    return m_title;
}

QIcon TitleBar::icon() const
{
    return m_icon;
}

bool TitleBar::onDoubleClicked()
{
    if ((Config::self().flags() & Config::Flag_DoubleClickMaximizes) && m_floatingWindow) {
        // Not using isFloating(), as that can be a dock widget nested in a floating window. By convention it's floating, but it's not the title bar of the top-level window.
        toggleMaximized();
        return true;
    } else if (supportsFloatingButton()) {
        onFloatClicked();
        return true;
    }

    return false;
}

bool TitleBar::supportsFloatingButton() const
{
    if (Config::self().flags() & Config::Flag_TitleBarHasMaximizeButton) {
        // Apps having a maximize/restore button traditionally don't have a floating one,
        // QDockWidget style only has floating and no maximize/restore.
        // We can add an option later if we need them to co-exist
        return false;
    }

    if (Config::self().flags() & Config::Flag_TitleBarNoFloatButton) {
        // Was explicitly disabled
        return false;
    }

    if (DockWidgetBase *dw = singleDockWidget()) {
        // Don't show the dock/undock button if the window is not dockable
        if (dw->options() & DockWidgetBase::Option_NotDockable)
            return false;
    }

    // If we have a floating window with nested dock widgets we can't re-attach, because we don't
    // know where to
    return !m_floatingWindow || m_floatingWindow->hasSingleFrame();
}

bool TitleBar::supportsMaximizeButton() const
{
    if (!(Config::self().flags() & Config::Flag_TitleBarHasMaximizeButton))
        return false;

    return m_floatingWindow != nullptr;
}

bool TitleBar::supportsMinimizeButton() const
{
    if ((Config::self().flags() & Config::Flag_TitleBarHasMinimizeButton) != Config::Flag_TitleBarHasMinimizeButton) // this specific flag is not base^2
        return false;

    return m_floatingWindow != nullptr;
}

bool TitleBar::supportsAutoHideButton() const
{
    // Only dock widgets docked into the MainWindow can minimize
    return m_supportsAutoHide && m_frame && (m_frame->isInMainWindow() || m_frame->isOverlayed());
}

bool TitleBar::hasIcon() const
{
    return !m_icon.isNull();
}

KDDockWidgets::Frame *TitleBar::frame() const
{
    return m_frame;
}

KDDockWidgets::FloatingWindow *TitleBar::floatingWindow() const
{
    return m_floatingWindow;
}

void TitleBar::focusInEvent(QFocusEvent *ev)
{
    if (!m_frame || !(Config::self().flags() & Config::Flag_TitleBarIsFocusable))
        return;

    // For some reason QWidget::setFocusProxy() isn't working, so forward manually
    m_frame->FocusScope::focus(ev->reason());
}

void TitleBar::updateButtons()
{
    updateCloseButton();
    updateFloatButton();
    updateMaximizeButton();
    updateMinimizeButton();
    updateAutoHideButton();
}

void TitleBar::updateCloseButton()
{

    const bool anyNonClosable = frame() ? frame()->anyNonClosable()
                                        : (floatingWindow() ? floatingWindow()->anyNonClosable()
                                                            : false);

    setCloseButtonEnabled(!anyNonClosable);
}

void TitleBar::toggleMaximized()
{
    if (!m_floatingWindow)
        return;

    if (m_floatingWindow->isMaximizedOverride())
        m_floatingWindow->showNormal();
    else
        m_floatingWindow->showMaximized();
}

bool TitleBar::isOverlayed() const
{
    return m_frame && m_frame->isOverlayed();
}

void TitleBar::setCloseButtonEnabled(bool enabled)
{
    if (enabled != m_closeButtonEnabled) {
        m_closeButtonEnabled = enabled;
        Q_EMIT closeButtonEnabledChanged(enabled);
    }
}

void TitleBar::setFloatButtonVisible(bool visible)
{
    if (visible != m_floatButtonVisible) {
        m_floatButtonVisible = visible;
        Q_EMIT floatButtonVisibleChanged(visible);
    }
}

void TitleBar::setFloatButtonToolTip(const QString &tip)
{
    if (tip != m_floatButtonToolTip) {
        m_floatButtonToolTip = tip;
        Q_EMIT floatButtonToolTipChanged(tip);
    }
}

void TitleBar::setTitle(const QString &title)
{
    if (title != m_title) {
        m_title = title;
        view()->update();
        Q_EMIT titleChanged();
    }
}

void TitleBar::setIcon(const QIcon &icon)
{
    m_icon = icon;
    Q_EMIT iconChanged();
}

void TitleBar::onCloseClicked()
{
}
void TitleBar::onFloatClicked()
{
}
void TitleBar::onMaximizeClicked()
{
}
void TitleBar::onMinimizeClicked()
{
}
void TitleBar::onAutoHideClicked()
{
}

bool TitleBar::closeButtonEnabled() const
{
    return false;
}

std::unique_ptr<KDDockWidgets::WindowBeingDragged> TitleBar::makeWindow()
{
    return {};
}

bool TitleBar::isWindow() const
{
    return false;
}

KDDockWidgets::DockWidgetBase *TitleBar::singleDockWidget() const
{
    return {};
}

bool TitleBar::isFloating() const
{
    return false;
}

bool TitleBar::isFocused() const
{
    return false;
}

void TitleBar::updateFloatButton()
{
}
