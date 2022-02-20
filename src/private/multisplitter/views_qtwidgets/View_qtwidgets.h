/*
  This file is part of KDDockWidgets.

  SPDX-FileCopyrightText: 2020-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Sérgio Martins <sergio.martins@kdab.com>

  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#pragma once

#include "../View.h"
#include "../Controller.h"
#include "../../Utils_p.h"

#include <QDebug>
#include <QEvent>
#include <QResizeEvent>
#include <QSizePolicy>
#include <QWidget>

#include <memory>

namespace KDDockWidgets::Views {

template<typename Base>
class DOCKS_EXPORT View_qtwidgets : public Base, public View
{
public:
    using View::height;
    using View::rect;
    using View::width;

    explicit View_qtwidgets(KDDockWidgets::Controller *controller, QWidget *parent = nullptr)
        : Base(parent)
        , View(controller, this)
    {
    }

    ~View_qtwidgets() override = default;

    void free_impl() override
    {
        QObject::deleteLater();
    }

    QSize sizeHint() const override
    {
        return QWidget::sizeHint();
    }

    QSize minSize() const override
    {
        return widgetMinSize(this);
    }

    QSize maxSizeHint() const override
    {
        return widgetMaxSize(this);
    }

    QRect geometry() const override
    {
        return QWidget::geometry();
    }

    void setGeometry(QRect geo) override
    {
        QWidget::setGeometry(geo);
    }

    bool isVisible() const override
    {
        return QWidget::isVisible();
    }

    void setVisible(bool is) override
    {
        QWidget::setVisible(is);
    }

    void move(int x, int y) override
    {
        QWidget::move(x, y);
    }

    void setSize(int width, int height) override
    {
        QWidget::resize(width, height);
    }

    void setWidth(int width) override
    {
        setSize(width, QWidget::height());
    }

    void setHeight(int height) override
    {
        setSize(QWidget::width(), height);
    }

    void show() override
    {
        QWidget::show();
    }

    void hide() override
    {
        QWidget::hide();
    }

    void update() override
    {
        QWidget::update();
    }

    void setParent(View *parent) override
    {
        if (!parent) {
            QWidget::setParent(nullptr);
            return;
        }

        if (auto qwidget = qobject_cast<QWidget *>(parent->asQObject())) {
            QWidget::setParent(qwidget);
        } else {
            qWarning() << Q_FUNC_INFO << "parent is not a widget, you have a bug" << parent->asQObject();
            Q_ASSERT(false);
        }
    }

    void raiseAndActivate() override
    {
        QWidget::window()->raise();
        if (!isWayland())
            QWidget::window()->activateWindow();
    }

    QPoint mapToGlobal(QPoint localPt) const override
    {
        return QWidget::mapToGlobal(localPt);
    }

    void setSizePolicy(QSizePolicy policy) override
    {
        QWidget::setSizePolicy(policy);
    }

protected:
    bool event(QEvent *e) override
    {
        if (e->type() == QEvent::LayoutRequest)
            onLayoutRequest();

        return QWidget::event(e);
    }

    void resizeEvent(QResizeEvent *ev) override
    {
        if (!onResize(ev->size()))
            QWidget::resizeEvent(ev);
    }

private:
    Q_DISABLE_COPY(View_qtwidgets)
};

}