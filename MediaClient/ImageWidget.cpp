#include"ImageWidget.h"
#include<QDebug>
ImageWidget::ImageWidget(QWidget *parent)
    : QWidget(parent)
{
    // 检查图片是否存在
    //m_image = QImage("D:/C++/QWiget/elephant.jpg");
    if (m_image.isNull()) {
        qDebug() << "Failed to load image!";
    } else {
        qDebug() << "Image loaded successfully, size:" << m_image.size();
    }
}

ImageWidget::~ImageWidget()
{

}

void ImageWidget::slot_setImage(QImage img)
{
    this->setImage(img);

}
