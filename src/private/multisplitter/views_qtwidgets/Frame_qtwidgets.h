/*
  This file is part of KDDockWidgets.

  SPDX-FileCopyrightText: 2019-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Sérgio Martins <sergio.martins@kdab.com>

  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#pragma once

#include "View_qtwidgets.h"

namespace KDDockWidgets::Controllers {
class Frame;
}

namespace KDDockWidgets::Views {

class DOCKS_EXPORT Frame_qtwidgets : public View_qtwidgets<QWidget>
{
    Q_OBJECT
public:
    explicit Frame_qtwidgets(Controllers::Frame *controller, QWidget *parent = nullptr);

    void setLayoutItem(Layouting::Item *item) override;

    void renameTab(int index, const QString &);
    void changeTabIcon(int index, const QIcon &);
    void removeWidget_impl(DockWidgetBase *);
    int indexOfDockWidget_impl(const DockWidgetBase *);
    int currentIndex_impl() const;
    void setCurrentTabIndex_impl(int index);
    void setCurrentDockWidget_impl(DockWidgetBase *);
    void insertDockWidget_impl(DockWidgetBase *, int index);
    DockWidgetBase *dockWidgetAt_impl(int index) const;
    DockWidgetBase *currentDockWidget_impl() const;
    int nonContentsHeight() const;


private:
    Controllers::Frame *const m_controller;
};

}
