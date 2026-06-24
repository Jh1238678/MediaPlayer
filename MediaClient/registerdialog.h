#ifndef REGISTERDIALOG_H
#define REGISTERDIALOG_H

#include <QDialog>
#include"packdef.h"
namespace Ui {
class registerDialog;
}

class registerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit registerDialog(QWidget *parent = nullptr);
    ~registerDialog();



signals:
    //把注册数据传给kernel
    void sig_registerData(QString name,QString password,Hobby* hy);
    //把登录数据传给kernel
    void sig_loginData(QString name,QString password);
private slots:
    void on_pb_clear_register_clicked();

    void on_pb_commit_register_clicked();

    void on_pb_clear_clicked();

    void on_pb_commit_clicked();

private:
    Ui::registerDialog *ui;
};

#endif // REGISTERDIALOG_H
