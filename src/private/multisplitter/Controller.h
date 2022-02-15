/*
  This file is part of KDDockWidgets.

  SPDX-FileCopyrightText: 2020-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Sérgio Martins <sergio.martins@kdab.com>

  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#pragma once

#include "docks_export.h"
#include "kddockwidgets/docks_export.h"

namespace KDDockWidgets {

class View;

class DOCKS_EXPORT Controller
{
public:
    virtual ~Controller();

    /// @brief Returns the view associated with this controller, if any.
    View *view() const;

protected:
    virtual void createView(View *parent) = 0;
    View *m_view = nullptr;
};

}
