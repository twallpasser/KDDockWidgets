/*
  This file is part of KDDockWidgets.

  SPDX-FileCopyrightText: 2019-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Sérgio Martins <sergio.martins@kdab.com>

  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "TabBar_qtwidgets.h"
#include "../controllers/TabBar.h"
#include "private/Utils_p.h"
#include "private/Logging_p.h"
#include "kddockwidgets/FrameworkWidgetFactory.h"
#include "kddockwidgets/private/DockRegistry_p.h"


using namespace KDDockWidgets;
using namespace KDDockWidgets::Views;


TabBar_qtwidgets::TabBar_qtwidgets(Controllers::TabBar *controller, QWidget *parent)
    : View_qtwidgets(controller, parent)
    , m_controller(controller)
{
}
