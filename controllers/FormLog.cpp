#include "FormLog.h"
#include "ui_FormLog.h"
#include <QTimer>

FormLog* FormLog::instance_ = nullptr;

FormLog::FormLog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormLog)
{
    ui->setupUi(this);
    instance_ = this;

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
    if(logs_.isEmpty())
        return;
    ui->textLog->append(logs_.takeFirst());
    QTimer::singleShot(50, this, SLOT(refreshLogs()));
}
