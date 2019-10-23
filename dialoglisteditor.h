#ifndef DIALOGLISTEDITOR_H
#define DIALOGLISTEDITOR_H

#include <QDialog>

namespace Ui {
class DialogListEditor;
}

class QListWidgetItem;

class DialogListEditor : public QDialog
{
    Q_OBJECT

public:
    explicit DialogListEditor(QWidget *parent = nullptr);
    ~DialogListEditor();

    void setItemList(const QString& list, QChar sep = ';');
    QString itemList(QChar sep = ';') const;

private slots:
    void on_buttonAdd_clicked();

    void on_buttonRemove_clicked();

    void on_buttonMoveUp_clicked();

    void on_buttonMoveDown_clicked();

    void on_list_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

    void on_list_itemSelectionChanged();

private:
    Ui::DialogListEditor *ui;
};

#endif // DIALOGLISTEDITOR_H
