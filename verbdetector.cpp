#include "verbdetector.h"
#include <QSet>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>

VerbDetector::VerbDetector(QString presentVerbRootsFileAddress, QString pastVerbRootsFileAddress, QString millas)
{
    QFile filePresent(presentVerbRootsFileAddress);
    QFile filePast(pastVerbRootsFileAddress);
    if(!filePresent.open(QIODevice::ReadOnly)){
        return;
    }
    if(!filePast.open(QIODevice::ReadOnly)){
        return;
    }
    QByteArray presentRaw = filePresent.readAll();
    filePresent.close();
    QByteArray pastRaw = filePast.readAll();
    filePast.close();
    QTextStream presentStream(presentRaw);
    presentStream.setAutoDetectUnicode(true);
    QString presentStringRawUtf = presentStream.readAll();
    Tokenizer presentTokenizer(presentStringRawUtf, "\t\r\n");
    QStringList presentRootList = presentTokenizer.getAllTokens();
    QTextStream pastStream(pastRaw);
    pastStream.setAutoDetectUnicode(true);
    QString pastStringRawUtf = pastStream.readAll();
    Tokenizer pastTokenizer(pastStringRawUtf, "\t\r\n");
    QStringList pastRootList = pastTokenizer.getAllTokens();
    foreach(QString s , presentRootList){
        presentRoots.insert(s);
    }
    foreach(QString s , pastRootList){
        pastRoots.insert(s);
    }
    QFile file(millas);
    file.open(QIODevice::ReadOnly);
    QTextStream stream(file.readAll());
    file.close();
    stream.setAutoDetectUnicode(true);
    __im = stream.readLine();
    __id = stream.readLine();
    __nd = stream.readLine();
    __m = stream.readLine();
    __i = stream.readLine();
    __mi = stream.readLine();
    __eh = stream.readLine();
    __bash = stream.readLine();
    __bud = stream.readLine();
    __d = stream.readLine();
    __am = stream.readLine();
    __ii =  stream.readLine();
    __ast = stream.readLine();
    __iim = stream.readLine();
    __iid = stream.readLine();
    __and = stream.readLine();
    __b = stream.readLine();
    __n = stream.readLine();
    __khah = stream.readLine();
    __nmi = __n + __mi;
}
int VerbDetector::IsVerb(Tokenizer& tokenizer){
    Tokenizer tokenizer2 = tokenizer;
    int p = IsAmrNahyVerb(tokenizer2);
    if(p > 0){
        return p;
    }
    tokenizer2 = tokenizer;
    p = IsFutureVerb(tokenizer2);
    if(p > 0){
        return p;
    }
    tokenizer2 = tokenizer;
    p = IsPresentVerb(tokenizer2);
    if(p > 0){
        return p;
    }
    tokenizer2 = tokenizer;
    p = IsPastVerb(tokenizer2);
    if(p){
        return p;
    }
    return 0;
}
int VerbDetector::IsAmrNahyVerb(Tokenizer &tokenizer){
    int p = 0;
    QString top = tokenizer.nextToken();
    p++;
    if(presentRoots.contains(top)){
        return p;
    }
    if(top.startsWith(__b) || top.startsWith(__n)){
        top = top.mid(1);
        if(presentRoots.contains(top)){
            return p;
        }
    }
    return 0;
}

int VerbDetector::IsPresentVerb(Tokenizer &tokenizer){
    int p = 0;
    QString top = tokenizer.nextToken();
    p++;
    bool MiSeen = false;
    if(top == __mi || top == __nmi){
        MiSeen = true;
        top = tokenizer.nextToken();
        p++;
    }
    QString twoChopped = top;
    twoChopped.chop(2);
    if(top.endsWith(__im) || top.endsWith(__id) || top.endsWith(__nd)){
        if(presentRoots.contains(twoChopped)){
            return p;
        }
    }
    QString oneChopped = top;
    oneChopped.chop(1);
    if(top.endsWith(__m) || top.endsWith(__i) || top.endsWith(__d)){
        if(presentRoots.contains(oneChopped)){
            return p;
        }
    }
    if(top.startsWith(__mi) || top.startsWith(__nmi)){
        int x;
        if(top.startsWith(__mi)){
            x = __mi.length();
        }else{
            x = __nmi.length();
        }
        top = top.mid(x);
        MiSeen = true;
        if(oneChopped.length() > x){
            oneChopped = oneChopped.mid(x);
            if(presentRoots.contains(oneChopped)){
                return p;
            }
        }
        if(twoChopped.length() > x){
            twoChopped = twoChopped.mid(x);
            if(presentRoots.contains(twoChopped)){
                return p;
            }
        }
    }
    if(!MiSeen && (top.startsWith(__b) || top.startsWith(__n))){
        oneChopped = oneChopped.mid(1);
        twoChopped = twoChopped.mid(1);
        if(presentRoots.contains(oneChopped) || presentRoots.contains(twoChopped)){
            return p;
        }
    }
    return 0;
}

int VerbDetector::IsFutureVerb(Tokenizer &tokenizer){
    int p = 0;
    QString top = tokenizer.nextToken();
    p++;
    if(top.startsWith(__khah)){
        top = top.mid(__khah.length());
    }else if(top.startsWith(__n + __khah)){
        top = top.mid((__n + __khah).length());
    }else{
        return 0;
    }
    if((top != __m) && (top != __i) && (top != __d) && (top != __im) && (top != __id) && (top != __nd)){
        return 0;
    }
    top = tokenizer.nextToken();
    p++;
    if(pastRoots.contains(top)){
        return p;
    }
    return 0;
}

int VerbDetector::IsPastVerb(Tokenizer &tokenizer){
    int p = 0;
    if(!tokenizer.hasMoreTokens()){
        return 0;
    }
    QString top = tokenizer.nextToken();
    p++;
    bool MiSeen = false;
    if(top == __mi || top == __nmi){
        int x = IsPastVerb(tokenizer);
        if(x > 0){
            return p + x;
        }else{
            return 0;
        }
    }
    if(pastRoots.contains(top)){
        return p;
    }
    if(top.endsWith(__im) || top.endsWith(__id) || top.endsWith(__nd)){
        top.chop(2);
    }else if(top.endsWith(__m) || top.endsWith(__i)){
        top.chop(1);
    }
    if(pastRoots.contains(top)){
        return p;
    }else{
        if(top.startsWith(__n)){
            if(pastRoots.contains(top.mid(1))){
                return p;
            }
        }
    }
    if(top.startsWith(__mi) || top.startsWith(__nmi)){
        int x;
        if(top.startsWith(__mi)){
            x = __mi.length();
        }else{
            x = __nmi.length();
        }
        MiSeen = true;
        top = top.mid(x);
        if(pastRoots.contains(top)){
            return p;
        }
    }
    if(top.endsWith(__eh) && !MiSeen){
        top.chop(1);

        if(pastRoots.contains(top)){
            if(!tokenizer.hasMoreTokens()){
                return 0;
            }
            top = tokenizer.nextToken();
            p++;
            if(top.startsWith(__bud)){
                if( top == __bud || top.endsWith(__im) || top.endsWith(__id) || top.endsWith(__nd) || top.endsWith(__m) || top.endsWith(__i)){
                    return p;
                }
            }else if(top.startsWith(__bash)){
                if(top.endsWith(__im) || top.endsWith(__id) || top.endsWith(__nd) || top.endsWith(__m) || top.endsWith(__i) || top.endsWith(__d)){
                    return p;
                }
            }else if(top.startsWith(__am) || top.startsWith(__ii) || top.startsWith(__ast) || top.startsWith(__iim) || top.startsWith(__iid) || top.startsWith(__and)){
                return p;
            }
        }else if(top.startsWith(__n)){
            top = top.mid(1);
            if(!tokenizer.hasMoreTokens()){
                return 0;
            }
            top = tokenizer.nextToken();
            p++;
            if(top.startsWith(__bud)){
                if( top == __bud || top.endsWith(__im) || top.endsWith(__id) || top.endsWith(__nd) || top.endsWith(__m) || top.endsWith(__i)){
                    return p;
                }
            }else if(top.startsWith(__bash)){
                if(top.endsWith(__im) || top.endsWith(__id) || top.endsWith(__nd) || top.endsWith(__m) || top.endsWith(__i) || top.endsWith(__d)){
                    return p;
                }
            }else if(top.startsWith(__am) || top.startsWith(__ii) || top.startsWith(__ast) || top.startsWith(__iim) || top.startsWith(__iid) || top.startsWith(__and)){
                return p;
            }
        }
    }

    return 0;
}
