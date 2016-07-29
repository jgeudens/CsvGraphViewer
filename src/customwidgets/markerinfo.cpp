
#include "guimodel.h"
#include "graphdatamodel.h"

#include "util.h"
#include "markerinfoitem.h"
#include "markerinfo.h"

MarkerInfo::MarkerInfo(QWidget *parent) : QFrame(parent)
{
    _pLayout = new QVBoxLayout();

    setContentsMargins(2, 2, 2, 2); // This is redundant with setMargin, which is deprecated

    _pTimeDataLabel = new QLabel("", this);
    _pLayout->addWidget(_pTimeDataLabel);
    
    for (quint32 idx = 0; idx < graphMarkerCount; idx++)
    {
        MarkerInfoItem * pGraphMarkerData = new MarkerInfoItem();

        graphMarkerData.append(pGraphMarkerData);
        _pLayout->addWidget(pGraphMarkerData);
    }

    setLayout(_pLayout);
}

void MarkerInfo::setModel(GuiModel * pGuiModel, GraphDataModel * pGraphDataModel)
{
    _pGuiModel = pGuiModel;
    _pGraphDataModel = pGraphDataModel;

    connect(_pGuiModel, SIGNAL(startMarkerPosChanged()), this, SLOT(updateMarkerData()));
    connect(_pGuiModel, SIGNAL(endMarkerPosChanged()), this, SLOT(updateMarkerData()));

    updateMarkerData();

    for (quint32 idx = 0; idx < graphMarkerCount; idx++)
    {
        graphMarkerData[idx]->setModel(_pGuiModel, _pGraphDataModel);
    }
}

void MarkerInfo::updateMarkerData()
{
    QString timeData = QString(
                "<b>Time 1</b>: %0<br />"
                "<b>Time 2</b>: %1<br />"
                "<b>Time Diff</b>: %2<br />"
                )
                .arg(Util::formatTime(_pGuiModel->startMarkerPos(), false))
                .arg(Util::formatTime(_pGuiModel->endMarkerPos(), false))
                .arg(Util::formatTimeDiff(_pGuiModel->endMarkerPos() - _pGuiModel->startMarkerPos()));

    _pTimeDataLabel->setText(timeData);
}
