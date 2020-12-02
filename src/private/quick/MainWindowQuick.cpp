/*
  This file is part of KDDockWidgets.

  SPDX-FileCopyrightText: 2019-2020 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Sérgio Martins <sergio.martins@kdab.com>

  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "MainWindowQuick_p.h"
#include "DropAreaWithCentralFrame_p.h"

#include <QDebug>

using namespace KDDockWidgets;

MainWindowQuick::MainWindowQuick(const QString &uniqueName, MainWindowOptions options,
                                 QQuickItem *parent)
    : MainWindowBase(uniqueName, options, parent)
{
    QWidgetAdapter::makeItemFillParent(this);

    MultiSplitter *ms = dropArea();
    QWidgetAdapter::makeItemFillParent(ms);


    // MainWindowQuick has the same constraints as MultiSplitter, so just forward the signal
    connect(ms, &MultiSplitter::geometryUpdated,
            this, &MainWindowQuick::geometryUpdated);

    connect(ms, &MultiSplitter::geometryUpdated,
            this, &MainWindowQuick::onMultiSplitterGeometryUpdated);
}

MainWindowQuick::~MainWindowQuick()
{
    if (isTopLevel()) {
        if (QWindow *window = windowHandle()) {
            QObject::setParent(nullptr);
            delete window;
        }
    }
}

QSize MainWindowQuick::minimumSize() const
{
    return multiSplitter()->rootItem()->minSize();
}

QSize MainWindowQuick::maximumSize() const
{
    return multiSplitter()->rootItem()->maxSizeHint();
}

SideBar *MainWindowQuick::sideBar(SideBarLocation) const
{
    return nullptr;
}

QMargins MainWindowQuick::centerWidgetMargins() const
{
    qDebug() << Q_FUNC_INFO << "SideBar hasn't been implemented yet";
    return {};
}

void MainWindowQuick::onMultiSplitterGeometryUpdated()
{
    const QSize minSz = minimumSize();
    const bool mainWindowIsTooSmall = minSz.expandedTo(size()) != size();
    if (mainWindowIsTooSmall) {
        if (isTopLevel()) {
            // If we're a top-level, let's go ahead and resize the QWindow
            // any other case is too complex for QtQuick as there's no layout propagation.
            windowHandle()->resize(minSz);
        }
    }
}
