#include "Frame_qtwidgets.h"
#include "private/multisplitter/views_qtwidgets/View_qtwidgets.h"
#include "private/multisplitter/controllers/Frame.h"

using namespace KDDockWidgets::Views;

Frame_qtwidgets::Frame_qtwidgets(Controllers::Frame *controller, QWidget *parent)
    : View_qtwidgets<QWidget>(controller, parent)
    , m_controller(controller)
{
}

void Frame_qtwidgets::setLayoutItem(Layouting::Item *item)
{
    // TODO: Remove from View, maybe
    m_controller->setLayoutItem(item);
}

void Frame_qtwidgets::renameTab(int index, const QString &title)
{
    m_controller->tabWidget()->renameTab(index, text);
}

void Frame_qtwidgets::changeTabIcon(int index, const QIcon &icon)
{
    m_controller->tabWidget()->changeTabIcon(index, icon);
}


int Frame_qtwidgets::nonContentsHeight() const
{
    Controllers::TitleBar *tb = m_controller->titleBar();
    QWidget *tabBar = this->tabBar();

    return (tb->isVisible() ? tb->height() : 0) + (tabBar->isVisible() ? tabBar->height() : 0);
}

int Frame_qtwidgets::indexOfDockWidget_impl(const DockWidgetBase *dw)
{
    return m_controller->tabWidget()->indexOfDockWidget(dw);
}

void Frame_qtwidgets::setCurrentDockWidget_impl(DockWidgetBase *dw)
{
    m_controller->tabWidget()->setCurrentDockWidget(dw);
}

int Frame_qtwidgets::currentIndex_impl() const
{
    return m_controller->tabWidget()->currentIndex();
}

void Frame_qtwidgets::insertDockWidget_impl(DockWidgetBase *dw, int index)
{
    m_controller->tabWidget()->insertDockWidget(dw, index);
}

void Frame_qtwidgets::removeWidget_impl(DockWidgetBase *dw)
{
    m_controller->tabWidget()->removeDockWidget(dw);
}

void Frame_qtwidgets::setCurrentTabIndex_impl(int index)
{
    m_controller->tabWidget()->setCurrentDockWidget(index);
}

DockWidgetBase *Frame_qtwidgets::currentDockWidget_impl() const
{
    return m_controller->tabWidget()->dockwidgetAt(m_controller->tabWidget()->currentIndex());
}

DockWidgetBase *Frame_qtwidgets::dockWidgetAt_impl(int index) const
{
    return qobject_cast<DockWidgetBase *>(m_controller->tabWidget()->dockwidgetAt(index));
}