#include "abstractseeder.h"

AbstractSeeder::AbstractSeeder(QObject *parent) :
    QObject(parent),StreamPoint()
{
}
const QString AbstractSeeder::defaultBoundary="boundarydonotcross";
