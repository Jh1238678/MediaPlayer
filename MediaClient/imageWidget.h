#ifndef IMAGEWIDGET_H
#define IMAGEWIDGET_H

#include<QObject>
#include<QWidget>
#include<QPaintEvent>
#include<QPainter>
class ImageWidget : public QWidget {
    Q_OBJECT
public:
    explicit ImageWidget(QWidget *parent = nullptr);
    ~ImageWidget();

    void setImage(const QImage& img) {
        m_image = img;
        update(); // 瑙﹀彂閲嶇粯
    }

protected:
    void paintEvent(QPaintEvent*) override {
        QPainter painter(this);
        if (!m_image.isNull()) {
            // 灞呬腑缁樺埗
            QSize targetSize = m_image.size().scaled(this->size(), Qt::KeepAspectRatio);

            QRect imageRect = QRect(QPoint(0,0), targetSize);
            imageRect.moveCenter(rect().center());
            painter.drawImage(imageRect, m_image);
        }
    }
public slots:
    void slot_setImage(QImage img);
public:
    QImage m_image;
};

#endif // IMAGEWIDGET_H
