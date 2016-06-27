#ifndef TOKENIZER_H
#define TOKENIZER_H
#include <QString>
#include <QStringList>
class Tokenizer
{
public:
    Tokenizer();
    Tokenizer(QString string, QString delimiters=" .-+=|\\/!??\t\r\n‌");
    QString nextToken();
    QString seeNextToken();
    QStringList getAllTokens();
    void forward(int steps);
    bool hasMoreTokens();
private:
    int index;
    QStringList tokens;
};

#endif // TOKENIZER_H
