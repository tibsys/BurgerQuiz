#include "FormLog.h"
#include "ui_FormLog.h"
#include <QTimer>
#include <QDateTime>

FormLog* FormLog::instance_ = nullptr;

FormLog::FormLog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormLog)
{
    ui->setupUi(this);
    instance_ = this;

    //On programme tout de suite le timer qui raffraihira l'affichage des logs
    QTimer::singleShot(50, this, SLOT(refreshLogs()));
}

FormLog::~FormLog()
{
    delete ui;
}

void FormLog::appendLog(const QString& str) {
    logs_ << str;
}

void FormLog::refreshLogs()
{
    //On ne traite qu'un message par tour pour créer un effet "téléscripteur"
    if(!logs_.isEmpty()) {
        ui->textLog->append(QString("%1. %2").arg(QDateTime::currentDateTime().toString("hh:mm:ss.zzz")).arg(logs_.takeFirst()));
    }
    //Puis on reprogramme un tour
    QTimer::singleShot(20, this, SLOT(refreshLogs()));
}
