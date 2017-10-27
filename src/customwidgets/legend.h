#ifndef LEGEND_H
#define LEGEND_H

#include <QFrame>
#include <QVBoxLayout>
#include <QMenu>

#include "legenditem.h"

/* Forward declaration */
class GuiModel;
class GraphDataModel;
class BasicGraphView;

class Legend : public QFrame
{
    Q_OBJECT
public:
    explicit Legend(QWidget *parent = 0);
    ~Legend();

    void setModels(GuiModel *pGuiModel, GraphDataModel * pGraphDataModel);
    void setGraphview(BasicGraphView * pGraphView);

    void mousePressEvent(QMouseEvent * event);
    void mouseDoubleClickEvent(QMouseEvent * event);

signals:

public slots:
    void updateDataInLegend();

private slots:
    void updateLegend();
    void showGraph(quint32 graphIdx);
    void changeGraphColor(const quint32 graphIdx);
    void changeGraphLabel(const quint32 graphIdx);
    void showContextMenu(const QPoint& pos);
    void toggleVisibilityClicked();
    void hideAll();
    void showAll();

private:
    void updateCursorDataInLegend(QStringList &cursorValueList);
    void addItem(quint32 graphIdx);
    qint32 itemUnderCursor();
    void toggleItemVisibility(qint32 idx);

    qint32 _popupMenuItem;

    // Models
    GuiModel * _pGuiModel;
    GraphDataModel * _pGraphDataModel;

    BasicGraphView * _pGraphView;

    // Widgets
    QVBoxLayout * _pLayout;
    QLabel * _pNoGraphs;
    QList<LegendItem *> _items;

    QMenu * _pLegendMenu;
    QAction * _pToggleVisibilityAction;
    QAction * _pHideAllAction;
    QAction * _pShowAllAction;
};

#endif // LEGEND_H
