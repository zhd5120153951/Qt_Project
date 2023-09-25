#ifndef GSAMPLEITEM_P_H
#define GSAMPLEITEM_P_H

#include <QList>
#include <QMetaType>

//class GSampleItemPrivate
//{
//public:
//    int fileRow;
//    QList<int> headers;
//    QList<int> channels;
//    QList<int> fluors;
//    QList<int> chNames;
//    int nameRow;     //��������
//    int typeRow;     //��������
//    QList<int> selects;
//    QList<int> values;
//    QList<int> valShows;
//};



struct _TreeStruct{
    bool expand;
    int  row;
    QList<int> items;
};
Q_DECLARE_METATYPE(_TreeStruct)

class GSampleItemPrivate
{
public:
    int fileRow;
    QList<int> headers;
    QList<_TreeStruct> channelItems;    //ͨ������
    int roxRow;     //�α�ӫ��

    int lockRow;     //����
    int nameRow;     //��������
    int typeRow;     //��������
    QList<_TreeStruct> sidItems;        //�������
    QList<_TreeStruct> uidItems;        //Ψһ����
    QList<_TreeStruct> useItems;     //ʹ��ͨ��
};

#endif // GSAMPLEITEM_P_H
