#ifndef VERBDETECTOR_H
#define VERBDETECTOR_H
#include "tokenizer.h"
#include "definitions.h"
#include <QSet>
class VerbDetector
{
public:
    VerbDetector(QString presentVerbRootsFileAddress = PRESENT_VERB_ROOT_FILE_ADDRESS, QString pastVerbRootsFileAddress = PAST_VERB_ROOT_FILE_ADDRESS, QString millas = MILLAS_FILE_ADDRESS);
    int IsVerb(Tokenizer& tokenizer);
    int IsPastVerb(Tokenizer& tokenizer);
    int IsPresentVerb(Tokenizer& tokenizer);
    int IsFutureVerb(Tokenizer& tokenizer);
    int IsAmrNahyVerb(Tokenizer& tokenizer);
private:
    QSet<QString> presentRoots;
    QSet<QString> pastRoots;
    QString __eh;
    QString __bash;
    QString __bud;
    QString __d;
    QString __im;
    QString __id;
    QString __nd;
    QString __m;
    QString __i;
    QString __mi;
    QString __am;
    QString __ii;
    QString __ast;
    QString __iim;
    QString __iid;
    QString __and;
    QString __b;
    QString __n;
    QString __khah;
    QString __nmi;
};

#endif // VERBDETECTOR_H
