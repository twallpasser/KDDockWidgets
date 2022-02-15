#include "Separator_qtwidgets.h"
/*
  This file is part of KDDockWidgets.

  SPDX-FileCopyrightText: 2019-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Sérgio Martins <sergio.martins@kdab.com>

  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "View_qtwidgets.h"

#include "../Logging_p.h"
#include "Config.h"
#include "../controllers/Separator.h"

#include <QMouseEvent>
#include <QPainter>
#include <QStyleOption>

using namespace KDDockWidgets::Views;

Separator_qtwidgets::Separator_qtwidgets(Controllers::Separator *controller, QWidget *parent)
    : View_qtwidgets(controller, parent)
    , m_controller(controller)
{
    setMouseTracking(true);
}

void Separator_qtwidgets::paintEvent(QPaintEvent *ev)
{
    if (KDDockWidgets::Config::self().disabledPaintEvents() & KDDockWidgets::Config::CustomizableWidget_Separator) {
        QWidget::paintEvent(ev);
        return;
    }

    QPainter p(this);

    QStyleOption opt;
    opt.palette = palette();
    opt.rect = QWidget::rect();
    opt.state = QStyle::State_None;
    if (!m_controller->isVertical())
        opt.state |= QStyle::State_Horizontal;

    if (isEnabled())
        opt.state |= QStyle::State_Enabled;

    QWidget::parentWidget()->style()->drawControl(QStyle::CE_Splitter, &opt, &p, this);
}

void Separator_qtwidgets::enterEvent(KDDockWidgets::Qt5Qt6Compat::QEnterEvent *)
{
    qCDebug(separators) << Q_FUNC_INFO << this;
    if (m_controller->isVertical())
        setCursor(Qt::SizeVerCursor);
    else
        setCursor(Qt::SizeHorCursor);
}

void Separator_qtwidgets::leaveEvent(QEvent *)
{
    setCursor(Qt::ArrowCursor);
}

void Separator_qtwidgets::mousePressEvent(QMouseEvent *)
{
    m_controller->onMousePress();
}

void Separator_qtwidgets::mouseMoveEvent(QMouseEvent *ev)
{
    m_controller->onMouseMove(mapToParent(ev->pos()));
}

void Separator_qtwidgets::mouseReleaseEvent(QMouseEvent *)
{
    m_controller->onMouseReleased();
}

void Separator_qtwidgets::mouseDoubleClickEvent(QMouseEvent *)
{
    m_controller->onMouseDoubleClick();
}
