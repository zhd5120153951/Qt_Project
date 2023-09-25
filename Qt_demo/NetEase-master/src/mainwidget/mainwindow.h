/*#######################################################################
		> File Name: mainwindow.h
		# Author: 		WIND
		# mail: 		fengleyl@163.com
		> Created Time: 2015��05��23�� ������ 11ʱ36��56��
########################################################################*/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "../basedwidget/basedwidget.h"
#include <QSystemTrayIcon>

class TitleWidget;
class MainTopWidget;
class MainBottomWidget;
class QMediaPlayer;
class QMediaPlaylist;
class QMenu;
class NetWorkWidget;
class SearchWidget;

bool connectDatabase();		//		����Sqlite���ݿ�

class MainWindow : public BasedWidget
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = 0);
	~MainWindow();

protected:
	void closeEvent(QCloseEvent *);

private slots:
	//		title signal-slot
	void showSkin();
	void showUpdate();
	void addFifles();
	void addLists();

	void trayIconClicked(QSystemTrayIcon::ActivationReason);

	void loopOnePlay();
	void loopPlay();
	void listPlay();
	void randomPlay();

	void mediaMetaChanged();
	void setMaxDruation();

	//		˫��QTableWidget����
	void playTo(int index);
	//		��Ӹ赥
	void addMusicList(QString name, QStringList files);
	//		��ȡ��ǰ��������Ϣ
	void getMusicInfo();

	//		��ǰ�����б�ı�
	void currentListChanged(const QString &text);

	//		�����ݿ����Ƴ�Ҫɾ��������
	void rmMusicFromData();
	void rmListFromData();

	//		������Դ�뱾����Դ�໥ȴ��..
	void switchNet();

	//		��ʾ��������..
	void showSearchWidget();

private:
	void initUi();
	void initConnect();
	void initMenu();

	//	�����б�
	void initList();

	//		�Զ���¼���ݻ�ȡ..
	void getNetUserInfo();

private:
	TitleWidget *m_mainTitle;

	QMenu *m_menu;
	QSystemTrayIcon *m_systemTray;

	MainTopWidget *m_mainTopWidget;
	MainBottomWidget *m_mainBottomWidget;

	//		������Դ����...
	NetWorkWidget *m_netWorkWidget;

	//		������
	QMediaPlayer *m_player;
	QMediaPlaylist *m_playList;
	int m_isDisconnect;
	QString m_currentList;		//	��¼��ǰ�����б���

	//		����...
	SearchWidget *m_searchWidget;
	//		�˳�ʱ��ʾ�Ƿ񱣴��û���Ϣ(�Զ���¼.)...
	int m_saveUserInfo;			//	ֵΪ0ʱ��ʾ������ʾ..
};

#endif // MAINWINDOW_H
