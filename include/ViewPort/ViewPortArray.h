#ifndef VIEWPORTS_H
#define VIEWPORTS_H

#include "ViewPort/ViewPort.h"

namespace ViewPort {
    using ViewPortInfo = QPair<QRectF, ViewPort::ProjectionType>;
    using ViewPorts = QVector<ViewPortInfo>;
    using ViewPortsIterator = QMutableVectorIterator<ViewPort>;

    class ViewPortArray : public QVector<ViewPort> {
    public:
        explicit ViewPortArray(const ViewPorts & viewPorts = ViewPorts(), const QSize & windowSize = QSize());

        void setViewPorts(const ViewPorts & viewPorts, const QSize & windowSize);

        void zoom(const qreal & zoomFactor);

        void resize(const QSize & surfaceSize);
    };
}

#endif // VIEWPORTS_H
