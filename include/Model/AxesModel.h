#ifndef AXESMODEL_H
#define AXESMODEL_H

#include "Model/AbstractModel.h"

namespace Model {
    class AxesModel : public AbstractModel {
        Q_OBJECT
    public:
        explicit AxesModel(Scene::AbstractScene * scene,
                           const ShaderInfo::ShaderFiles & shaderFiles =
                           ShaderInfo::ShaderFiles(
                                                   ShaderInfo::VertexShaderFiles() << ShaderInfo::VertexShaderFile(":shaders/Axes/vertex.glsl"),
                                                   ShaderInfo::FragmentShaderFiles() << ShaderInfo::FragmentShaderFile(":shaders/Axes/fragment.glsl")
                                                     ),

                             const ShaderInfo::ShaderVariablesNames & attributeArrays =
                             ShaderInfo::ShaderVariablesNames() << "vertex" << "color",

                             const ShaderInfo::ShaderVariablesNames & uniformValues =
                             ShaderInfo::ShaderVariablesNames() << "mvp" << "parentTraslate");

        virtual void init(const ModelInfo::Params & params);

        virtual void setColors(const QVector<QColor> & axesColors);

        virtual void setLenght(const GLfloat & lenght = 1.5f);

    protected:
        virtual void bindAttributeArrays(QOpenGLShaderProgram * program) const;
        virtual void bindUniformValues(QOpenGLShaderProgram * program, const Viewport::Viewport * viewport) const;

        virtual void drawingRoutine() const;

    private:
        QVector<QColor> _axesColors;

        GLfloat _lenght;
    };
}

#endif // AXESMODEL_H
