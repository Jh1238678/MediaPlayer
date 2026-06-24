#include "registerdialog.h"
#include "ui_registerdialog.h"
#include"QMessageBox"
registerDialog::registerDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::registerDialog)
{
    ui->setupUi(this);
}

registerDialog::~registerDialog()
{
    delete ui;
}

void registerDialog::on_pb_clear_register_clicked()
{
    ui->le_name_register->setText("");
    ui->le_password_register->setText("");

    ui->cb_dance  ->setChecked(false);
    ui->cb_edu    ->setChecked(false);
    ui->cb_film   ->setChecked(false);
    ui->cb_food   ->setChecked(false);
    ui->cb_funny  ->setChecked(false);
    ui->cb_good   ->setChecked(false);
    ui->cb_music  ->setChecked(false);
    ui->cb_outdoor->setChecked(false);
}


void registerDialog::on_pb_commit_register_clicked()
{
    QString name = ui->le_name_register->text();
    QString passwd = ui->le_password_register->text();
    Hobby* hy = new Hobby;
    //名字和密码不能为空
    if(name.remove(" ").isEmpty() || passwd.remove(" ").isEmpty()
            || name.isEmpty() || passwd.isEmpty())
    {
        QMessageBox::about(this,"提示","不能输入空内容或者是全空格");
        return ;
    }
    if(ui->cb_allin->isChecked())
    {
        hy->dance   = 50;
        hy->edu     = 50;
        hy->film    = 50;
        hy->food    = 50;
        hy->funny   = 50;
        hy->good    = 50;
        hy->music   = 50;
        hy->outdoor = 50;
    }else{
        hy->dance   =ui->cb_dance   ->isChecked()?50:0;
        hy->edu     =ui->cb_edu     ->isChecked()?50:0;
        hy->film    =ui->cb_film    ->isChecked()?50:0;
        hy->food    =ui->cb_food    ->isChecked()?50:0;
        hy->funny   =ui->cb_funny   ->isChecked()?50:0;
        hy->good    =ui->cb_good    ->isChecked()?50:0;
        hy->music   =ui->cb_music   ->isChecked()?50:0;
        hy->outdoor =ui->cb_outdoor ->isChecked()?50:0;
    }

    Q_EMIT sig_registerData(name,passwd,hy);
}


void registerDialog::on_pb_clear_clicked()
{
    ui->le_name->setText("");
    ui->le_password->setText("");
}


void registerDialog::on_pb_commit_clicked()
{
    QString name = ui->le_name->text();
    QString passwd = ui->le_password->text();
    //名字和密码不能为空
    if(name.remove(" ").isEmpty() || passwd.remove(" ").isEmpty()
            || name.isEmpty() || passwd.isEmpty())
    {
        QMessageBox::about(this,"提示","不能输入空内容或者是全空格");
        return ;
    }
    Q_EMIT sig_loginData(name,passwd);
}

