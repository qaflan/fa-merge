#ifndef COMPVECTOR_H
#define COMPVECTOR_H
#include <QList>
class CompVector
{
public:
    CompVector(int _id);
    int TermAt(int) const;
    int FrequencyAt(int) const;
    int GetID() const;
    int Length() const;
    int Lookup(int _term) const;
    void AddTerm(int _term);
    static double Sim(const CompVector&, const CompVector&);
private:
    int id;
    QList<int>  terms;
    QList<int>  frequencies;
};

#endif // COMPVECTOR_H
