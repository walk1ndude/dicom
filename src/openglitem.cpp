#include <QtQuick/QQuickWindow>
#include <QtQuick/QSGSimpleTextureNode>

#include <QtGui/QOpenGLFramebufferObject>

#include "openglitem.h"

OpenGLItem::OpenGLItem() :
    _context(0),
    _needsInitialize(false),
    _isTextureUpdated(true),
    _fbo(0) {

    setFlag(QQuickItem::ItemHasContents);
}

OpenGLItem::~OpenGLItem() {

}

QSGNode * OpenGLItem::updatePaintNode(QSGNode * node, UpdatePaintNodeData * ) {
    QOpenGLContext * savedQt = window()->openglContext();

    if (_needsInitialize) {
        window()->setClearBeforeRendering(false);

        _context = new QOpenGLContext;

        QSurfaceFormat format;
        format.setVersion(3, 3);
        format.setSamples(16);
        format.setRenderableType(QSurfaceFormat::OpenGL);
        format.setProfile(QSurfaceFormat::CoreProfile);
        format.setSwapBehavior(QSurfaceFormat::TripleBuffer);

        _context->setFormat(format);
        _context->create();

        QObject::connect(_context, &QOpenGLContext::aboutToBeDestroyed, this, &OpenGLItem::cleanup, Qt::DirectConnection);

        _context->makeCurrent(window());

        initializeOpenGLFunctions();
        initialize();

        _needsInitialize = false;

        emit initialized();

        savedQt->makeCurrent(window());
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

        _fbo = new QOpenGLFramebufferObject(viewportWidth, viewportHeight);
        _fbo->bind();

        _context->makeCurrent(window());

        if (!_isTextureUpdated) {
            initializeTextures();
            _isTextureUpdated = true;
        }

        render();

        savedQt->makeCurrent(window());

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

void OpenGLItem::render() {

}

void OpenGLItem::sync() {

}

void OpenGLItem::cleanup() {

}
