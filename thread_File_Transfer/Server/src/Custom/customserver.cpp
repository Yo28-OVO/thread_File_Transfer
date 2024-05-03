#include "customserver.h"

CustomServer::CustomServer(QObject *parent) {
    this->setParent(parent);
}

void CustomServer::incomingConnection(qintptr Descriptor) {
    emit this->sentSocketDescriptor(Descriptor);
}
