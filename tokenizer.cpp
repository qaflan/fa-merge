#include "tokenizer.h"
#include "nomoretokensexception.h"
#include <QString>
#include <QStringList>
Tokenizer::Tokenizer() : index(0)
{
}
Tokenizer::Tokenizer(QString string, QString delimiters){
    for(int i=1 ; i<delimiters.length() ; i++){
        string.replace(delimiters.at(i),delimiters.at(0));
    }
    tokens = string.split(delimiters.at(0), QString::SkipEmptyParts);
    index = 0;
}

QString Tokenizer::nextToken(){
    if(index < tokens.size()){
        return (QString)tokens.at(index++);
    }else{
//        throw NoMoreTokensExeception();
        return QString();
    }
}
QStringList Tokenizer::getAllTokens(){
    return tokens;
}
void Tokenizer::forward(int steps){
    index += steps;
}

QString Tokenizer::seeNextToken(){
    if(index < tokens.length()){
        return tokens.at(index);
    }else{
        return QString();
//        throw NoMoreTokensException();
    }
}

bool Tokenizer::hasMoreTokens(){
    return index < tokens.length();
}
