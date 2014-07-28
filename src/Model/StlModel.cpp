#include "Model/StlModel.h"

namespace Model {
    StlModel::StlModel(PointsModel * points, AbstractModel * parent, const ShaderInfo::ShaderFiles & shaderFiles) :
        AbstractModelWithPoints(points, parent, shaderFiles) {
            points->setParent(this);
    }

    void StlModel::initShaderVariables(QOpenGLShaderProgram * program) {
        _shaderVertex = program->attributeLocation("vertex");
        _shaderNormal = program->attributeLocation("normal");

        _shaderColorU = program->uniformLocation("colorU");
        _shaderMPV = program->uniformLocation("mvp");
        _shaderNormalMatrix = program->uniformLocation("normalMatrix");
    }

    void StlModel::glStatesEnable() {
        glEnable(GL_CULL_FACE);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
    }

    void StlModel::glStatesDisable() {
        glDisable(GL_CULL_FACE);
        glDisable(GL_BLEND);
        glDisable(GL_DEPTH_TEST);
    }

    ModelInfo::ViewAxisRange StlModel::correctedViewwAxisRange(const ModelInfo::ViewAxisRange & viewAxisRange) {
        return viewAxisRange;
    }

    void StlModel::bindShaderVariablesToBuffers(QOpenGLShaderProgram * program) {
        program->enableAttributeArray(_shaderVertex);
        program->setAttributeBuffer(_shaderVertex, GL_FLOAT, 0, 3, stride());

        program->enableAttributeArray(_shaderNormal);
        program->setAttributeBuffer(_shaderNormal, GL_FLOAT, sizeof(GLfloat) * 3, 3, stride());
    }

    void StlModel::setShaderVariables(QOpenGLShaderProgram * program, ViewPort::ViewPort & viewPort) {
        program->setUniformValue(_shaderColorU, QVector4D(1.0, 1.0, 1.0, 1.0));
        program->setUniformValue(_shaderMPV, viewPort.projection() * viewPort.view() * model());
        program->setUniformValue(_shaderNormalMatrix, (model() * viewPort.view()).normalMatrix());
    }
}
