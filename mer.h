#ifndef MER_H
#define MER_H

#include <QDialog>
#include <QList>
#include "csegy.h"
#include "Zoomer.h"


namespace Ui {
class Mer;
}

class Mer : public QDialog
{
    Q_OBJECT

public:
    explicit Mer(QWidget *parent = 0);
    ~Mer();

private:
    Ui::Mer *ui;



public:
    void SetWindowLength();
    void LoadData(); //获取要处理的道数据
    void PlotTrace(int num);
    void CalculateMer(int traceNo, int traceLength); //计算给定时窗内的能量比+改进能量比
    void CalculateStaLta(int traceNo, int traceLength); //计算给定时窗长短时窗能量比STA/LTA
    void GetMaxIndex(QVector<double> traceData, int traceLength);
    void SortData(); //冒泡排序

    void ClearData();
    void ClearMerCurve();
    void ClearErCurve();

    void PlotEr(); //绘制能量比数据
    void PlotMer(); //绘制改进能量比数据
    void PlotStaLta(); //绘制长短时窗能量比STA/LTA



private:
    CSegy m_segy;
    Zoomer *m_pZoomer;

private:
    QVector<double> vectorX;
    QVector<QVector<double>*> m_Traces; //一个SEGY文件中所有道的数据
    int m_nTotalTraceNum;
    int m_nTraceLen;
    int m_nTraceNo;
    double absMax[100];

    int plotType; // 0 = 能量比法；1 = 改进能量比法；2 = STA/LTA长短时窗平均能量比值法

    int m_nWindowLen; //能量比时窗长度
    int m_nWindowLenSta; //短时窗
    int m_nWindowLenLta; //长时窗

    double m_Max; //单道最大采样值
    int m_Index;
    QList <double> m_TraceData;
    QList <double> m_EnergyValue;
    QList<double> m_StaLta;
    QList <int> m_SortIndex;

    int m_traceLength;
    int m_SortLength;


    // int m_nTraceNo;

private slots:
    void OnBtnOpenClicked();
    void OnBtnCalculateClicked();
    void OnRadioErClicked();
    void OnRadioMerClicked();
    void OnRadioStaLtaClicked();
    void OnBtnClearClicked();
    void OnSpinTraceNoChanged();




};

#endif // MER_H
