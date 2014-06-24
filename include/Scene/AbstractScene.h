#ifndef ABSTRACTSCENE_H
#define ABSTRACTSCENE_H

#include <QtCore/QSize>

#include <Info/ModelInfo.h>

namespace Scene {
    class AbstractScene {
    public:
        AbstractScene() :
            _isInitialized(false) {}

        virtual void initScene(const QSize & surfaceSize) = 0;
        virtual void renderScene(const QSize & surfaceSize) = 0;

        virtual bool isEmpty() = 0;
        virtual bool isInitialized() final { return _isInitialized; }

        virtual void cleanUp() = 0;

        virtual void setRotation(const QVector3D & rotation) = 0;

        virtual QVector3D rotation() = 0;

        virtual QRect screenSaveRect() = 0;

        // zoom the whole scene
        virtual void setZoomFactor(const qreal & zoomFactor) = 0;

        // to clip selected model
        virtual void setXRange(const ModelInfo::ViewAxisRange & xRange) = 0;
        virtual void setYRange(const ModelInfo::ViewAxisRange & yRange) = 0;
        virtual void setZRange(const ModelInfo::ViewAxisRange & zRange) = 0;
    private:
        bool _isInitialized;
    };
}

#endif // ABSTRACTSCENE_H