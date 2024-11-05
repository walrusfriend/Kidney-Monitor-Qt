#ifndef GRAPHSTAB_H
#define GRAPHSTAB_H

#include <QObject>
#include <QWidget>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QCheckBox>

class GraphsTab : public QWidget
{
    Q_OBJECT
public:
    explicit GraphsTab(QWidget *parent = nullptr);
    ~GraphsTab();

private:
    Q_SLOT void onCheckBoxStateChanged(const Qt::CheckState& state);

private:
    std::unique_ptr<QLineSeries> m_series;
    std::unique_ptr<QChart> m_chart;
    std::unique_ptr<QChartView> m_chartView;

    std::array<QLineSeries*, 6> test_series;
    std::array<QCheckBox*, 6> cb_array;

    enum DisplayedOptions {
        PRESSURE = 0,
        PERFUSION_SPEED,
        RESISTANCE,
        REGIME,
        TEMP1,
        TEMP2
    };

    std::unordered_map<DisplayedOptions, QString> graphs_names_map {
        {DisplayedOptions::PRESSURE, "Давление"},
        {DisplayedOptions::PERFUSION_SPEED, "Скорость перфузии"},
        {DisplayedOptions::RESISTANCE, "Сопротивление"},
        {DisplayedOptions::REGIME, "Режим работы"},
        {DisplayedOptions::TEMP1, "Температура 1"},
        {DisplayedOptions::TEMP2, "Температура 2"}
    };
};

#endif // GRAPHSTAB_H
