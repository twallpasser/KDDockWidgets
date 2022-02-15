/*
  This file is part of KDDockWidgets.

  SPDX-FileCopyrightText: 2020-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Sérgio Martins <sergio.martins@kdab.com>

  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#pragma once

#include "kddockwidgets/docks_export.h"

#include <QSize> // TODO Remove Qt headers, introduce Size and Rect structs
#include <QRect>
#include <QObject>
#include <QSizePolicy>

#include <memory>

namespace Layouting {
class Item;
}

namespace KDDockWidgets {

class DOCKS_EXPORT View
{
public:
    explicit View(QObject *thisObj);
    virtual ~View();

    QObject *asQObject() const;
    QWidget *asQWidget() const; // TODO: Remove
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
    virtual void raiseAndActivate() = 0;
    virtual QPoint mapToGlobal(QPoint) const = 0;
    virtual std::unique_ptr<View> window() const = 0;

    // TODO: Check if these two should be in the controller or on view
    virtual void onLayoutRequest()
    {
    }
    virtual bool onResize(QSize /*newSize*/)
    {
        return false;
    }

    QSize size() const;
    QRect rect() const;
    int x() const;
    int y() const;
    int height() const;
    int width() const;
    void resize(QSize);

    static QSize hardcodedMinimumSize();
    static QSize boundedMaxSize(QSize min, QSize max);


    template<typename T>
    static QSize widgetMinSize(const T *w)
    {
        const int minW = w->minimumWidth() > 0 ? w->minimumWidth()
                                               : w->minimumSizeHint().width();

        const int minH = w->minimumHeight() > 0 ? w->minimumHeight()
                                                : w->minimumSizeHint().height();

        return QSize(minW, minH).expandedTo(View::hardcodedMinimumSize());
    }

    template<typename T>
    static QSize widgetMaxSize(const T *w)
    {
        // The max size is usually QWidget::maximumSize(), but we also honour the QSizePolicy::Fixed+sizeHint() case
        // as widgets don't need to have QWidget::maximumSize() to have a max size honoured

        const QSize min = widgetMinSize(w);
        QSize max = w->maximumSize();
        max = View::boundedMaxSize(min, max); // for safety against weird values

        const QSizePolicy policy = w->sizePolicy();

        if (policy.verticalPolicy() == QSizePolicy::Fixed || policy.verticalPolicy() == QSizePolicy::Maximum)
            max.setHeight(qMin(max.height(), w->sizeHint().height()));
        if (policy.horizontalPolicy() == QSizePolicy::Fixed || policy.horizontalPolicy() == QSizePolicy::Maximum)
            max.setWidth(qMin(max.width(), w->sizeHint().width()));

        max = View::boundedMaxSize(min, max); // for safety against weird values
        return max;
    }

protected:
    QObject *const m_thisObj;

private:
    const QString m_id;
};

}
