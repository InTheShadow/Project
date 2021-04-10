#ifndef SETTINGWINDOW_H
#define SETTINGWINDOW_H

#include <QMainWindow>
#include "gameconfig.h"

namespace Ui {
class SettingWindow;
}

class SettingWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit SettingWindow(QWidget *parent = 0);
    ~SettingWindow();
    void setGameConfigPtr(GameConfig* gameConfigPtr){this->gameConfigPtr = gameConfigPtr;}
    void createContents();
signals:
    void backFromSettings();
private slots:
    void on_returnBtn_clicked();
    void on_defaultBtn_clicked();
    void on_musicBtn_clicked();
    void on_musicPath_textChanged(const QString &arg1);
    void on_bgiBtn_clicked();
    void on_bgiPath_textChanged(const QString &arg1);
    void on_whiteBtn_clicked();
    void on_whitePath_textChanged(const QString &arg1);
    void on_blackBtn_clicked();
    void on_blackPath_textChanged(const QString &arg1);
    void on_undoTimesEnter_textChanged(const QString &arg1);
    void undoTimesChanged(int value);
    void countTimeChanged(int value);
    void AIDelayChanged(int value);
    void on_countTimeEnter_textChanged(const QString &arg1);
    void on_AIDelayEnter_textChanged(const QString &arg1);

private:
    Ui::SettingWindow *ui;
    GameConfig* gameConfigPtr;
};

#endif // SETTINGWINDOW_H
