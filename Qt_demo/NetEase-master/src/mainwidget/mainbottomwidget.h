#ifndef MAINBOTTOMWIDGET_H
#define MAINBOTTOMWIDGET_H

#include <QWidget>
#include <QMediaPlayer>

class BackGroundWidget;
class QLabel;
class FuncButton;
class SliderWidget;

class MainBottomWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MainBottomWidget(QWidget *parent = 0);
    ~MainBottomWidget();

    void setMaxDuration(qint64 max);
    void setPlayButtonStatues(const QString &icon);//���ò��Ű�ť״̬
    void setNetWorkMode(bool isTrue);

    void setPostion(const QString &currentPos, const QString &postion);
    void currentPosSlider(qreal pos);

signals:
    void play();
    void playNext();
    void playFront();
    void pasue();

    //����MainWindow��QMediaplayer
    void setPostion(qint64 pos);
    void setPosTo(qreal pos);
    void setVoice(int voice);

public slots:
    //MainWindow->this
    //�������ȸı�..
    void postionChangeTo(qint64 pos);
    //����״̬�ı�
    void playStateChanged(QMediaPlayer::State state);

public slots:
    void on_playButton_clicked();
    //SliderWidget->this
    void voiceChanged(qreal pos);
    void postionChanged(qreal pos);

private:
    void initUi();
    void initConnect();

private:
    BackGroundWidget *m_backGroundWidget;
    QLabel *m_musicLogo;

    //buttons
    FuncButton *m_playButton;
    FuncButton *m_playNextButton;
    FuncButton *m_playFrontButton;

    //���Ⱥ�������
    qint64 m_maxDuration;//��ǰ����ʱ��
    QLabel *m_currentVoice;
    QLabel *m_currentPostion;
    QLabel *m_duration;
    SliderWidget *m_voiceWidget;
    SliderWidget *m_postionWidget;
};

#endif // MAINBOTTOMWIDGET_H
