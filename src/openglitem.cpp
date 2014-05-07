#include <QtQuick/QQuickWindow>
#include <QtQuick/QSGSimpleTextureNode>

#include "openglitem.h"

OpenGLItem::OpenGLItem() :
    _needsInitialize(false),
    _isTextureUpdated(true),
    _screenSaveRect(QRectF(0, 0, width(), height())),
    _context(0),
    _fbo(0),
    _takeShot(false) {

    setFlag(QQuickItem::ItemHasContents);
    QObject::connect(this, &OpenGLItem::windowChanged, this, &OpenGLItem::handleWindowChanged, Qt::DirectConnection);

    _cleanUpHelper.create();
}

void OpenGLItem::handleWindowChanged(QQuickWindow * window) {
    if (window) {
        window->setClearBeforeRendering(false);
        initializeViewPorts();
    }
}

OpenGLItem::~OpenGLItem() {
    if (_fbo) {
        delete _fbo;
    }
}

bool OpenGLItem::takeShot() {
    return _takeShot;
}

void OpenGLItem::setTakeShot(const bool & takeShot) {
    _takeShot = takeShot;
}

QSGNode * OpenGLItem::updatePaintNode(QSGNode * node, UpdatePaintNodeData *) {
    if (_needsInitialize) {
        _context = new QOpenGLContext;

        QSurfaceFormat format;
        format.setVersion(4, 1);
        format.setRenderableType(QSurfaceFormat::OpenGL);
        format.setProfile(QSurfaceFormat::CoreProfile);
        format.setSwapBehavior(QSurfaceFormat::TripleBuffer);

        _context->setFormat(format);
        _context->create();

        _savedContext = window()->openglContext();
        _context->makeCurrent(window());

        QObject::connect(window(), SIGNAL(closing(QQuickCloseEvent *)), _context, SIGNAL(aboutToBeDestroyed()));
        QObject::connect(_context, &QOpenGLContext::aboutToBeDestroyed, [=]() { this->cleaningUp(); });

        initializeOpenGLFunctions();
        initialize();

        _needsInitialize = false;

        emit initialized();

        _savedContext->makeCurrent(window());
    }

    QSGSimpleTextureNode * texNode = static_cast<QSGSimpleTextureNode *>(node);

    if (!texNode) {
        texNode = new QSGSimpleTextureNode;
    }

    if (_context) {
        const GLsizei viewportWidth = width() * window()->devicePixelRatio();
        const GLsizei viewportHeight = height() * window()->devicePixelRatio();

        if (_fbo) {
            delete _fbo;
            _fbo = 0;
        }

        QOpenGLFramebufferObjectFormat format;
        format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
        format.setMipmap(true);
        format.setInternalTextureFormat(GL_RGBA);

        _fbo = new QOpenGLFramebufferObject(QSize(viewportWidth, viewportHeight), format);

        _fbo->bind();

        _context->makeCurrent(window());

        if (!_isTextureUpdated) {
            initializeTextures();
            _isTextureUpdated = true;
        }

        render();

        if (_takeShot) {
            _fbo->toImage().copy(
                        _screenSaveRect.x(),
                        _screenSaveRect.y(),
                        _screenSaveRect.width(),
                        _screenSaveRect.height()
                        ).save((_rotation.y() > 99 ? "" : (_rotation.y() > 9 ? "0" : "00")) + QString::number(_rotation.y()) + ".png",
                               "png");
        }
        else {
            _context->swapBuffers(window());
        }

        _savedContext->makeCurrent(window());

        _fbo->bindDefault();

        texNode->setTexture(window()->createTextureFromId(_fbo->texture(), _fbo->size()));
        texNode->setRect(boundingRect());
    }
    else {
        texNode->setTexture(window()->createTextureFromId(0, QSize(0, 0)));
    }

    return texNode;
}

void OpenGLItem::initialize() {

}

void OpenGLItem::initializeTextures() {

}

void OpenGLItem::initializeViewPorts() {

}

void OpenGLItem::render() {

}

void OpenGLItem::cleanup() {

}

void OpenGLItem::cleaningUp() {
    _context->makeCurrent(&_cleanUpHelper);
    cleanup();

    _savedContext->doneCurrent();
}
