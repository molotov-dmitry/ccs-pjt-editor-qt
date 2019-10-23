#ifndef DIALOGLINKORDEREDITOR_H
#define DIALOGLINKORDEREDITOR_H

#include <QDialog>

#include <QAbstractItemModel>

class QListWidget;
class QListWidgetItem;

namespace Ui {
class DialogLinkOrderEditor;
}

class DialogLinkOrderEditor : public QDialog
{
    Q_OBJECT

public:
    explicit DialogLinkOrderEditor(QWidget *parent = nullptr);
    ~DialogLinkOrderEditor();

    void addUnordered(const QString& file);
    void addOrdered(const QString& file, int order);

private slots:
    void on_buttonAdd_clicked();

    void on_buttonRemove_clicked();

    void on_buttonMoveUp_clicked();

    void on_buttonMoveDown_clicked();

    void on_listUnordered_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

    void on_listOrdered_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

    void on_listOrdered_itemDoubleClicked(QListWidgetItem *item);

    void on_listUnordered_itemDoubleClicked(QListWidgetItem *item);

    void updateList();

private:
    Ui::DialogLinkOrderEditor *ui;

    void moveCurrentItem(QListWidget* from, QListWidget* to);
};

#endif // DIALOGLINKORDEREDITOR_H
