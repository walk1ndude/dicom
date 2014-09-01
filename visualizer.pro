#-------------------------------------------------
#
# Project created by QtCreator 2014-01-24T14:23:02
#
#-------------------------------------------------

greaterThan(QT_VERSION, 5.2): QT += quick

TARGET = visualizer
TEMPLATE = app

CONFIG += c++11 qtquickcompiler

INCLUDEPATH += include

unix:macx {

    QMAKE_MAC_SDK = macosx10.10

    INCLUDEPATH += /usr/local/include/gdcm-2.4 \
                   /usr/local/include

    LIBS += -L/usr/local/lib -lgdcmCommon \
                            -lgdcmDICT \
                            -lgdcmMSFF \
                            -lgdcmDSED \
                            -lgdcmIOD \
                            -lopencv_core \
                            -lopencv_imgproc \
                            -lopencv_highgui

    LIBS += -framework OpenCL
}

unix:!macx {
    INCLUDEPATH += /usr/include/gdcm-2.4

    LIBS += -lopencv_core \
            -lopencv_imgproc \
            -lopencv_highgui

    LIBS += -lgdcmCommon \
            -lgdcmDICT \
            -lgdcmMSFF \
            -lgdcmDSED \
            -lgdcmIOD

    LIBS += -lOpenCL
}

win32 {
    INCLUDEPATH += "C:\Program Files\OpenCV_249\build\include" \
                   C:\GDCM\gdcm\include

    !contains(QMAKE_HOST.arch, x86_64) {
            QMAKE_LFLAGS *= /MACHINE:X86
    }
    else {
        contains(QMAKE_HOST.arch, x86_64):{
            QMAKE_LFLAGS *= /MACHINE:X64
        }
    }

    LIBS += -L"C:\GDCM\gdcm\bin\MinSizeRel" \
            -L"C:\Program Files\OpenCV_249\build\x86\vc12\lib"

    LIBS += -lopencv_core249 \
            -lopencv_highgui249 \
            -lopencv_imgproc249

    LIBS += -lgdcmCommon \
            -lgdcmDICT \
            -lgdcmMSFF \
            -lgdcmDSED \
            -lgdcmIOD

    LIBS += -lOpenCL
}

SOURCES +=  src/main.cpp \
            src/Gui/AppWindow.cpp \
            src/Quick/ModelViewer.cpp \
            src/Parser/DicomReader.cpp \
            src/Parser/Reconstructor.cpp \
            src/Parser/StlReader.cpp \
            src/Render/AbstractRenderer.cpp \
            src/Render/ModelRenderer.cpp \
            src/Model/AbstractModel.cpp \
            src/Model/HeadModel.cpp \
            src/Model/StlModel.cpp \
            src/Scene/ModelScene.cpp \
            src/Model/PointsModel.cpp \
            src/Model/AbstractModelWithPoints.cpp \
            src/Scene/AbstractScene.cpp \
            src/Viewport/Viewport.cpp \
            src/Viewport/ViewportArray.cpp \
    src/Quick/ConsoleLogger.cpp

HEADERS  += include/Gui/AppWindow.h \
            include/Quick/ModelViewer.h \
            include/Parser/ctprocessing.hpp \
            include/Parser/parallelprocessing.hpp \
            include/Parser/DicomReader.h \
            include/Parser/Reconstructor.h \
            include/Parser/StlReader.h \
            include/Render/AbstractRenderer.h \
            include/Render/ModelRenderer.h \
            include/Model/AbstractModel.h \
            include/Model/HeadModel.h \
            include/Model/StlModel.h \
            include/Info/SliceInfo.h \
            include/Info/ModelInfo.h \
            include/Info/MaterialInfo.h \
            include/Info/LightInfo.h \
            include/Info/Info.h \
            include/Info/TextureInfo.h \
            include/Info/ShaderInfo.h \
            include/Scene/AbstractScene.h \
            include/Scene/ModelScene.h \
            include/Parser/Helpers.hpp \
            include/Info/PointsInfo.h \
            include/Model/PointsModel.h \
            include/Model/AbstractModelWithPoints.h \
            include/Viewport/Viewport.h \
            include/Viewport/ViewportArray.h \
    include/Quick/TextureNode.h \
    include/Quick/ConsoleLogger.h

RESOURCES += resources.qrc