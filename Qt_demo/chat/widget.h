#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QtNetwork>
#include <QtGui>
#include "tcpclient.h"
#include "tcpserver.h"
namespace Ui {
    class Widget;
}

enum MessageType{Message,NewParticipant,ParticipantLeft,FileName,Refuse};
//ö�ٱ�����־��Ϣ�����ͣ��ֱ�Ϊ��Ϣ�����û����룬���û��˳�
class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();
    QString getUserName();
    QString getMessage();

protected:
    void changeEvent(QEvent *e);
    void sendMessage(MessageType type,QString serverAddress="");
    void newParticipant(QString userName,QString localHostName,QString ipAddress);
    void participantLeft(QString userName,QString localHostName,QString time);
    void closeEvent(QCloseEvent *);
    void hasPendingFile(QString userName,QString serverAddress,
                        QString clientAddress,QString fileName);

     bool eventFilter(QObject *target, QEvent *event);//�¼�������
private:
    Ui::Widget *ui;
    QUdpSocket *udpSocket;
    qint16 port;
    QString fileName;
    TcpServer *server;

    QString getIP();

    QColor color;//��ɫ

    bool saveFile(const QString& fileName);//���������¼
private slots:

    void on_textUnderline_clicked(bool checked);
    void on_clear_clicked();
    void on_save_clicked();

    void on_textcolor_clicked();
    void on_textitalic_clicked(bool checked);
    void on_textbold_clicked(bool checked);
    void on_fontComboBox_currentFontChanged(QFont f);
    void on_fontsizecomboBox_currentIndexChanged(QString );
    void on_close_clicked();
    void on_sendfile_clicked();
    void on_send_clicked();
    void processPendingDatagrams();
    void sentFileName(QString);
    void currentFormatChanged(const QTextCharFormat &format);

};

#endif // WIDGET_H
