#include "mer.h"
#include "ui_mer.h"
#include <qwt_plot_canvas.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_magnifier.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_curve.h>
#include <qwt_legend.h>
#include <QFileDialog>
#include "util.h"
#include <math.h>
#include <QMessageBox>

Mer::Mer(QWidget *parent) :
    QDialog(parent),
    m_nWindowLen(100),
    m_nTraceNo(1),
    plotType(0),
    m_nWindowLenLta(100),
    m_nWindowLenSta(20),
    ui(new Ui::Mer)
{
    ui->setupUi(this);

    //-----------------------qwtPlot二维波形显示
    (void) new QwtPlotPanner (ui->PlotMer->canvas());
    (void) new QwtPlotMagnifier (ui->PlotMer->canvas());


    ui->PlotMer->enableAxis(QwtPlot::yLeft);
    ui->PlotMer->enableAxis(QwtPlot::yRight);
    ui->PlotMer->enableAxis(QwtPlot::xTop);
    ui->PlotMer->enableAxis(QwtPlot::xBottom);

    ui->PlotMer->setAxisTitle(ui->PlotMer->yLeft, tr("Amplitude"));
    ui->PlotMer->setAxisScale(ui->PlotMer->yLeft, -1.0,1.0);
    ui->PlotMer->setAxisScale(ui->PlotMer->yRight, -1.0,1.0);
    ui->PlotMer->setAxisTitle(ui->PlotMer->xBottom, tr("Samples"));
    ui->PlotMer->setAxisScale(ui->PlotMer->xBottom, 0,1000);

    ui->PlotMer->setLineWidth(1);
    ui->PlotMer->setFrameStyle(QFrame::Box| QFrame::Plain);

    QPalette canvasPalette1 (Qt::white);
    canvasPalette1.setColor(QPalette::Foreground, QColor (133,190,232));
    ui->PlotMer->canvas()->setPalette(canvasPalette1);

    QwtPlotMarker* pMarker;
    pMarker = new QwtPlotMarker();
    pMarker->setLineStyle(QwtPlotMarker::HLine);
    pMarker->setLinePen(QPen(Qt::black, 0, Qt::DotLine));
    pMarker->setYValue(0.0);
    pMarker->attach(ui->PlotMer);

    QwtLegend * legend = new QwtLegend;
    ui->PlotMer->insertLegend(legend, QwtPlot::BottomLegend);

    //-----------------------qwtPlot二维波形显示

    ui->spinTraceNo->setValue(1);
    ui->edtWindowLen->setText(tr("100"));
    ui->btnCaculate->setEnabled(true);
    ui->edtLongWindow->setEnabled(false);
    ui->edtShortWindow->setEnabled(false);
    ui->edtLongWindow->setText(tr("100"));
    ui->edtShortWindow->setText(tr("20"));

    ui->radioER->setChecked(true);
    ui->radioOverlap->setChecked(true);


    connect(ui->btnOpen, SIGNAL(clicked(bool)), this, SLOT(OnBtnOpenClicked()));
    connect(ui->btnCaculate,SIGNAL(clicked(bool)), this, SLOT(OnBtnCalculateClicked()));
    connect(ui->radioER, SIGNAL(clicked(bool)), this, SLOT(OnRadioErClicked()));
    connect(ui->radioMER, SIGNAL(clicked(bool)), this, SLOT(OnRadioMerClicked()));
    connect(ui->btnClear,SIGNAL(clicked(bool)),this, SLOT(OnBtnClearClicked()));
    connect(ui->spinTraceNo, SIGNAL(valueChanged(int)), this, SLOT(OnSpinTraceNoChanged()));
    connect(ui->radioStaLta, SIGNAL(clicked(bool)), this, SLOT(OnRadioStaLtaClicked()));




}

Mer::~Mer()
{
    ClearData();
    delete ui;
}

void Mer::SetWindowLength()
{
    m_nWindowLen=ui->edtWindowLen->text().toInt();
    m_nTraceNo=ui->spinTraceNo->value();
    m_nWindowLenLta = ui->edtLongWindow->text().toInt();
    m_nWindowLenSta = ui->edtShortWindow->text().toInt();
}



void Mer::LoadData()
{
    m_nTotalTraceNum=m_segy.getTotalTraceNumber();
    m_nTraceLen = m_segy.getSamplesNumber();

    QVector<double>* m_ChangeData;
    double sampleValue= 0.0;
    QVector <double> tempVector;
    float* pTrace[m_nTotalTraceNum];

    float j= 0.0;

    for (int i=0; i<m_nTraceLen; i++)
    {
        vectorX<<j;
        j+=1.0;

    }

    for (int i=0; i<m_nTotalTraceNum; i++)
    {

        m_ChangeData=new QVector<double>;
        m_ChangeData->clear();
        pTrace[i] = m_segy.GetTraceData(i+1);


        tempVector.clear();

        for (int j=0; j<m_nTraceLen; j++)
        {
            m_ChangeData->append(sampleValue);


        }


        for (int k=0; k<m_nTraceLen; k++)
        {
            tempVector<<(double)(pTrace[i])[k];

        }
        //地震道单道归一化，每个采样点振幅值除以该道最大振幅值的绝对值
        double absMin=0;
        GetVectorMax(tempVector, absMax[i]);

        for (int k=0; k<m_nTraceLen; k++)
        {

            tempVector[k] = ( tempVector[k]-absMin)/(absMax[i]-absMin);

            m_ChangeData->replace (k, tempVector[k]);


        }

        m_Traces<<m_ChangeData;


    }
}

void Mer::PlotTrace(int num)
{

    QwtPlotCurve* traceCurve = new QwtPlotCurve();
    ui->PlotMer->setAxisScale(ui->PlotMer->xBottom, 0, double(m_nTraceLen));
    ui->PlotMer->setAxisScale(ui->PlotMer->xTop, 0, double(m_nTraceLen));

    traceCurve->attach(ui->PlotMer);//设置曲线
    traceCurve->setSamples(vectorX,*m_Traces[num-1]);
    traceCurve->setRenderHint(QwtPlotItem::RenderAntialiased);
    traceCurve->setPen(QPen(Qt::blue));
    traceCurve->setTitle(tr("Trace"));

    //加入放大缩小所选择矩形区域
    int rectY = 2; //放大缩小矩形框纵坐标大小
    QRectF rect(0,-1,m_nTraceLen,rectY);

    //设置放大缩小zoomer功能
    QwtPlotCanvas* traceCanvas = new QwtPlotCanvas();
    QPalette canvasPalette1 (Qt::white);
    canvasPalette1.setColor(QPalette::Foreground, QColor (133,190,232));
    traceCanvas->setPalette(canvasPalette1);
    ui->PlotMer->setCanvas(traceCanvas);
    m_pZoomer = new Zoomer(traceCanvas,rect);

    m_pZoomer->setRect(rect);
    m_pZoomer->setZoomBase(rect);
    m_pZoomer->setMousePattern(QwtEventPattern::MouseSelect2,
                               Qt::RightButton,Qt::ControlModifier);
    m_pZoomer->setMousePattern(QwtEventPattern::MouseSelect3,
                               Qt::RightButton);
    m_pZoomer->setRubberBandPen(QPen(Qt::red, 2, Qt::DotLine));
    m_pZoomer->setTrackerMode(QwtPicker::AlwaysOff);
    m_pZoomer->setEnabled(true);



    ui->PlotMer->replot();
}

void Mer::CalculateMer(int traceNo, int traceLength)
{

    for (int i=0; i<traceLength; i++)
    {
        m_TraceData.append(0.0);
        m_TraceData.replace(i, (*m_Traces[traceNo-1])[i]);
    }


    int i,j,k;
    double m_Energy1,m_Energy2;
    double m_AverageEnergy;
    //计算单道的平均能量
    m_EnergyValue.clear();
    m_SortIndex.clear();
    m_AverageEnergy=0;

    if(traceLength<2*m_nWindowLen)
        traceLength=2*m_nWindowLen;

    for(i=0;i<traceLength;i++)
        m_AverageEnergy+=m_TraceData[i]*m_TraceData[i];
    m_AverageEnergy=m_AverageEnergy/traceLength;


    for (i=0,k=0;i<m_nTraceLen-2*m_nWindowLen+1;i++,k++)
    {
        m_Energy1=0;
        m_Energy2=0;
        for (j=i;j<i+m_nWindowLen;j++)
        {
            m_Energy1+=m_TraceData[j]*m_TraceData[j];
        }
        for (j=i+m_nWindowLen;j<i+2*m_nWindowLen;j++)
        {
            m_Energy2+=m_TraceData[j]*m_TraceData[j];
        }
        if (m_Energy1==0)
            m_Energy1=m_AverageEnergy;
        m_EnergyValue<<m_Energy2/m_Energy1;
        m_SortIndex<<i+m_nWindowLen; //能量比值对应第二个窗口的第一个节点

    }
    m_SortLength=k;
}


void Mer::CalculateStaLta(int traceNo, int traceLength)
{

    for (int i=0; i<traceLength; i++)
    {
        m_TraceData.append(0.0);
        m_TraceData.replace(i, (*m_Traces[traceNo-1])[i]);
    }


    int i,j,k;
    double m_Energy1,m_Energy2;
    double m_AverageEnergy;
    //计算单道的平均能量
    m_EnergyValue.clear();
    m_SortIndex.clear();
    m_AverageEnergy=0;

   if(traceLength<2*m_nWindowLenLta)
       traceLength=2*m_nWindowLenLta;

    for(i=0;i<traceLength;i++)
        m_AverageEnergy+=m_TraceData[i]*m_TraceData[i];
    m_AverageEnergy=m_AverageEnergy/traceLength;


    for (i=0,k=0;i<m_nTraceLen-m_nWindowLenLta+1;i++,k++)
    {
        m_Energy1=0; //长时窗LTA
        m_Energy2=0; //短时窗STA
        for (j=i;j<i+m_nWindowLenLta;j++)
        {
            m_Energy1+=m_TraceData[j]*m_TraceData[j];
        }
         m_Energy1 = m_Energy1/m_nWindowLenLta;

        for (j=i+m_nWindowLenLta-m_nWindowLenSta;j<i+m_nWindowLenLta;j++)
        {
            m_Energy2+=m_TraceData[j]*m_TraceData[j];

        }
        m_Energy2 = m_Energy2/m_nWindowLenSta;

        if (m_Energy1==0)
            m_Energy1=m_AverageEnergy;

        m_StaLta<<m_Energy2/m_Energy1;
        m_SortIndex<<i+m_nWindowLen; //能量比值对应第二个窗口的第一个节点

    }
    m_SortLength=k;
}

void Mer::GetMaxIndex(QVector<double> traceData, int traceLength)
{
    int i,j;
    float m_Temp;

    m_Max=-1;
    m_Index=-1;
    for (i=0;i<traceLength;i++)
    {
        m_Temp=traceData[i];
        m_Temp=(m_Temp>0)?m_Temp:-1*m_Temp;
        if (m_Temp>m_Max)
        {
            m_Max=m_Temp;
            m_Index=i;
        }
    }
}

void Mer::ClearData()
{

    if (!vectorX.isEmpty())
    {
        vectorX.clear();
    }
    int i,j;
    if (!m_Traces.isEmpty())
    {
        i=m_Traces.count();
        for(j=i-1;j>=0;j--)
            m_Traces[j]->clear();
        m_Traces.clear();

    }
    if (!m_EnergyValue.isEmpty())
    {
        for ( i =0; i<m_nTraceLen-2*m_nWindowLen+1; i++)
            m_EnergyValue.clear();
    }
    if (!m_StaLta.isEmpty())
    {
        for ( i =0; i<m_nTraceLen-m_nWindowLenLta+1; i++)
            m_StaLta.clear();
    }
}


void Mer::PlotMer()
{
    QVector<double> energyValue;
    QVector <double> newVectorX;
    QVector<double> mEnergyValue;
    for (int i =0; i<m_nTraceLen-2*m_nWindowLen+1; i++)
    {
        newVectorX.append(0.0);
        energyValue.append(0.0);
        mEnergyValue.append(0.0);
    }
    for (int j =0; j<m_nTraceLen-2*m_nWindowLen+1; j++)
    {
        newVectorX.replace(j,vectorX[m_nWindowLen+j]);
        energyValue.replace(j, m_EnergyValue[j]);
        mEnergyValue.replace(j,pow((energyValue[j]*abs((*m_Traces[m_nTraceNo])[j])),3.0));

    }

    //地震道单道归一化，每个采样点振幅值除以该道最大振幅值的绝对值
    double energyMin=0.0;
    double energyMax =0.0;
    double mEnergyMax = 0.0;
    GetVectorMax(energyValue, energyMax);
    GetVectorMax(mEnergyValue, mEnergyMax);

    for (int k =0; k<m_nTraceLen-2*m_nWindowLen+1; k++)
    {
        energyValue[k] = (energyValue[k]-energyMin)/energyMax;
        mEnergyValue[k] =(mEnergyValue[k]-energyMin)/mEnergyMax;
    }

    QwtPlotCurve* merCurve = new QwtPlotCurve();
    merCurve->attach(ui->PlotMer);//设置曲线
    merCurve->setSamples(newVectorX,mEnergyValue);
    merCurve->setRenderHint(QwtPlotItem::RenderAntialiased);
    merCurve->setPen(QPen(Qt::red,2));
    merCurve->setTitle(tr("Modified Energy Ratio"));

    ui->PlotMer->replot();
}

void Mer::PlotStaLta()
{
    QVector<double> staLtaValue;
    QVector <double> newVectorX;

    for (int i =0; i<m_nTraceLen-m_nWindowLenLta+1; i++)
    {
        newVectorX.append(0.0);
        staLtaValue.append(0.0);

    }

    for (int j =0; j<m_nTraceLen-m_nWindowLenLta+1; j++)
    {
        newVectorX.replace(j, vectorX[m_nWindowLenLta+j-1]); //一定不要忘记减1
        staLtaValue.replace(j, m_StaLta[j]);

    }

    //地震道单道归一化，每个采样点振幅值除以该道最大振幅值的绝对值
    double energyMin=0.0;
    double energyMax =0.0;
    GetVectorMax(staLtaValue, energyMax);

    for (int k =0; k<m_nTraceLen-m_nWindowLenLta+1; k++)
    {
        staLtaValue[k] = (staLtaValue[k]-energyMin)/energyMax;

    }

    QwtPlotCurve* merCurve = new QwtPlotCurve();
    merCurve->attach(ui->PlotMer); //设置曲线
    merCurve->setSamples(newVectorX,staLtaValue);
    merCurve->setRenderHint(QwtPlotItem::RenderAntialiased);
    merCurve->setPen(QPen(Qt::darkGreen,2));
    merCurve->setTitle(tr("STA/LTA"));

    ui->PlotMer->replot();
}

void Mer::PlotEr()
{
    QVector<double> energyValue;
    QVector <double> newVectorX;
    QVector<double> mEnergyValue;

    for (int i =0; i<m_nTraceLen-2*m_nWindowLen+1; i++)
    {
        newVectorX.append(0.0);
        energyValue.append(0.0);
        mEnergyValue.append(0.0);
    }
    for (int j =0; j<m_nTraceLen-2*m_nWindowLen+1; j++)
    {
        newVectorX.replace(j,vectorX[m_nWindowLen+j]);
        energyValue.replace(j, m_EnergyValue[j]);
        mEnergyValue.replace(j,pow((energyValue[j]*abs((*m_Traces[m_nTraceNo])[j])),3.0));

    }

    //地震道单道归一化，每个采样点振幅值除以该道最大振幅值的绝对值
    double energyMin=0.0;
    double energyMax =0.0;
    double mEnergyMax = 0.0;
    GetVectorMax(energyValue, energyMax);
    GetVectorMax(mEnergyValue, mEnergyMax);

    for (int k =0; k<m_nTraceLen-2*m_nWindowLen+1; k++)
    {
        energyValue[k] = (energyValue[k]-energyMin)/energyMax;
        mEnergyValue[k] =(mEnergyValue[k]-energyMin)/mEnergyMax;
    }

    QwtPlotCurve* merCurve = new QwtPlotCurve();
    merCurve->attach(ui->PlotMer);//设置曲线
    merCurve->setSamples(newVectorX, energyValue);
    merCurve->setRenderHint(QwtPlotItem::RenderAntialiased);
    merCurve->setPen(QPen(Qt::darkMagenta, 2));
    merCurve->setTitle(tr("Energy Ratio"));

    ui->PlotMer->replot();
}

void Mer::OnBtnOpenClicked()
{


    QString fileName = QFileDialog::getOpenFileName(
                this,
                tr("Select one SEG-Y file"),
                tr(""),
                tr("SEG-Y Files(*.sgy *.segy)")
                );

    if ( fileName .length()>0 )
    {
        ClearData();
        char szFilePath[2000];
        memset(szFilePath,0,2000);
        QStringData *data = fileName.data_ptr();
        ushort *s = data->data();

        for (int i = 0; i<data->size; i++ )
        {
            szFilePath[i] = s[i];
        }


        m_segy.OpenFile(szFilePath);
        ui->edtFileName->setText(fileName);
        //设置在窗口底部显示采样率
        float sampleRate = float(m_segy.getSamplesInterval())/1000;
        QString strSampleRate;
        strSampleRate.sprintf("%.2f", sampleRate);
        ui->lbSampleRate->setText(QString("Sample Rate: %1 ms.").arg(strSampleRate));
        LoadData();
        PlotTrace(m_nTraceNo);

    }
}

void Mer::OnBtnCalculateClicked()
{
    SetWindowLength();

    switch(plotType)
    {
    case 0:
    {
        CalculateMer(m_nTraceNo, m_nTraceLen);
        PlotEr();
        break;
    }
    case 1:
    {
        CalculateMer(m_nTraceNo, m_nTraceLen);
        PlotMer();
        break;
    }
    case 2:
    {
        CalculateStaLta(m_nTraceNo,m_nTraceLen);
        PlotStaLta();
        break;
    }
    }

}

void Mer::OnRadioErClicked()
{
    plotType = 0;
    ui->edtLongWindow->setEnabled(false);
    ui->edtShortWindow->setEnabled(false);
    ui->PlotMer->setTitle(tr("Energy Ratio"));
    ui->btnCaculate->setEnabled(true);
}

void Mer::OnRadioMerClicked()
{
    plotType = 1;
    ui->edtLongWindow->setEnabled(false);
    ui->edtShortWindow->setEnabled(false);
    ui->PlotMer->setTitle(tr("Modified Energy Ratio"));
    ui->btnCaculate->setEnabled(true);
}

void Mer::OnRadioStaLtaClicked()
{
    plotType = 2;
    ui->edtLongWindow->setEnabled(true);
    ui->edtShortWindow->setEnabled(true);
    ui->PlotMer->setTitle(tr("STA/LTA"));
    ui->btnCaculate->setEnabled(true);
}

void Mer::OnBtnClearClicked()
{
    if (!m_EnergyValue.isEmpty())
    {
        for (int i =0; i<m_nTraceLen-2*m_nWindowLen+1; i++)
            m_EnergyValue.clear();
    }
    ui->PlotMer->detachItems();
    ui->PlotMer->replot();
    PlotTrace(m_nTraceNo);
}

void Mer::OnSpinTraceNoChanged()
{
    m_nTraceNo = ui->spinTraceNo->value();
    if (m_nTraceNo > m_nTotalTraceNum)
    {
        QMessageBox::information(this, tr("Trace Number Error"),QString("Total trace number is:\n    %1").arg(m_nTotalTraceNum));
        m_nTraceNo = m_nTotalTraceNum;
        ui->spinTraceNo->setValue(m_nTraceNo);
        return;
    }

}
