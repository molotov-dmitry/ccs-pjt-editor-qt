#ifndef DIALOGLISTEDITOR_H
#define DIALOGLISTEDITOR_H

#include <QDialog>

namespace Ui {
class DialogListEditor;
}

class DialogListEditor : public QDialog
{
    Q_OBJECT

public:
    explicit DialogListEditor(QWidget *parent = nullptr);
    ~DialogListEditor();

private:
    Ui::DialogListEditor *ui;
};

#endif // DIALOGLISTEDITOR_H
