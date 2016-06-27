#ifndef NOMORETOKENEXCEPTION_H
#define NOMORETOKENEXCEPTION_H
#include <QtCore>
class NoMoreTokensException: public QtConcurrent::Exception {
public:
    NoMoreTokensException(){};
};

#endif // NOMORETOKENEXCEPTION_H
