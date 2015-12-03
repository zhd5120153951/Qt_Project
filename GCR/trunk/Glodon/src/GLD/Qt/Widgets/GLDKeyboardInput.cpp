#include <QBitmap>
#include <QPainter>
#include <QHash>
#include <QPushButton>
#include <QGridLayout>

#include "GLDFileUtils.h"
#include "GLDKeyboardInput.h"
#include "GLDKeyboardButton.h"

const int c_nHorizontalSpace                             = 1;
const int c_nCapsRowNumber                               = 1;
const int c_nCapsColoumNumber                            = 12;
const int c_nDeleteRowNumber                             = 0;
const int c_nDeleteColoumNumber                          = 11;
const int c_nVerticalSpace                               = 3;
const int c_nPatterXRadius                               = 3;
const int c_nPatterYRadius                               = 3;
const int c_nOneArraySize                                = 11;
const int c_nTwoArraySize                                = 10;
const int c_nThreeArraySize                              = 13;
const int c_nFourArraySize                               = 13;

const QSize c_keySize                                    = QSize(24, 24);
const QSize c_shiftSize                                  = QSize(48, 24);
const QSize c_deleteSize                                 = QSize(40, 24);
const QSize c_capsSize                                   = QSize(44, 24);
const QSize c_KeyboardInputSize                          = QSize(360, 120);

const QRect c_keyCloseButton                             = QRect(340, 3, 16, 16);

const QMargins c_keyMargins                              = QMargins(8, 3, 2, 1);

const GString numberTagListBlock[c_nOneArraySize]        = {"@","#","$","%","^","&","*","(",")","~","!"};
const GString numberTagList[c_nOneArraySize]             = {"2","3","4","5","6","7","8","9","0","`","1"};

const GString specialSymbolListBlock[c_nTwoArraySize]    = {"<",">","_","+","|","?","{","}",":","\""};
const GString specialSymbolList[c_nTwoArraySize]         = {",",".","/","-","=","\\","[","]",";","'"};

const GString characterOneListBlock[c_nThreeArraySize]   = {"M","A","B","C","D","E","F","G","H","I","J","K","L"};
const GString characterOneList[c_nThreeArraySize]        = {"m","a","b","c","d","e","f","g","h","i","j","k","l"};

const GString characterTwoListBlock[c_nFourArraySize]    = {"O","P","Q","R","S","T","U","V","W","X","Y","Z","N"};
const GString characterTwoList[c_nFourArraySize]         = {"o","p","q","r","s","t","u","v","w","x","y","z","n"};

const GString c_sKeyBordQssFile = ":/qsses/GBTKeyboardPurpleStyle.qss";

GLDKeyboardInput::GLDKeyboardInput(QWidget *parent)
    : QWidget(parent, 0)
    , m_signalMapperLowercase(new QSignalMapper(this))
    , m_signalMapperBlockletter(new QSignalMapper(this))
{
    m_pclosebtn = new QPushButton(this);
    m_pdeletebtn = new QPushButton("<-", this);
    m_pshiftbtn = new QPushButton(this);
    m_pcapslockbtn = new QPushButton(this);
    m_pcapslockbtn->setCheckable(true);
    m_pshiftbtn->setCheckable(true);

    this->setStyleSheet(loadQssFile(c_sKeyBordQssFile));
    initUI();
    initConnection();
    setFocusPolicy(Qt::WheelFocus);
}

void GLDKeyboardInput::addButton(QGridLayout *pgridlayout,
                                 const GString numberTagList[],
                                 const GString numberTagListBlock[],
                                 int lineNumber, int startKeySpace, int length)
{
    for (int i = 0; i < length; i++)
    {
        GLDKeyboardButton *pNumerbtn = new GLDKeyboardButton(numberTagList[i], this);
        pNumerbtn->setFixedSize(c_keySize);
        pNumerbtn->setObjectName("KeyboardButton");
        pNumerbtn->setCursor(Qt::PointingHandCursor);

        connect(pNumerbtn, SIGNAL(clicked()), m_signalMapperLowercase, SLOT(map()));
        m_signalMapperLowercase->setMapping(pNumerbtn, numberTagList[i]);

        connect(pNumerbtn, SIGNAL(clicked()), m_signalMapperBlockletter, SLOT(map()));
        m_signalMapperBlockletter->setMapping(pNumerbtn, numberTagListBlock[i]);

        if(0 == lineNumber)
        {
            connect(m_pshiftbtn, SIGNAL(clicked()), pNumerbtn, SLOT(switchNumberAndSymbol()));
        }
        else if(1 == lineNumber)
        {
            connect(m_pshiftbtn, SIGNAL(clicked()), pNumerbtn, SLOT(switchPunctuations()));
        }
        else
        {
            connect(m_pcapslockbtn, SIGNAL(clicked()), pNumerbtn, SLOT(switchLetterCase()));
            connect(m_pshiftbtn, SIGNAL(clicked()), pNumerbtn, SLOT(switchLetterCase()));
        }

        pgridlayout->addWidget(pNumerbtn, lineNumber, i + startKeySpace);
    }
}

void GLDKeyboardInput::initUI()
{
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setObjectName("Keyboard");
    setFixedSize(c_KeyboardInputSize);
    setFormsMask();

    //关闭按钮
    m_pclosebtn->setGeometry(c_keyCloseButton);
    m_pclosebtn->setObjectName("KeyboardCloseButton");
    m_pclosebtn->setCursor(Qt::PointingHandCursor);

    QGridLayout *pgridlayout = new QGridLayout;
    pgridlayout->setAlignment(Qt::AlignVCenter);
    pgridlayout->setContentsMargins(c_keyMargins);
    pgridlayout->setHorizontalSpacing(c_nHorizontalSpace);
    pgridlayout->setVerticalSpacing(c_nVerticalSpace);
    setLayout(pgridlayout);
    // 第一行按键
    // 第一行删除按钮
    m_pdeletebtn->setFocusProxy(this);
    m_pdeletebtn->setFixedSize(c_deleteSize);
    m_pdeletebtn->setObjectName("KeyboardDeleteButton");
    pgridlayout->addWidget(m_pdeletebtn, c_nDeleteRowNumber, c_nDeleteColoumNumber);
    addButton(pgridlayout, numberTagList, numberTagListBlock, 0, 0, c_nOneArraySize);
    // 第二行按键
    // 第二行第一个为shift键，占2个键位
    m_pshiftbtn->setFocusProxy(this);
    m_pshiftbtn->setFixedSize(c_shiftSize);
    m_pshiftbtn->setText("Shift");
    m_pshiftbtn->setObjectName("KeyboardShift");
    pgridlayout->addWidget(m_pshiftbtn, 1, 0, Qt::AlignLeft);
    // 第二行中间按键
    addButton(pgridlayout, specialSymbolList, specialSymbolListBlock, 1, 2, c_nTwoArraySize);
    // 第二行最后一个键为caps键
    m_pcapslockbtn->setFocusProxy(this);
    m_pcapslockbtn->setFixedSize(c_capsSize);
    m_pcapslockbtn->setObjectName("KeyboardCapsButton");
    m_pcapslockbtn->setText("Caps");
    pgridlayout->addWidget(m_pcapslockbtn, c_nCapsRowNumber, c_nCapsColoumNumber);
    // 第三行
    addButton(pgridlayout, characterOneList, characterOneListBlock, 2, 0, c_nThreeArraySize);
    // 第四行
    addButton(pgridlayout, characterTwoList, characterTwoListBlock, 3, 0, c_nFourArraySize);
}

void GLDKeyboardInput::initConnection()
{
    connect(m_pclosebtn, SIGNAL(clicked()), this, SLOT(close()));
    connect(m_pcapslockbtn, SIGNAL(clicked(bool)), this, SLOT(onClickedCapsLock(bool)));
    connect(m_pshiftbtn, SIGNAL(clicked(bool)), this, SLOT(onClickedShift(bool)));
    connect(m_pdeletebtn, SIGNAL(clicked()), parentWidget(), SLOT(onClickedDeletebtn()));
    connect(m_signalMapperLowercase, SIGNAL(mapped(const QString &)), parentWidget(), SLOT(onClickedBtn(const QString &)));
}

void GLDKeyboardInput::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() == Qt::LeftButton)
    {
        event->accept();
    }
    else
    {
        QWidget::mouseMoveEvent(event);
    }
}

void GLDKeyboardInput::onClickCapsAndShift(bool checked)
{
    if (checked)
    {
        disconnect(m_signalMapperLowercase, SIGNAL(mapped(const QString &)),
                   parentWidget(), SLOT(onClickedBtn(const QString &)));
        connect(m_signalMapperBlockletter, SIGNAL(mapped(const QString &)),
                parentWidget(), SLOT(onClickedBtn(const QString &)));
    }
    else
    {
        disconnect(m_signalMapperBlockletter, SIGNAL(mapped(const QString &)),
                   parentWidget(), SLOT(onClickedBtn(const QString &)));
        connect(m_signalMapperLowercase, SIGNAL(mapped(const QString &)),
                parentWidget(), SLOT(onClickedBtn(const QString &)));
    }
}

void GLDKeyboardInput::onClickedCapsLock(bool checked)
{
    onClickCapsAndShift(checked);
}

void GLDKeyboardInput::onClickedShift(bool checked)
{
    onClickCapsAndShift(checked);
}

void GLDKeyboardInput::focusOutEvent(QFocusEvent *)
{
    hide();
}

void GLDKeyboardInput::show()
{
    setFocus();
    QWidget::show();
}

void GLDKeyboardInput::setFormsMask()
{
    QBitmap bmp(this->size());
    bmp.fill();
    QPainter paiter(&bmp);
    paiter.setPen(Qt::NoPen);
    paiter.setBrush(Qt::blue);
    paiter.drawRoundedRect(bmp.rect(), c_nPatterXRadius, c_nPatterYRadius);
    setMask(bmp);//设置窗体遮罩
}

