#ifndef MAINTOPWIDGET_H
#define MAINTOPWIDGET_H

#include <QWidget>

class UserWidget;
class QTableWidget;
class MusicListsWidget;
class BackGroundWidget;

class MainTopWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MainTopWidget(QWidget *parent = 0);
    ~MainTopWidget();

    void addListItem(const QString &item);
    void addMusicItem(const QString &name, const QString &artist, const QString &special);

    void setMusicItem(const int &index, const QString &name, const QString &artist, const QString &special);

    void setcurrentList(int index);
    void setCurrentFile(int index);

    /**
     * @brief ��ȡ��ǰ�����б���
     * @return
     */
    QString getCurrentListName();
    /**
     * @brief ��ȡ��ǰ�������ֱ���
     * @return
     */
    QString currentMusicName();

    /**
     * @brief �û���Ϣ����
     * @param head
     * @param name
     */
    void setUserInfo(const QString &head, const QString &name);

signals:
    void logIn();

    void play(int index);

    void musicListItemClicked(const QString &text);

public slots:
    void rmCurrentListItem();
    void rmCurrentMusicItem();
    void rmAll();
    void rmAllList();

    void currentMusic(int index);

private:
    void initUi();
    void initConnect();

private:
    BackGroundWidget *m_backGroundWidget;

    /**
     * @brief �赥����
     */
    MusicListsWidget *m_musicListsWidget;
    /**
     * @brief �û�����
     */
    UserWidget *m_userWidget;
    /**
     * @brief ��ǰ�����б�
     */
    QTableWidget *m_currentList;
};

#endif // MAINTOPWIDGET_H
