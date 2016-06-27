#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ui_wizard.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include "tokenizer.h"
#include "verbdetector.h"
#include "compvector.h"
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->lstInputFiles, SIGNAL(currentRowChanged(int)), this, SLOT(inputFileIndexChanged(int)));
    browserEmptyHtml = ui->txtBrsInputReader->toHtml();
    //
    QFile LangDataFile(NOUN_AFFIX_FILE_ADDRESS);
    LangDataFile.open(QFile::ReadOnly);
    QTextStream Stream(&LangDataFile);
    Stream.setAutoDetectUnicode(true);
    lang_an = Stream.readLine().trimmed();
    lang_at = Stream.readLine().trimmed();
    lang_ati = Stream.readLine().trimmed();
    lang_ha = Stream.readLine().trimmed();
    lang_haii = Stream.readLine().trimmed();
    lang_ii = Stream.readLine().trimmed();
    lang_ini = Stream.readLine().trimmed();
    lang_in = Stream.readLine().trimmed();
    lang_yii = Stream.readLine().trimmed();
    LangDataFile.close();
    QFile NounsFile(NOUNS_FILE_ADDRESS);
    NounsFile.open(QFile::ReadOnly);
    QTextStream Stream2(&NounsFile);
    Stream2.setAutoDetectUnicode(true);
    while(!Stream2.atEnd()){
        Lang_Nouns.insert(Stream2.readLine().trimmed());
    }
    NounsFile.close();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void MainWindow::on_action_2_triggered()
{

}

void MainWindow::on_actionAddFilesToInput_triggered()
{
    //Show a dialog box and read from file
    QFileDialog Dialog;
    QStringList List = Dialog.getOpenFileNames(this,"Open");
    if(List.length() > 0){
        foreach(QString Path, List){
            if(inputFileNames.contains(Path)){
                //Error
                QMessageBox Box;
                Box.setText("File already added!");
                Box.exec();
            }else if(!QFile::exists(Path)){
                QMessageBox Box;
                Box.setText("File not found!");
                Box.exec();
            }else{
                QFile File(Path);
                File.open(QFile::ReadOnly);
                QTextStream Stream(&File);
                Stream.setAutoDetectUnicode(true);
                inputFileContents.append(Stream.readAll());
                inputFileNames.append(Path);
                File.close();
                updateList();
            }
        }
    }
}

void MainWindow::inputFileIndexChanged(int _index)
{
    if(_index >= 0 ){
        ui->btnInputDeleteFile->setEnabled(true);
        qDebug("Index Changed.");
        ui->txtBrsInputReader->setText(inputFileContents.at(_index));
        ui->txtBrsInputReader->setEnabled(true);
    }
}

void MainWindow::updateList()
{
    int currentSelected = ui->lstInputFiles->currentIndex().row();
    ui->lstInputFiles->clear();
    foreach(QString FileName, inputFileNames){
        ui->lstInputFiles->addItem(FileName);
    }
    if(currentSelected >= 0 && currentSelected < ui->lstInputFiles->count()){
        ui->lstInputFiles->setCurrentRow(currentSelected);
        ui->btnInputDeleteFile->setEnabled(true);
    }else{
        ui->btnInputDeleteFile->setEnabled(false);
        ui->txtBrsInputReader->setHtml(browserEmptyHtml);
        ui->txtBrsInputReader->setEnabled(false);
    }
}

void MainWindow::on_actionDeleteFileFromInput_triggered()
{
    int currentSelected = ui->lstInputFiles->currentRow();
    if(currentSelected >= 0 && currentSelected < ui->lstInputFiles->count()){
        inputFileNames.removeAt(currentSelected);
        inputFileContents.removeAt(currentSelected);
        updateList();
    }
}

void MainWindow::on_actionProcess_triggered()
{
    //Load Files
    QSet<QString>* PrepositionsSet = loadIntoHash(PREPOSITION_FILE_ADDRESS);
    QSet<QString>* CommonWordsSet = loadIntoHash(COMMON_WORDS_FILE_ADDRESS);
    QSet<QString>  EffectiveWords;
    //Statistics
    int VerbCount = 0;
    int CommonWordCount = 0;
    int PrepositionCount = 0;
    int WordCount = 0;
    int LetterCount  = 0;
    int SentenceCount = 0;
    int EffectiveWordCount = 0;
    VerbDetector vDetector;
    QFile WWW("fff.fff");
    WWW.open(QFile::WriteOnly);
    foreach(QString Doc, inputFileContents){
        Tokenizer tokenizer(Doc);
        while(tokenizer.hasMoreTokens()){
            int Steps = vDetector.IsVerb(tokenizer);
            if(Steps > 0){
                VerbCount++;
                for(int i = 0 ; i < Steps ; i++){
                    LetterCount += tokenizer.nextToken().length();
                }
            }else{
                QString Token = tokenizer.nextToken();
                LetterCount += Token.length();
                Token = NormalizeToken(Token);
                if(Token.length() == 0){
                    continue;
                }
                if(PrepositionsSet->contains(Token)){
                    PrepositionCount++;
                }else if(CommonWordsSet->contains(Token)){
                    CommonWordCount++;
                }else{
                    if(!EffectiveWords.contains(Token)){
                        WWW.write((Token).toUtf8());
                        WWW.write("\n");
                        EffectiveWordCount++;
                        EffectiveWords.insert(Token);
                    }
                }
            }
            WordCount++;
        }
    }
    WWW.close();
    ui->lblStatVerbCount->setText(QString::number(VerbCount));
    ui->lblStatPrepCount->setText(QString::number(PrepositionCount));
    ui->lblStatCommonWordCount->setText(QString::number(CommonWordCount));
    ui->lblStatEffectiveWordCount->setText(QString::number(EffectiveWordCount));
    ui->lblStatWordCount->setText(QString::number(WordCount));
    ui->lblStatLetterCount->setText(QString::number(LetterCount));
    ui->lblStatTotalUselessWordCount->setText(QString::number(VerbCount + PrepositionCount + CommonWordCount));
    delete PrepositionsSet;
    delete CommonWordsSet;
    QList<QString> FinalWords = EffectiveWords.toList();
    //Read and Encode Sentences
    QHash<int, QString> Sentencetable;
    QList<CompVector> Sentences = ReadAndEncodeSentences(FinalWords, Sentencetable);
    ui->lblStatSentenceCount->setText(QString::number(Sentences.length()));
    QList<QList<CompVector> > Paragraphs = ConstructParagraphs(Sentences);
    qDebug("Num Paragraphs: %d", Paragraphs.count());
    //Decode Sentences
    FinalResult.clear();
    foreach(QList<CompVector> Paragraph, Paragraphs){
        foreach(CompVector Sentence, Paragraph){
            QString DecodedSentence = *Sentencetable.find(Sentence.GetID());
            FinalResult.append(" " + DecodedSentence);
        }
        FinalResult.append("\n\r\n\r\n\r\n\r");
    }
    ui->txtBrsOutput->setText(FinalResult);
}

QSet<QString>* MainWindow::loadIntoHash(QString _file)
{
    QSet<QString>* Set = new QSet<QString>();
    qDebug(qPrintable(_file));
    QFile File(_file);
    File.open(QFile::ReadOnly);
    QTextStream Stream(&File);
    Stream.setAutoDetectUnicode(true);
    while(!Stream.atEnd()){
        Set->insert(Stream.readLine().trimmed());
    }
    return Set;
}

QList<CompVector> MainWindow::ReadAndEncodeSentences(QList<QString> EffectiveWords, QHash<int, QString> &SentenceTable)
{
    int ID = 0;
    QList<CompVector> Result;
    foreach(QString Content, inputFileContents){
        Tokenizer tokenizer(Content, ".!?؟\n\r");
        while(tokenizer.hasMoreTokens()){
            QString NextSentence = tokenizer.nextToken();
            CompVector SentenceVector(ID);
            SentenceTable.insert(ID, NextSentence);
            ID++;
            Tokenizer LocalTokenizer(NextSentence);
            while(LocalTokenizer.hasMoreTokens()){
                QString Word = LocalTokenizer.nextToken();
                int Location = EffectiveWords.indexOf(Word);
                if(Location != -1){
                    SentenceVector.AddTerm(Location);
                }
            }
            Result.append(SentenceVector);
        }
    }
    return Result;
}

QList<QList<CompVector> > MainWindow::ConstructParagraphs(QList<CompVector> _sentences)
{
//    qDebug("%f", CompVector::Sim(_sentences.at(0), _sentences.at(0)));
    double TRESHOLD = 0.15;
    double BestSimSofar = 0;
    int BestParagraphSoFar = -1;
    int ParagraphNo = -1;
    QList<QList<CompVector> > Result;
    foreach(CompVector Vector, _sentences){
        if(Result.length() == 0){
            //First Time
            QList<CompVector> List;
            List.append(Vector);
            Result.append(List);
        }else{
            ParagraphNo = -1;
            BestParagraphSoFar = -1;
            BestSimSofar = 0;
            foreach(QList<CompVector> Paragraph, Result){
                ParagraphNo++;
                foreach(CompVector Sentence, Paragraph){
                    double sim = CompVector::Sim(Vector, Sentence);
                    qDebug("%f", sim);
                    if(sim > TRESHOLD && sim > BestSimSofar){
                        BestSimSofar = sim;
                        BestParagraphSoFar = ParagraphNo;
                    }
                }
            }
            if(BestParagraphSoFar >= 0){
                Result[BestParagraphSoFar].append(Vector);
            }else{
                QList<CompVector> List;
                List.append(Vector);
                Result.append(List);
            }
        }
    }
    return Result;
}

void MainWindow::on_actionSaveOutput_triggered()
{
    QFileDialog Dialog;
    QString Path = Dialog.getSaveFileName(this);
    if(Path.length() > 0){
        QFile File(Path);
        if(!File.open(QFile::WriteOnly)){
            QMessageBox Box;
            Box.setText("Unable to save!");
            Box.exec();
            return;
        }
        File.write(FinalResult.toUtf8());
        File.close();
    }
}

QString MainWindow::NormalizeToken(QString _token){
//    _token.remove(')');
//    _token.remove('(');
//    _token.remove('\'');
//    _token.remove('\"');
//    _token.remove('[');
//    _token.remove(']');
    if(IsValidWord(_token)){
        return _token;
    }
    if(ValidIfRemoved(_token, lang_an)){
        _token.chop(lang_an.length());
    }else if(ValidIfRemoved(_token, lang_at)){
        _token.chop(lang_at.length());
    }else if(ValidIfRemoved(_token, lang_ati)){
        _token.chop(lang_ati.length());
    }else if(ValidIfRemoved(_token, lang_ha)){
        _token.chop(lang_ha.length());
    }else if(ValidIfRemoved(_token, lang_haii)){
        _token.chop(lang_haii.length());
    }else if(ValidIfRemoved(_token, lang_in)){
        _token.chop(lang_in.length());
    }else if(ValidIfRemoved(_token, lang_ini)){
        _token.chop(lang_ini.length());
    }else if(ValidIfRemoved(_token, lang_yii)){
        _token.chop(lang_yii.length());
    }else if(ValidIfRemoved(_token, lang_ii)){
        _token.chop(lang_ii.length());
    }
    return _token.trimmed();
}


bool MainWindow::IsValidWord(QString _word)
{
//    QMessageBox B;
//    B.setText(_word + (Lang_Nouns.contains(_word) ? " Yes" : " No"));
//    B.exec();

    if(Lang_Nouns.contains(_word))
        return true;
    return false;
}

bool MainWindow::ValidIfRemoved(QString _token, QString _affix)
{
    if(_token.endsWith(_affix)){
        if(IsValidWord(_token.left(_token.length() - _affix.length()).trimmed())){
            return true;
        }
        return false;
    }
    return false;
}
