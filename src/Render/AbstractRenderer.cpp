#include <QtGui/QGuiApplication>
#include <QtGui/QImage>

#include <cmath>

#include "Render/AbstractRenderer.h"

namespace Render {
    AbstractRenderer::AbstractRenderer(QOpenGLContext * context, const QSize & surfaceSize) :
        _canRenderContent(false),
        _textureUpdateNeeded(false),
        _contentInitializeNeeded(false),
        _surfaceSize(surfaceSize),
        _currectScene(nullptr),
        _fboRender(nullptr),
        _fboDisplay(nullptr) {

        _context = new QOpenGLContext;

        QSurfaceFormat surfaceFormat = context->format();
        _context->setFormat(surfaceFormat);

        _context->setShareContext(context);

        _context->create();
        _context->moveToThread(this);

        _surface = new QOffscreenSurface;
        _surface->setFormat(surfaceFormat);
        _surface->create();

        QObject::connect(this, &AbstractRenderer::redraw, this, &AbstractRenderer::renderNext);
    }

    AbstractRenderer::~AbstractRenderer() {

    }

    bool AbstractRenderer::updateContent() {
        if (_canRenderContent) {
            _textureUpdateNeeded = true;
            return true;
        }
        else {
            _canRenderContent = true;
            _contentInitializeNeeded = true;
            return false;
        }
    }

    bool AbstractRenderer::activateContext() {
        return _context->makeCurrent(_surface);
    }

    void AbstractRenderer::setSurface(QOffscreenSurface * surface) {
        _surface = surface;
    }

    void AbstractRenderer::selectScene(Scene::AbstractScene * scene) {
        QMutexLocker locker(&renderMutex);

        // no more connection with previous scene
        if (_currectScene) {
            disconnectWithScene(_currectScene);
        }

        _currectScene = scene;
        connectWithScene(_currectScene);

        // for cleanup reasons mostly
        _sceneHistory.insert(scene);
    }

    void AbstractRenderer::connectWithScene(Scene::AbstractScene * scene) {
        QObject::connect(scene, &Scene::AbstractScene::redraw, this, &Render::AbstractRenderer::render);
        QObject::connect(scene, &Scene::AbstractScene::post, this, &Render::AbstractRenderer::post);
    }

    void AbstractRenderer::disconnectWithScene(Scene::AbstractScene * scene) {
        QObject::disconnect(scene, &Scene::AbstractScene::redraw, this, &Render::AbstractRenderer::render);
        QObject::disconnect(scene, &Scene::AbstractScene::post, this, &Render::AbstractRenderer::post);
    }

    Scene::AbstractScene * AbstractRenderer::currentScene() const {
        return _currectScene;
    }

    QSize AbstractRenderer::surfaceSize() const {
        return _surfaceSize;
    }

    void AbstractRenderer::setSurfaceSize(const QSize & surfaceSize) {
        _surfaceSize = surfaceSize;
    }

    void AbstractRenderer::renderNext() {
        QMutexLocker locker(&renderMutex);
        activateContext();

        if (!_fboRender) {
            QOpenGLFramebufferObjectFormat format;
            format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
            format.setInternalTextureFormat(GL_RGBA16);
            _fboRender = new QOpenGLFramebufferObject(_surfaceSize, format);
            _fboDisplay = new QOpenGLFramebufferObject(_surfaceSize, format);
        }

        _fboRender->bind();

        render();

        glFinish();

        _fboRender->bindDefault();
        std::swap(_fboDisplay, _fboRender);

        emit textureReady(_fboDisplay->texture(), _surfaceSize);
    }

    void AbstractRenderer::shutDown() {
        _context->makeCurrent(_surface);

        if (_fboRender) {
            delete _fboRender;
        }

        if (_fboDisplay) {
            delete _fboDisplay;
        }

        cleanUp();

        delete _context;

        _surface->deleteLater();

        exit();
        moveToThread(QGuiApplication::instance()->thread());
    }

    void AbstractRenderer::cleanUp() {
        QMutexLocker locker(&renderMutex);

        QObject::disconnect(this, &AbstractRenderer::redraw, this, &AbstractRenderer::renderNext);

        disconnectWithScene(currentScene());

        activateContext();

        for (Scene::AbstractScene * scene : _sceneHistory) {
            scene->cleanUp();
        }
    }
}
