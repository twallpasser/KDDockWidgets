#include "View_qtwidgets.h"
#include "../../Utils_p.h"

#include <QDebug>
#include <QEvent>
#include <QResizeEvent>

#include <memory>

using namespace KDDockWidgets;
using namespace KDDockWidgets::Views;

View_qtwidgets::View_qtwidgets(Controller *controller, QWidget *parent)
    : QWidget(parent)
    , View(controller, this)
{
}

View_qtwidgets::~View_qtwidgets()
{
}

void View_qtwidgets::free_impl()
{
    deleteLater();
}

QSize View_qtwidgets::sizeHint() const
{
    return QWidget::sizeHint();
}

QSize View_qtwidgets::minSize() const
{
    return widgetMinSize(this);
}

QSize View_qtwidgets::maxSizeHint() const
{
    return widgetMaxSize(this);
}

QRect View_qtwidgets::geometry() const
{
    return QWidget::geometry();
}

void View_qtwidgets::setGeometry(QRect geo)
{
    QWidget::setGeometry(geo);
}

bool View_qtwidgets::isVisible() const
{
    return QWidget::isVisible();
}

void View_qtwidgets::setVisible(bool is)
{
    QWidget::setVisible(is);
}

void View_qtwidgets::move(int x, int y)
{
    QWidget::move(x, y);
}

void View_qtwidgets::setSize(int width, int height)
{
    QWidget::resize(width, height);
}

void View_qtwidgets::setWidth(int width)
{
    setSize(width, QWidget::height());
}

void View_qtwidgets::setHeight(int height)
{
    setSize(QWidget::width(), height);
}

void View_qtwidgets::show()
{
    QWidget::show();
}

void View_qtwidgets::hide()
{
    QWidget::hide();
}

void View_qtwidgets::update()
{
    QWidget::update();
}

void View_qtwidgets::setParent(View *parent)
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

std::unique_ptr<View> View_qtwidgets::window() const
{
    if (auto w = QWidget::window())
        return std::unique_ptr<View>(new View_qtwidgets(nullptr, w));

    return {};
}

bool View_qtwidgets::event(QEvent *e)
{
    if (e->type() == QEvent::LayoutRequest)
        onLayoutRequest();

    return QWidget::event(e);
}

void View_qtwidgets::resizeEvent(QResizeEvent *ev)
{
    if (!onResize(ev->size()))
        QWidget::resizeEvent(ev);
}

void View_qtwidgets::raiseAndActivate()
{
    QWidget::window()->raise();
    if (!isWayland())
        QWidget::window()->activateWindow();
}

QPoint View_qtwidgets::mapToGlobal(QPoint localPt) const
{
    return QWidget::mapToGlobal(localPt);
}
