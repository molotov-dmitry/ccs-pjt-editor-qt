#ifndef LISTEDITOR_H
#define LISTEDITOR_H

#include <QWidget>

namespace Ui {
class ListEditor;
}

class ListEditor : public QWidget
{
    Q_OBJECT

public:
    explicit ListEditor(QWidget *parent = nullptr);
    ~ListEditor();

    void setText(const QString& text);
    QString text() const;

    QStringList items() const;
    void setItems(const QStringList& items);

    void clear();

    void setDragEnabled(bool enabled);
    bool dragEnabled() const;

    void setReadOnly(bool readOnly);
    bool isReadOnly() const;

    void setPlaceholderText(const QString& text);
    QString placeholderText() const;

    void setClearButtonEnabled(bool enabled);
    bool isClearButtonEnabled() const;

    void setSeparator(QChar sep);
    QChar separator() const;

signals:

    void listUpdated();
    void textChanged(QString);

private slots:
    void on_buttonEditAsList_clicked();

private:
    Ui::ListEditor *ui;

    QChar mSeparator;
};

#endif // LISTEDITOR_H
