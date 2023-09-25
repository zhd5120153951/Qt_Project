#ifndef QLONGSPINBOX_H
#define QLONGSPINBOX_H

#include <QAbstractSpinBox>
#include <QRegExpValidator>
#include <QLineEdit>

class  QLongSpinBox : public QAbstractSpinBox
{
    Q_OBJECT

public:
    explicit QLongSpinBox(QWidget *parent = 0);

    qint64  value()     const;
    qint64  minimum()   const;
    qint64  maximum()   const;

protected:
    QRegExpValidator  fvalidator;

    void    stepBy(int steps);
    QAbstractSpinBox::StepEnabled   stepEnabled()   const;
    void    showEvent(QShowEvent *);
    void    updateLineEdit();

signals:
    void    valueChanged(qint64 value);

public slots:
    void    setValue(const qint64 & value);
    void    setMinimum(const qint64 & value);
    void    setMaximum(const qint64 & value);

private slots:
    void    applyData();

private :
    qint64          fvalue;
    qint64          fmin,fmax;
    QLineEdit*      flineEdit;
};

#endif // QLONGSPINBOX_H
