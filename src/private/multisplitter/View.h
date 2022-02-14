/*
  This file is part of KDDockWidgets.

  SPDX-FileCopyrightText: 2020-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Sérgio Martins <sergio.martins@kdab.com>

  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#pragma once

#include <QSize> // TODO Remove Qt headers, introduce Size and Rect structs
#include <QRect>

#include <QObject>

namespace Layouting {
class Item;
}

namespace KDDockWidgets {

class View
{
public:
    explicit View(QObject *thisObj);
    virtual ~View();

    QObject *asQObject() const;
    QObject *parent() const;

    ///@brief returns an id for corelation purposes for saving layouts
    QString id() const;

    /// @brief Deletes this view.
    /// The default impl will just do a normal C++ "delete", but derived classes are free
    /// to implement other ways, for example QObject::deleteLater(), which is recommended for Qt.
    virtual void free();

    /// @brief Called by the layouting engine
    /// Override it in case your widget needs to know where it is in the layout. Usually only needed by Frame.s
    virtual void setLayoutItem(Layouting::Item *) {};

    virtual void setParent(View *) = 0;
    virtual QSize sizeHint() const;
    virtual QSize minSize() const = 0;
    virtual QSize maxSizeHint() const = 0;
    virtual QRect geometry() const = 0;
    virtual void setGeometry(QRect) = 0;
    virtual bool isVisible() const = 0;
    virtual void setVisible(bool) const = 0;
    virtual void move(int x, int y) = 0;
    virtual void setSize(int width, int height) = 0;
    virtual void setWidth(int width) = 0;
    virtual void setHeight(int height) = 0;
    virtual void show() = 0;
    virtual void hide() = 0;
    virtual void update() = 0;

    QSize size() const;
    QRect rect() const;
    int x() const;
    int y() const;
    int height() const;
    int width() const;

    static QSize hardcodedMinimumSize();
    static QSize boundedMaxSize(QSize min, QSize max);

protected:
    QObject *const m_thisObj;

private:
    const QString m_id;
};

}
