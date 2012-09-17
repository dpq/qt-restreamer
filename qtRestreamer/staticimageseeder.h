#ifndef BLANKSQUARESEEDER_H
#define BLANKSQUARESEEDER_H

#include "abstractseeder.h"
#include <QTimer>
#include <QFile>
#include <QScopedPointer>

class StaticImageSeeder : public AbstractSeeder
{
    Q_OBJECT
public:
    explicit StaticImageSeeder(QString imageTag, QString imagePath, QObject *parent = 0);
    inline const QString getTag()const {return m_imageTag;}
private:
    QTimer* t;
    QByteArray oneImage;
    QString m_imageTag;
    const static QString defaultPath;// = "/var/www/WhitePixel45w.jpg";
signals:
    
protected slots:
  void onTimeout();
  void doInit();
protected:
    virtual void unregisterStreamPoint(){}
 // void  onItemConnected();
};

#endif // BLANKSQUARESEEDER_H
