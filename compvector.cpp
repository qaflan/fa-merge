#include "compvector.h"
#include <math.h>

CompVector::CompVector(int _id) : id(_id)
{
}

int CompVector::GetID() const
{
    return id;
}

int CompVector::TermAt(int _index) const
{
    return terms.at(_index);
}

int CompVector::FrequencyAt(int _index) const
{
    return frequencies.at(_index);
}
int CompVector::Length() const
{
    return terms.length();
}

int CompVector::Lookup(int _term) const
{
    int Index = terms.indexOf(_term, 0);
    if(Index < 0){
        return 0;
    }
    return frequencies.at(Index);
}

double CompVector::Sim(const CompVector &Vector1, const CompVector &Vector2)
{
    double Result = 0;
    int Numer = 0;
    for(int i = 0 ; i < Vector1.Length() ; i++){
        int Term = Vector1.TermAt(i);
        int Freq1 = Vector1.FrequencyAt(i);
        int Freq2 = Vector2.Lookup(Term);
        Numer += Freq1 * Freq2;
    }
    if(Numer == 0){
        return 0;
    }
    int Denom1 = 0, Denom2 = 0, Denom = 0;
    for(int i = 0 ; i < Vector1.Length() ; i++){
        Denom1 += Vector1.frequencies.at(i)*Vector1.frequencies.at(i);
    }
    for(int i = 0 ; i < Vector2.Length() ; i++){
        Denom2 += Vector2.frequencies.at(i)*Vector2.frequencies.at(i);
    }
    Denom = Denom1 * Denom2;
    Result = ((double)Numer) / sqrt(Denom);
    if(Result > 1.0 || Result < 0.0){
        qDebug("Inconsistent SIM Result: %f", Result);
    }
    return Result;
}

void CompVector::AddTerm(int _term)
{
    int Index = terms.indexOf(_term);
    if(Index == -1){
        terms.append(_term);
        frequencies.append(1);
    }else{
        frequencies[Index]++;
    }
}
