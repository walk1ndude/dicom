#ifndef MODELSCENE_H
#define MODELSCENE_H

#include "Scene/AbstractScene.h"

#include "Info/LightInfo.h"
#include "Info/MaterialInfo.h"
#include "Info/SliceInfo.h"
#include "Info/TextureInfo.h"
#include "Info/PointsInfo.h"

//#include "Model/AbstractModel.h"
#include "Model/StlModel.h"

#include "ViewPort/ViewPortArray.h"

namespace Scene {
    class ModelScene : public AbstractScene {
        Q_OBJECT
    public:
        explicit ModelScene();
        ~ModelScene();

        void initScene(const QSize & surfaceSize);
        void renderScene(const QSize & surfaceSize);

        bool isEmpty();

        void cleanUp();

        QVector3D rotation();

        QRect screenSaveRect();

        // some constants here
        void initMaterials();
        void initLightSources();

        void addMaterial(const MaterialInfo::Emissive & emissive,
                         const MaterialInfo::Diffuse & diffuse,
                         const MaterialInfo::Specular & specular,
                         const MaterialInfo::Shininess & shininess);

        void addLightSource(const LightInfo::Position & position,
                            const LightInfo::Color & color,
                            const LightInfo::AmbientIntensity & ambientIntensity);

        void addTexture(TextureInfo::Texture & textureInfo);

        void setRotation(const QVector3D & rotation);

        // zoom the whole scene
        void setZoomFactor(const qreal & zoomFactor);

        // to clip selected model
        void setXRange(const ModelInfo::ViewAxisRange & xRange);
        void setYRange(const ModelInfo::ViewAxisRange & yRange);
        void setZRange(const ModelInfo::ViewAxisRange & zRange);

        void addPoint(const PointsInfo::Point & point);

    private:
        QMap<QOpenGLTexture *, TextureInfo::Texture> _texturesInfo;
        QList<Model::AbstractModel *> _models;

        QMultiMap<Model::AbstractModel *, QOpenGLTexture *> _texturesInModel;
        QMultiMap<Model::AbstractModel *, MaterialInfo::Material *> _materialsInModel;

        QList<MaterialInfo::Material *> _materials;
        QList<LightInfo::LightSource *> _lightSources;
        QList<QOpenGLTexture *> _textures;

        Model::AbstractModel * _selectedModel;
        QOpenGLTexture * _selectedTexture;

        ViewPort::ViewPortArray _viewPorts;

        QVector3D _rotation;

        ViewPort::ViewPortRect _screenSaveRect;

        PointsInfo::FacePoints _facePoints;

        void initializeViewPorts(const QSize & surfaceSize);

    public slots:
        void addStlModel(ModelInfo::BuffersVN buffers);
        void addHeadModel(SliceInfo::Slices slices);
    };

}

#endif // MODELSCENE_H
