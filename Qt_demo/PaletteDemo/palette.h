#ifndef PALETTE_H
#define PALETTE_H

#include <QDialog>
#include <QComboBox>
#include <QLabel>
#include <QTextEdit>
#include <QPushButton>
#include <QLineEdit>

namespace Ui {
class Palette;
}

class Palette : public QDialog
{
    Q_OBJECT

public:
    explicit Palette(QWidget *parent = 0);
    ~Palette();
    void createCtrlFrame();				//��ɴ�����벿����ɫѡ�����Ĵ���
    void createContentFrame();			//��ɴ����Ұ벿�ֵĴ���
    void fillColorList(QComboBox *);	//�������ɫ�����б���в�����ɫ�Ĺ���

private slots:
    void ShowWindow();
    void ShowWindowText();
    void ShowButton();
    void ShowButtonText();
    void ShowBase();

private:
    Ui::Palette *ui;
    QFrame *ctrlFrame;                  //��ɫѡ�����
    QLabel *windowLabel;
    QComboBox *windowComboBox;
    QLabel *windowTextLabel;
    QComboBox *windowTextComboBox;
    QLabel *buttonLabel;
    QComboBox *buttonComboBox;
    QLabel *buttonTextLabel;
    QComboBox *buttonTextComboBox;
    QLabel *baseLabel;
    QComboBox *baseComboBox;
    QFrame *contentFrame;              	//������ʾ���
    QLabel *label1;
    QComboBox *comboBox1;
    QLabel *label2;
    QLineEdit *lineEdit2;
    QTextEdit *textEdit;
    QPushButton *OkBtn;
    QPushButton *CancelBtn;
};

#endif // PALETTE_H
