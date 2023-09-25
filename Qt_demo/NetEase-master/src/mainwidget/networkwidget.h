#ifndef NETWORKWIDGET_H
#define NETWORKWIDGET_H

#include <QWidget>

#define NULL_REQUEST -1					//		��������..
#define LOGIN 0							//		��¼����..
#define GETMUSICLIST 1					//		��ȡ�赥...
#define GETLISTINFO 2					//		��ȡ�赥��ϸ��Ϣ...
#define SEARCH 3						//		����...
#define GETMP3URL 4						//		ͨ��songs id ��ȡmp3url..

class MainTopWidget;
class MainBottomWidget;
class TitleWidget;
class LogInWidget;
class QNetworkAccessManager;
class QNetworkReply;
class QProcess;

class NetWorkWidget : public QWidget
{
    Q_OBJECT
public:
    explicit NetWorkWidget(QWidget *parent = 0);
    ~NetWorkWidget();


    void closeWidget();
    void quitPlay();			//		���غ����粥��ȴ��ʱ����..
    //		�Զ���¼ʱ����NetWorkWidget����..
    void setAutoLogInInfo(QStringList list);
    //		�����û���Ϣ�����ݿ�...
    QStringList saveUserInfo();

public slots:
    //		ע����¼...
    void logOut();
    void search(const QString &search);			//		��������.�赥...

protected:
    void timerEvent(QTimerEvent *);

private slots:
    //		����ģ��...
    void playMedia(const QString &path);	//	����...
    void playFinished(int);						//	��ǰ�����������..
    void musicListItemClicked(const QString &);
    void musicItemClicked(int index);			//		˫������..
    //		��������..
    void adjustVoice(int voice);
    //		���ȵ���..
    void adjustDuration(qreal pos);
    //		����MainBottomWidget ���źŴ���...
    void play();
    void playNext();
    void playFront();


    //		��¼...
    void logInClicked();
    void logIn(const QString &, const QString &);

    void finished(QNetworkReply *reply);			//	�������ж�ȡ����..

private:
    void initUi();
    void initConnect();

    //		������������..
    void getMusicTime();							//	��ȡ��ǰ���ֽ��ȼ���ʱ��...
    void dealLogIn(QNetworkReply *reply);			//	��¼����...
    void getMusicList(int offset = 0, int limit = 100);
    void dealGetMusicList(QNetworkReply *reply);	//	��ȡ���ָ赥..
    void dealGetListInfo(QNetworkReply *reply);		//	��ȡ�赥��ϸ��Ϣ..
    void dealSeach(QNetworkReply *reply);			//	�����������..
    void getSongsUrl();
    void dealSongId(QNetworkReply *reply);			//	ͨ��songid��ȡsongurl..

private:
    //		���沼��...
    LogInWidget *m_logInWidget;
    MainTopWidget *m_topWidget;
    MainBottomWidget *m_bottomWIdget;

    //		�������..
    QNetworkAccessManager *m_accessManager;
    int m_type;			//		���ֲ�ͬ��������...

    //		�û���Ϣ...
    int m_userId;
    QString m_nickName;
    QString m_userName;
    QString m_userPasswd;		//		����md5���ܺ������..

    //		�赥��Ϣ...
    QHash<int, QString> m_currentMusicList;
    //		����赥��Ϣ,�赥���ĸ赥ID...
    QHash<QString, int> m_musicList;
    //		��¼������ȡ��songs Id...
    //QVector<int> m_songsId;
    QHash<int, int> m_songsId;


    //		���ֲ���ģ��..
    QProcess *m_process;		//		����������mplayer����..
    int m_currentPlayMusic;		//		��ǰ�������ֵ�����...
    bool m_isPlaying;			//		�Ƿ����ڲ���..
    bool m_isExitEvent;			//		�˳��¼�..
    int m_timerId;				//		��ʱ��...
    double m_duration;			//		��ǰ������ʱ��..
};

#endif // NETWORKWIDGET_H
