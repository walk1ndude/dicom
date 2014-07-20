#ifndef HEADMODEL_H
#define HEADMODEL_H

#include "Model/AbstractModel.h"

#include "Info/PointsInfo.h"

namespace Model {
    class HeadModel : public AbstractModel {
        Q_OBJECT
    public:
        explicit HeadModel(const ShaderInfo::ShaderFiles & shaderFiles =
                ShaderInfo::ShaderFiles(
                              ShaderInfo::VertexShaderFile(":shaders/sliceVertex.glsl"),
                              ShaderInfo::FramentShaderFile(":shaders/sliceFragment.glsl")
                              ));

        void initShaderVariables();
        void setShaderVariables(ViewPort::ViewPort & viewPort);
        void bindShaderVariablesToBuffers();

        void glStatesEnable();
        void glStatesDisable();

        ModelInfo::ViewAxisRange correctedViewwAxisRange(const ModelInfo::ViewAxisRange & viewAxisRange);

        void init(const int & depth);

        void addPoint(
                const QString & name,
                const PointsInfo::FacePoint & point,
                const ShaderInfo::ShaderVariableName & shaderVariableName
                );

    private:
        ShaderInfo::ShaderVariable _shaderVertex;
        ShaderInfo::ShaderVariable _shaderTexHead;

        ShaderInfo::ShaderVariable _shaderModel;
        ShaderInfo::ShaderVariable _shaderView;
        ShaderInfo::ShaderVariable _shaderProjection;
        ShaderInfo::ShaderVariable _shaderNormalMatrix;
        ShaderInfo::ShaderVariable _shaderScale;
        ShaderInfo::ShaderVariable _shaderStep;

        PointsInfo::FacePoints _facePoints;
        PointsInfo::FacePointsProgram _facePointsProgram;

        GLfloat _step;
    };
}
#endif // HEADMODEL_H
