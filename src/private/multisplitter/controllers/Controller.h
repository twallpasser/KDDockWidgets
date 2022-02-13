/*
  This file is part of KDDockWidgets.

  SPDX-FileCopyrightText: 2020-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Sérgio Martins <sergio.martins@kdab.com>

  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#pragma once

namespace KDDockWidgets {

class View;

class Controller
{
public:
    virtual ~Controller();

    /// @brief Returns the view associated with this controller, if any.
    View *view() const;

protected:
    View *m_view = nullptr;
};

}
