#ifndef BLANKSQUARESEEDER_H
#define BLANKSQUARESEEDER_H

#include "abstractseeder.h"
#include <QTimer>
#include <QFile>

class BlankSquareSeeder : public AbstractSeeder
{
    Q_OBJECT
public:
    explicit BlankSquareSeeder(QObject *parent = 0);

private:
    QTimer t;
    QByteArray oneImage;
    const static QString defaultPath;// = "/var/www/WhitePixel45w.jpg";
signals:
    
protected slots:
  void onTimeout();
 // void  onItemConnected();
};

#endif // BLANKSQUARESEEDER_H
