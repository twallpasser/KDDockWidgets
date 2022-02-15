#include "View_qtwidgets.h"
#include "../../Utils_p.h"

#include <QDebug>
#include <QEvent>
#include <QResizeEvent>

#include <memory>

using namespace KDDockWidgets;
using namespace KDDockWidgets::Views;

View_qtwidgets::View_qtwidgets(QWidget *parent)
    : QWidget(parent)
    , View(this)
    , m_widget(this) // TODO: Remove
{
}

View_qtwidgets::~View_qtwidgets()
{
}

QWidget *View_qtwidgets::widget() const
{
    return m_widget;
}

void View_qtwidgets::free()
{
    m_widget->deleteLater();
    delete this;
}

QSize View_qtwidgets::sizeHint() const
{
    return m_widget->sizeHint();
}

QSize View_qtwidgets::minSize() const
{
    return widgetMinSize(m_widget);
}

QSize View_qtwidgets::maxSizeHint() const
{
    return widgetMaxSize(m_widget);
}

QRect View_qtwidgets::geometry() const
{
    return m_widget->geometry();
}

void View_qtwidgets::setGeometry(QRect geo)
{
    m_widget->setGeometry(geo);
}

bool View_qtwidgets::isVisible() const
{
    return m_widget->isVisible();
}

void View_qtwidgets::setVisible(bool is) const
{
    m_widget->setVisible(is);
}

void View_qtwidgets::move(int x, int y)
{
    m_widget->move(x, y);
}

void View_qtwidgets::setSize(int width, int height)
{
    m_widget->resize(width, height);
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
    m_widget->show();
}

void View_qtwidgets::hide()
{
    m_widget->hide();
}

void View_qtwidgets::update()
{
    m_widget->update();
}

void View_qtwidgets::setParent(View *parent)
{
    if (!parent) {
        m_widget->setParent(nullptr);
        return;
    }

    if (auto qwidget = qobject_cast<QWidget *>(parent->asQObject())) {
        m_widget->setParent(qwidget);
    } else {
        qWarning() << Q_FUNC_INFO << "parent is not a widget, you have a bug" << parent->asQObject();
        Q_ASSERT(false);
    }
}

std::unique_ptr<View> View_qtwidgets::window() const
{
    if (auto w = m_widget->window())
        return std::unique_ptr<View>(new View_qtwidgets(w));

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
