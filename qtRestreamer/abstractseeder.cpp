#include "abstractseeder.h"

AbstractSeeder::AbstractSeeder(QObject* parent) :
    StreamPoint(parent)
{
}
const QString AbstractSeeder::defaultBoundary="boundarydonotcross";
