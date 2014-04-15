#-------------------------------------------------
#
# Project created by QtCreator 2014-01-24T14:23:02
#
#-------------------------------------------------

greaterThan(QT_MAJOR_VERSION, 4): QT += quick

TARGET = dicom
TEMPLATE = app

CONFIG += c++11

INCLUDEPATH += include

unix {
    INCLUDEPATH += /usr/include/gdcm-2.4

    CONFIG += link_pkgconfig
    PKGCONFIG += opencv
}

win32 {
    INCLUDEPATH += C:\opencv\build\include \
                   C:\GDCM\gdcm\include

    !contains(QMAKE_HOST.arch, x86_64) {
            QMAKE_LFLAGS *= /MACHINE:X86
            LIBS += -L"C:\opencv\build\x86\vc12\lib"
    }
    else {
        contains(QMAKE_HOST.arch, x86_64):{
            QMAKE_LFLAGS *= /MACHINE:X64
            LIBS += -L"C:\opencv\build\x64\vc12\lib"
        }
    }

    LIBS += -L"C:\GDCM\gdcm\bin\Debug"

    LIBS += -lopencv_core248 \
            -lopencv_highgui248 \
            -lopencv_imgproc248 \
            -lopencv_ocl248
}

LIBS += -lgdcmcharls \
        -lgdcmjpeg12 \
        -lgdcmCommon \
        -lgdcmjpeg16 \
        -lgdcmDICT \
        -lgdcmjpeg8 \
        -lgdcmDSED \
        -lgdcmMSFF \
        -lgdcmIOD \
        -lgdcmopenjpeg

SOURCES += src/main.cpp\
           src/dicomreader.cpp\
           src/glheadmodel.cpp \
           src/matrixstack.cpp \
           src/openglitem.cpp \
           src/sliceviewer.cpp

HEADERS  += include/dicomreader.h \
            include/ctprocessing.hpp \
            include/glheadmodel.h \
            include/matrixstack.h \
            include/openglitem.h \
            include/sliceviewer.h

RESOURCES += resources.qrc
