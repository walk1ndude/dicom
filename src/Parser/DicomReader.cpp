#include <gdcmReader.h>
#include <gdcmImageReader.h>
#include <gdcmAttribute.h>
#include <gdcmStringFilter.h>
#include <gdcmException.h>

#include <QtCore/QUrl>

#include <opencv2/highgui/highgui.hpp>

#include "Parser/DicomReader.h"
#include "Parser/Helpers.hpp"

#define MIN_HU 200
#define MAX_HU 1500

namespace Parser {
    DicomReader::DicomReader() :
        AbstractParser()
    {

    }

    void DicomReader::fetchDicomData(DicomData & dicomData, gdcm::File & dFile, const gdcm::Image & dImage) {
        gdcm::StringFilter dStringFilter;
        dStringFilter.SetFile(dFile);

        gdcm::DataSet & dDataSet = dFile.GetDataSet();

        dicomData.imageSpacings.setX(dImage.GetSpacing(0));
        dicomData.imageSpacings.setY(dImage.GetSpacing(1));

        gdcm::Tag tagFind(0x0018, 0x0050);
        if (dDataSet.FindDataElement(tagFind)) {
            dicomData.imageSpacings.setZ(std::stof(dStringFilter.ToString(tagFind)));
        }
        else {
            //pure assumtion for now
            dicomData.imageSpacings.setZ(1.0);
        }

        tagFind.SetElementTag(0x0018, 0x0088);
        if (dDataSet.FindDataElement(tagFind)) {
            dicomData.imageSpacings[2] += std::stof(dStringFilter.ToString(tagFind));
        }
        else {
            //pure assumtion for now
            //dicomData.imageSpacings[2] += 0.1;
        }

        tagFind.SetElementTag(0x0028, 0x1050);
        if (dDataSet.FindDataElement(tagFind)) {
            dicomData.windowCenter = std::stoi(dStringFilter.ToString(tagFind));
        }
        else {
            dicomData.windowCenter = 0;
        }

        tagFind.SetElementTag(0x0028, 0x1051);
        if (dDataSet.FindDataElement(tagFind)) {
            dicomData.windowWidth = std::stoi(dStringFilter.ToString(tagFind));
        }
        else {
            dicomData.windowWidth = 0;
        }

        dicomData.width = dImage.GetDimension(0);
        dicomData.height = dImage.GetDimension(1);
        dicomData.depth = dImage.GetDimension(2);

        dicomData.vbuffer.resize(dImage.GetBufferLength());
        dicomData.buffer = &(dicomData.vbuffer[0]);
        dImage.GetBuffer(dicomData.buffer);

        //MONOCHROME2

        gdcm::PhotometricInterpretation photometricInterpretation = dImage.GetPhotometricInterpretation();

        dicomData.inverseNeeded = (photometricInterpretation != gdcm::PhotometricInterpretation::MONOCHROME2);

        gdcm::PixelFormat pixelFormat = dImage.GetPixelFormat();
        if (pixelFormat.GetScalarType() == gdcm::PixelFormat::UINT16) {
            dicomData.type = CV_16UC1;
        }
        else {
            dicomData.type = CV_8UC1; // for now
        }

        dicomData.bytesAllocated = pixelFormat.GetBitsAllocated() / 8;

        try {
            dicomData.slope = dImage.GetSlope();
            dicomData.intercept = dImage.GetIntercept();
        }
        catch (gdcm::Exception &) {
            dicomData.slope = 1.0f;
            dicomData.intercept = 0.0f;
        }

        dicomData.minValue = (int) pixelFormat.GetMin();
        dicomData.maxValue = (int) pixelFormat.GetMax();

        dicomData.minHUPossible = dicomData.slope * dicomData.minValue + dicomData.intercept;
        dicomData.maxHUPossible = dicomData.slope * dicomData.maxValue + dicomData.intercept;

        dicomData.isLittleEndian = (pixelFormat.GetBitsAllocated() - pixelFormat.GetHighBit() == 1);

        dicomData.neighbourRadius = 0;

        dicomData.minHU = std::max(dicomData.minHUPossible, MIN_HU);
        dicomData.maxHU = std::min(dicomData.maxHUPossible, MAX_HU);
    }

    void DicomReader::readImage(gdcm::File & dFile, const gdcm::Image & dImage) {
        fetchDicomData(_dicomData, dFile, dImage);

        runSliceProcessing(true);
    }

    void DicomReader::runSliceProcessing(const bool & tellAboutHURange) {
        TextureInfo::MergedDataPtr mergedData = nullptr;

        QOpenGLPixelTransferOptions pixelTransferOptions;

        _dicomData.pixelTransferOptions = &pixelTransferOptions;
        _dicomData.mergeLocation = &mergedData;

        float startTime = cv::getTickCount() / cv::getTickFrequency();

        cv::parallel_for_(cv::Range(0, (int) _dicomData.depth), SliceProcessing(&_dicomData));

        qDebug() << "Elapsed Time: " << cv::getTickCount() / cv::getTickFrequency() - startTime;

        size_t depth = _dicomData.depth - _dicomData.neighbourRadius * 2;

        VolumeInfo::Scaling scaling = scaleVector<float, QVector3D>(
                    _dicomData.width * _dicomData.imageSpacings.x(),
                    _dicomData.height * _dicomData.imageSpacings.y(),
                    depth * _dicomData.imageSpacings.z()
                    ) *
                    QVector3D(
                        1.0f / _dicomData.imageSpacings.x(),
                        1.0f / _dicomData.imageSpacings.y(),
                        1.0f / _dicomData.imageSpacings.z()
                        );

        scaling.setZ(scaling.x());

        TextureInfo::TextureInfo texture;
        texture.mergedData = TextureInfo::MergedDataPointer(mergedData);

        texture.pixelTransferOptions = pixelTransferOptions;

        texture.size = TextureInfo::Size(_dicomData.width, _dicomData.height, depth);

        texture.pixelType = QOpenGLTexture::UInt16;
        texture.textureFormat = QOpenGLTexture::R16U;
        texture.pixelFormat = QOpenGLTexture::Red_Integer;
        texture.target = QOpenGLTexture::Target3D;

        QVariantMap blueprintOverallMap = _blueprint.toMap();
        QVariantList textureVolumeList = blueprintOverallMap["textures"].toList();
        QVariantMap textureVolume = textureVolumeList[0].toMap();

        textureVolume["desciptor"] = QVariant::fromValue(texture);

        textureVolumeList[0] = QVariant(textureVolume);

        blueprintOverallMap["textures"] = QVariant(textureVolumeList);

        QVariantList blueprintList = blueprintOverallMap["models"].toList();

        QVariantMap blueprintMap = blueprintList[0].toMap();
        QVariantMap blueprintParams = blueprintMap["params"].toMap();

        blueprintParams["size"] = QVariant(texture.size);
        blueprintParams["scaling"] = QVariant(scaling);
        blueprintParams["slope"] = QVariant(_dicomData.slope);
        blueprintParams["intercept"] = QVariant(_dicomData.intercept);
        blueprintParams["windowWidth"] = QVariant(_dicomData.windowWidth);
        blueprintParams["windowCenter"] = QVariant(_dicomData.windowCenter);
        blueprintParams["huRange"] = QVariant(VolumeInfo::HuRange(_dicomData.minHU, _dicomData.maxHU));
        blueprintParams["valueRange"] = QVariant(VolumeInfo::ValueRange(_dicomData.minValue, _dicomData.maxValue));
        blueprintParams["physicalSize"] = QVariant(VolumeInfo::PhysicalSize(_dicomData.width * _dicomData.imageSpacings.x(),
                                                                            _dicomData.height * _dicomData.imageSpacings.y(),
                                                                            depth * _dicomData.imageSpacings.z()));

        blueprintMap["params"] = QVariant(blueprintParams);

        blueprintList[0] = QVariant(blueprintMap);
        blueprintOverallMap["models"] = QVariant(blueprintList);

        Message::SettingsMessage message("DicomParser", "Scene");
        message.data["action"] = "add";
        message.data["blueprint"] = QVariant(blueprintOverallMap);

        Message::SettingsMessage messageSpecs("DicomParser", "sidebar");
        messageSpecs.data["action"] = "setSpecs";

        QVariantMap paramsSpecs;

        paramsSpecs["huRange"] = QVariant(VolumeInfo::HuRange(_dicomData.minHU, _dicomData.maxHU));
        paramsSpecs["huRangePossible"] = QVariant(VolumeInfo::HuRange(_dicomData.minHUPossible, _dicomData.maxHUPossible));
        paramsSpecs["windowWidth"] = QVariant(_dicomData.windowWidth);
        paramsSpecs["windowCenter"] = QVariant(_dicomData.windowCenter);

        messageSpecs.data["params"] = QVariant(paramsSpecs);

        send(messageSpecs);

        send(message);
    }

    QUrl DicomReader::file() const {
        return _dicomFile;
    }

    void DicomReader::setFile(const QUrl & file) {
        if (file.isEmpty()) {
            return;
        }

        gdcm::ImageReader dIReader;

        qDebug() << file.toLocalFile().toStdString().c_str();
        dIReader.SetFileName(file.toLocalFile().toStdString().c_str());

        if (dIReader.Read()) {
            readImage(dIReader.GetFile(), dIReader.GetImage());
            _dicomFile = file;
        }
        else {
            qDebug() << "can't read file";
        }

        emit fileChanged();
    }
}
