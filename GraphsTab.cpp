#include "GraphsTab.h"

/* TODO: Delete */
#include <QRandomGenerator>

GraphsTab::GraphsTab(QWidget *parent)
    : QWidget{parent}
{
    /* Draw test graph */
    std::array<QLineSeries*, 6> test_series;

    m_chart = std::make_unique<QChart>();

    for (int i = 0; i < test_series.size(); ++i) {
        test_series[i] = new QLineSeries(this);
        for (int j = 0; j < 10; ++j) {
            test_series[i]->append(j, QRandomGenerator::global()->bounded(1, 50));
        }
        test_series[i]->setName(graphs_names_map.at(static_cast<DisplayedOptions>(i)));

        m_chart->addSeries(test_series[i]);
    }

    test_series[0]->setColor(QColor("red"));
    test_series[1]->setColor(QColor("red"));
    test_series[2]->setColor(QColor("red"));
    test_series[3]->setColor(QColor("red"));
    test_series[4]->setColor(QColor("red"));
    test_series[5]->setColor(QColor("red"));

    // m_chart->addSeries(m_series.get());
    // m_chart->addSeries(m_series2);
    m_chart->createDefaultAxes();
    m_chart->setTitle("График");


    m_chartView = std::make_unique<QChartView>(m_chart.get());
    m_chartView->setRenderHint(QPainter::Antialiasing);

    /* Create UI */
    QVBoxLayout* main_layout = new QVBoxLayout(this);
    QHBoxLayout* checkbox_layout = new QHBoxLayout(this);

    for (int i = 0; i < cb_array.size(); ++i) {
        cb_array[i] = new QCheckBox(this);
        checkbox_layout->addWidget(cb_array[i]);

        /* Apply settings for all check boxes if we can */
        cb_array[i]->setCheckState(Qt::Checked);
        cb_array[i]->setText(graphs_names_map.at(static_cast<DisplayedOptions>(i)));
    }

    // cb_array[0]->setText("Давление");
    // cb_array[1]->setText("Скорость перфузии");
    // cb_array[2]->setText("Сопротивление");
    // cb_array[3]->setText("Режим работы");
    // cb_array[4]->setText("Температура 1");
    // cb_array[5]->setText("Температура 2");

    main_layout->addLayout(checkbox_layout);
    main_layout->addWidget(m_chartView.get());
    setLayout(main_layout);
}
