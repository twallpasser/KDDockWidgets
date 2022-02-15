/*
  This file is part of KDDockWidgets.

  SPDX-FileCopyrightText: 2020-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Sérgio Martins <sergio.martins@kdab.com>

  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#pragma once

#include "../View.h"

#include <QWidget>
#include <qglobal.h>

namespace KDDockWidgets::Views {

class DOCKS_EXPORT View_qtwidgets : public QWidget, public View
{
public:
    using View::setVisible;

    explicit View_qtwidgets(QWidget *parent = nullptr);
    ~View_qtwidgets() override;

    void free() override;
    QSize sizeHint() const override;
    QSize minSize() const override;
    QSize maxSizeHint() const override;
    QRect geometry() const override;
    void setGeometry(QRect) override;
    bool isVisible() const override;
    void setVisible(bool) override;
    void move(int x, int y) override;
    void setSize(int width, int height) override;
    void setWidth(int width) override;
    void setHeight(int height) override;
    void show() override;
    void hide() override;
    void update() override;
    void setParent(View *) override;
    std::unique_ptr<View> window() const override;
    void raiseAndActivate() override;
    QPoint mapToGlobal(QPoint) const override;

protected:
    bool event(QEvent *) override;
    void resizeEvent(QResizeEvent *) override;

private:
    Q_DISABLE_COPY(View_qtwidgets)
};

}