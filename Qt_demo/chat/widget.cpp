#include "widget.h"
#include "ui_widget.h"

#include <QColorDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QScrollBar>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    ui->label->setText("<a href=\"http://www.bcwhy.com\"> www.bcwhy.com </a>");
    this->resize(850,550);
    ui->textEdit->setFocusPolicy(Qt::StrongFocus);
    ui->textBrowser->setFocusPolicy(Qt::NoFocus);

    ui->textEdit->setFocus();
    ui->textEdit->installEventFilter(this);

    udpSocket = new QUdpSocket(this);
    port = 45454;
    udpSocket->bind(port,QUdpSocket::ShareAddress
                    | QUdpSocket::ReuseAddressHint);
    connect(udpSocket,SIGNAL(readyRead()),this,SLOT(processPendingDatagrams()));
    sendMessage(NewParticipant);

    server = new TcpServer(this);
    connect(server,SIGNAL(sendFileName(QString)),this,SLOT(sentFileName(QString)));
    connect(ui->textEdit,SIGNAL(currentCharFormatChanged(QTextCharFormat)),this,SLOT(currentFormatChanged(const QTextCharFormat)));
}

void Widget::currentFormatChanged(const QTextCharFormat &format)
{
    //���༭���������ʽ�ı�ʱ�������ò���״̬Ҳ��֮�ı�
    ui->fontComboBox->setCurrentFont(format.font());

    //��������С������Ϊ������С������Ϊ9
    if(format.fontPointSize()<9)
    {
        ui->fontsizecomboBox->setCurrentIndex(3);
    }
    else
    {
        //����ʾ12
        ui->fontsizecomboBox->setCurrentIndex(
                    ui->fontsizecomboBox->findText(QString::number(format.fontPointSize())));
    }

    ui->textbold->setChecked(format.font().bold());
    ui->textitalic->setChecked(format.font().italic());
    ui->textUnderline->setChecked(format.font().underline());
    color = format.foreground().color();
}

//��������UDP
void Widget::processPendingDatagrams()
{
    while(udpSocket->hasPendingDatagrams())
    {
        QByteArray datagram;
        datagram.resize(udpSocket->pendingDatagramSize());
        udpSocket->readDatagram(datagram.data(),datagram.size());
        QDataStream in(&datagram,QIODevice::ReadOnly);
        int messageType;
        in >> messageType;
        QString userName,localHostName,ipAddress,message;
        QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
        switch(messageType)
        {
        case Message:
        {
            in >>userName >>localHostName >>ipAddress >>message;
            ui->textBrowser->setTextColor(Qt::blue);
            ui->textBrowser->setCurrentFont(QFont("Times New Roman",12));
            ui->textBrowser->append("[ " +userName+" ] "+ time);
            ui->textBrowser->append(message);
            break;
        }
        case NewParticipant:
        {
            in >>userName >>localHostName >>ipAddress;
            newParticipant(userName,localHostName,ipAddress);

            break;
        }
        case ParticipantLeft:
        {
            in >>userName >>localHostName;
            participantLeft(userName,localHostName,time);
            break;
        }
        case FileName:
        {
            in >>userName >>localHostName >> ipAddress;
            QString clientAddress,fileName;
            in >> clientAddress >> fileName;
            hasPendingFile(userName,ipAddress,clientAddress,fileName);
            break;
        }
        case Refuse:
        {
            in >> userName >> localHostName;
            QString serverAddress;
            in >> serverAddress;
            QString ipAddress = getIP();

            if(ipAddress == serverAddress)
            {
                server->refused();
            }
            break;
        }
        }
    }
}

//�������û�����
void Widget::newParticipant(QString userName,QString localHostName,QString ipAddress)
{
    bool bb = ui->tableWidget->findItems(localHostName,Qt::MatchExactly).isEmpty();
    if(bb)
    {
        QTableWidgetItem *user = new QTableWidgetItem(userName);
        QTableWidgetItem *host = new QTableWidgetItem(localHostName);
        QTableWidgetItem *ip = new QTableWidgetItem(ipAddress);
        ui->tableWidget->insertRow(0);
        ui->tableWidget->setItem(0,0,user);
        ui->tableWidget->setItem(0,1,host);
        ui->tableWidget->setItem(0,2,ip);
        ui->textBrowser->setTextColor(Qt::gray);
        ui->textBrowser->setCurrentFont(QFont("Times New Roman",10));
        ui->textBrowser->append(QStringLiteral("%1 ���ߣ�").arg(userName));
        ui->onlineUser->setText(QStringLiteral("����������%1").arg(ui->tableWidget->rowCount()));
        sendMessage(NewParticipant);
    }
}

//�����û��뿪
void Widget::participantLeft(QString userName,QString localHostName,QString time)
{
    int rowNum = ui->tableWidget->findItems(localHostName,Qt::MatchExactly).first()->row();
    ui->tableWidget->removeRow(rowNum);
    ui->textBrowser->setTextColor(Qt::gray);
    ui->textBrowser->setCurrentFont(QFont("Times New Roman",10));
    ui->textBrowser->append(QStringLiteral("%1 �� %2 �뿪��").arg(userName).arg(time));
    ui->onlineUser->setText(QStringLiteral("����������%1").arg(ui->tableWidget->rowCount()));
}

Widget::~Widget()
{
    delete ui;
}

void Widget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

//��ȡip��ַ
QString Widget::getIP()
{
    QList<QHostAddress> list = QNetworkInterface::allAddresses();

    foreach (QHostAddress address, list)
    {
        //����ʹ��IPv4��ַ
        if(address.protocol() == QAbstractSocket::IPv4Protocol)
        {
            return address.toString();
        }
    }

    return 0;
}

//������Ϣ
void Widget::sendMessage(MessageType type, QString serverAddress)
{
    QByteArray data;
    QDataStream out(&data,QIODevice::WriteOnly);
    QString localHostName = QHostInfo::localHostName();
    QString address = getIP();
    out << type << getUserName() << localHostName;


    switch(type)
    {
    case ParticipantLeft:
    {
        break;
    }
    case NewParticipant:
    {
        out << address;
        break;
    }

    case Message :
    {
        if(ui->textEdit->toPlainText() == "")
        {
            QMessageBox::warning(0,QStringLiteral("����"),QStringLiteral("�������ݲ���Ϊ��"),QMessageBox::Ok);
            return;
        }
        out << address << getMessage();
        ui->textBrowser->verticalScrollBar()->setValue(ui->textBrowser->verticalScrollBar()->maximum());
        break;

    }
    case FileName:
    {
        int row = ui->tableWidget->currentRow();
        QString clientAddress = ui->tableWidget->item(row,2)->text();
        out << address << clientAddress << fileName;
        break;
    }
    case Refuse:
    {
        out << serverAddress;
        break;
    }
    }
    udpSocket->writeDatagram(data,data.length(),QHostAddress::Broadcast, port);

}

//��ȡ�û���
QString Widget::getUserName()
{
    QStringList envVariables;
    envVariables << "USERNAME.*" << "USER.*" << "USERDOMAIN.*"
                 << "HOSTNAME.*" << "DOMAINNAME.*";
    QStringList environment = QProcess::systemEnvironment();

    foreach (QString string, envVariables)
    {
        int index = environment.indexOf(QRegExp(string));

        if (index != -1)
        {
            QStringList stringList = environment.at(index).split('=');

            if (stringList.size() == 2)
            {
                return stringList.at(1);
                break;
            }
        }
    }
    return false;
}

//���Ҫ���͵���Ϣ
QString Widget::getMessage()
{
    QString msg = ui->textEdit->toHtml();

    ui->textEdit->clear();
    ui->textEdit->setFocus();
    return msg;
}

void Widget::closeEvent(QCloseEvent *)
{
    sendMessage(ParticipantLeft);
}

void Widget::sentFileName(QString fileName)
{
    this->fileName = fileName;
    sendMessage(FileName);
}

//�����ļ�
void Widget::hasPendingFile(QString userName,QString serverAddress,
                            QString clientAddress,QString fileName)
{
    QString ipAddress = getIP();

    if(ipAddress == clientAddress)
    {
        int btn = QMessageBox::information(this,QStringLiteral("�����ļ�"),
                                           QStringLiteral("����%1(%2)���ļ���%3,�Ƿ���գ�")
                                           .arg(userName).arg(serverAddress).arg(fileName),
                                           QMessageBox::Yes,QMessageBox::No);
        if(btn == QMessageBox::Yes)
        {
            QString name = QFileDialog::getSaveFileName(0,QStringLiteral("�����ļ�"),fileName);

            if(!name.isEmpty())
            {
                TcpClient *client = new TcpClient(this);
                client->setFileName(name);
                client->setHostAddress(QHostAddress(serverAddress));
                client->show();
            }
        }
        else
        {
            sendMessage(Refuse,serverAddress);
        }
    }
}

//����
void Widget::on_send_clicked()
{
    sendMessage(Message);
}

void Widget::on_sendfile_clicked()
{
    if(ui->tableWidget->selectedItems().isEmpty())
    {
        QMessageBox::warning(0,QStringLiteral("ѡ���û�"),QStringLiteral("���ȴ��û��б�ѡ��Ҫ���͵��û���"),QMessageBox::Ok);
        return;
    }

    server->show();
    server->initServer();
}

//�ر�
void Widget::on_close_clicked()
{
    this->close();
}

bool Widget::eventFilter(QObject *target, QEvent *event)
{
    if(target == ui->textEdit)
    {
        if(event->type() == QEvent::KeyPress)
        {
            QKeyEvent *k = static_cast<QKeyEvent *>(event);

            if(k->key() == Qt::Key_Return)
            {
                on_send_clicked();
                return true;
            }
        }
    }

    return QWidget::eventFilter(target,event);
}

//��������
void Widget::on_fontComboBox_currentFontChanged(QFont f)
{
    ui->textEdit->setCurrentFont(f);
    ui->textEdit->setFocus();
}

void Widget::on_fontsizecomboBox_currentIndexChanged(QString size)
{
    ui->textEdit->setFontPointSize(size.toDouble());
    ui->textEdit->setFocus();
}

void Widget::on_textbold_clicked(bool checked)
{
    if(checked)
        ui->textEdit->setFontWeight(QFont::Bold);
    else
        ui->textEdit->setFontWeight(QFont::Normal);
    ui->textEdit->setFocus();
}

void Widget::on_textitalic_clicked(bool checked)
{
    ui->textEdit->setFontItalic(checked);
    ui->textEdit->setFocus();
}

void Widget::on_textUnderline_clicked(bool checked)
{
    ui->textEdit->setFontUnderline(checked);
    ui->textEdit->setFocus();
}

void Widget::on_textcolor_clicked()
{
    color = QColorDialog::getColor(color,this);
    if(color.isValid())
    {
        ui->textEdit->setTextColor(color);
        ui->textEdit->setFocus();
    }
}

//���������¼
void Widget::on_save_clicked()
{
    if(ui->textBrowser->document()->isEmpty())
    {
        QMessageBox::warning(0,
                             QStringLiteral("����"),
                             QStringLiteral("�����¼Ϊ�գ��޷����棡"),
                             QMessageBox::Ok);
    }
    else
    {
        //����ļ���
        QString fileName = QFileDialog::getSaveFileName(this,
                                                        QStringLiteral("���������¼"),
                                                        QStringLiteral("�����¼"),
                                                        QStringLiteral("�ı�(*.txt);;All File(*.*)"));
        if(!fileName.isEmpty())
        {
            saveFile(fileName);
        }
    }
}

bool Widget::saveFile(const QString &fileName)//�����ļ�
{
    QFile file(fileName);

    if(!file.open(QFile::WriteOnly | QFile::Text))
    {
        QMessageBox::warning(this,QStringLiteral("�����ļ�"),
                             QStringLiteral("�޷������ļ� %1:\n %2").arg(fileName)
                             .arg(file.errorString()));
        return false;
    }

    QTextStream out(&file);

    out << ui->textBrowser->toPlainText();

    return true;
}

void Widget::on_clear_clicked()//��������¼
{
    ui->textBrowser->clear();
}
