#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "compvector.h"
#include  <QList>
#include <QSet>
namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::MainWindow *ui;
    QStringList     inputFileNames;
    QStringList     inputFileContents;
    QString         FinalResult;
    void updateList();
    QString browserEmptyHtml;
    QSet<QString>* loadIntoHash(QString _file);
    QList<CompVector> ReadAndEncodeSentences(QList<QString>, QHash<int, QString>&);
    QList<QList<CompVector> > ConstructParagraphs(QList<CompVector>);
    QString NormalizeToken(QString);
    bool IsValidWord(QString);
    bool ValidIfRemoved(QString _token, QString _affix);
    QString lang_ha, lang_at, lang_an, lang_in, lang_ii, lang_haii, lang_ati, lang_ini, lang_yii;
    QSet<QString> Lang_Nouns;
private slots:
    void on_actionSaveOutput_triggered();
    void on_actionProcess_triggered();
    void on_actionDeleteFileFromInput_triggered();
    void on_actionAddFilesToInput_triggered();
    void on_action_2_triggered();
    void inputFileIndexChanged(int);
};

#endif // MAINWINDOW_H
