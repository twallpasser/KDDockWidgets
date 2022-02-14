#include "View_qtwidgets.h"

#include <QDebug>

using namespace KDDockWidgets;
using namespace KDDockWidgets::Views;

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

View_qtwidgets::View_qtwidgets(QWidget *widget)
    : View(widget)
    , m_widget(widget)
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
    setSize(width, height());
}

void View_qtwidgets::setHeight(int height)
{
    setSize(width(), height);
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