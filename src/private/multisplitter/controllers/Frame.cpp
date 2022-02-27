/*
  This file is part of KDDockWidgets.

  SPDX-FileCopyrightText: 2020-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Sérgio Martins <sergio.martins@kdab.com>

  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "Frame.h"

#include "kddockwidgets/Config.h"
#include "kddockwidgets/FrameworkWidgetFactory.h"

#include "private/multisplitter/Controller.h"
#include "private/multisplitter/View.h"
#include "private/multisplitter/controllers/TitleBar.h"
#include "private/multisplitter/controllers/Stack.h"
#include "private/Logging_p.h"
#include "private/Utils_p.h"
#include "private/DockRegistry_p.h"
#include "private/DockWidgetBase_p.h"
#include "private/FloatingWindow_p.h"
#include "private/LayoutSaver_p.h"
#include "private/LayoutWidget_p.h"
#include "private/Position_p.h"
#include "private/WidgetResizeHandler_p.h"
#include "private/MDILayoutWidget_p.h"
#include "private/DropAreaWithCentralFrame_p.h"
#include "private/multisplitter/Item_p.h"
#include "private/multisplitter/views_qtwidgets/Frame_qtwidgets.h"

#include <QCloseEvent>
#include <QTimer>
#include <qobject.h>

#define MARGIN_THRESHOLD 100

static int s_dbg_numFrames = 0;

using namespace KDDockWidgets;
using namespace KDDockWidgets::Controllers;

namespace KDDockWidgets {

static FrameOptions actualOptions(FrameOptions options)
{
    if (Config::self().flags() & Config::Flag_AlwaysShowTabs)
        options |= FrameOption_AlwaysShowsTabs;

    return options;
}

static TabWidgetOptions tabWidgetOptions(FrameOptions options)
{
    if (options & FrameOption_NonDockable) {
        /// If we can't tab things into this Frame then let's not draw the QTabWidget frame either
        return TabWidgetOption_DocumentMode;
    }

    return TabWidgetOption_None;
}

}

Frame::Frame(View *parent, FrameOptions options, int userType)
    : Controller(new Views::Frame_qtwidgets(this, parent->asQWidget()))
    , FocusScope(qobject_cast<Views::View_qtwidgets<QWidget> *>(view()->asQWidget())) // TODO
    , m_tabWidget(new Controllers::Stack(this, tabWidgetOptions(options)))
    , m_titleBar(new Controllers::TitleBar(this))
    , m_options(actualOptions(options))
    , m_userType(userType)
{
    s_dbg_numFrames++;
    DockRegistry::self()->registerFrame(this);

    connect(this, &Frame::currentDockWidgetChanged, this, &Frame::updateTitleAndIcon);
    connect(m_tabWidget, &Controllers::Stack::currentTabChanged,
            this, &Frame::onCurrentTabChanged);

    setLayoutWidget(qobject_cast<LayoutWidget *>(parent->asQWidget())); // TODO
    m_inCtor = false;
}

Frame::~Frame()
{
    m_inDtor = true;
    s_dbg_numFrames--;
    if (m_layoutItem)
        m_layoutItem->unref();

    delete m_resizeHandler;
    m_resizeHandler = nullptr;

    DockRegistry::self()->unregisterFrame(this);

    // Run some disconnects() too, so we don't receive signals during destruction:
    setLayoutWidget(nullptr);
    delete m_titleBar;
    delete m_tabWidget;
}

void Frame::onCloseEvent(QCloseEvent *e)
{
    qCDebug(closing) << "Frame::closeEvent";
    e->accept(); // Accepted by default (will close unless ignored)
    const DockWidgetBase::List docks = dockWidgets();
    for (DockWidgetBase *dock : docks) {
        qApp->sendEvent(dock, e);
        if (!e->isAccepted())
            break; // Stop when the first dockwidget prevents closing
    }
}

void Frame::renameTab(int index, const QString &title)
{
    // TODO
    qobject_cast<Views::Frame_qtwidgets *>(view()->asQWidget())->renameTab(index, title);
}

void Frame::changeTabIcon(int index, const QIcon &icon)
{
    // TODO
    qobject_cast<Views::Frame_qtwidgets *>(view()->asQWidget())->changeTabIcon(index, icon);
}

void Frame::removeWidget_impl(DockWidgetBase *dw)
{
    // TODO
    qobject_cast<Views::Frame_qtwidgets *>(view()->asQWidget())->removeWidget_impl(dw);
}

int Frame::indexOfDockWidget_impl(const DockWidgetBase *dw)
{
    // TODO
    return qobject_cast<Views::Frame_qtwidgets *>(view()->asQWidget())->indexOfDockWidget_impl(dw);
}

int Frame::currentIndex_impl() const
{
    // TODO
    return qobject_cast<Views::Frame_qtwidgets *>(view()->asQWidget())->currentIndex_impl();
}

void Frame::setCurrentTabIndex_impl(int index)
{
    // TODO
    qobject_cast<Views::Frame_qtwidgets *>(view()->asQWidget())->setCurrentTabIndex_impl(index);
}

void Frame::setCurrentDockWidget_impl(DockWidgetBase *dw)
{
    qobject_cast<Views::Frame_qtwidgets *>(view()->asQWidget())->setCurrentDockWidget_impl(dw);
}

void Frame::insertDockWidget_impl(DockWidgetBase *dw, int index)
{
    qobject_cast<Views::Frame_qtwidgets *>(view()->asQWidget())->insertDockWidget_impl(dw, index);
}

DockWidgetBase *Frame::dockWidgetAt_impl(int index) const
{
    return qobject_cast<Views::Frame_qtwidgets *>(view()->asQWidget())->dockWidgetAt_impl(index);
}

DockWidgetBase *Frame::currentDockWidget_impl() const
{
    return qobject_cast<Views::Frame_qtwidgets *>(view()->asQWidget())->currentDockWidget_impl();
}

int Frame::nonContentsHeight() const
{
    return qobject_cast<Views::Frame_qtwidgets *>(view()->asQWidget())->nonContentsHeight();
}

Controllers::Stack *Frame::tabWidget() const
{
    return m_tabWidget;
}

Controllers::TabBar *Frame::tabBar() const
{
    return m_tabWidget->tabBar();
}
