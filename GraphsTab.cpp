#include "GraphsTab.h"

/* TODO: Delete */
#include <QRandomGenerator>

GraphsTab::GraphsTab(QWidget *parent)
    : QWidget{parent}
{
    /* Draw test graph */
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
    test_series[1]->setColor(QColor("magenta"));
    test_series[2]->setColor(QColor("purple"));
    test_series[3]->setColor(QColor("green"));
    test_series[4]->setColor(QColor("blue"));
    test_series[5]->setColor(QColor("black"));

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

        connect(cb_array[i], &QCheckBox::checkStateChanged,
                this, &GraphsTab::onCheckBoxStateChanged, Qt::QueuedConnection);
    }

    main_layout->addLayout(checkbox_layout);
    main_layout->addWidget(m_chartView.get());
    setLayout(main_layout);

    /* Connect check boxes */

}

GraphsTab::~GraphsTab() {
    for (auto cb : cb_array) {
        delete cb;
    }
}

void GraphsTab::onCheckBoxStateChanged(const Qt::CheckState& state)
{
    QCheckBox* cb = qobject_cast<QCheckBox*>(sender());

    auto&& it = std::find(cb_array.begin(), cb_array.end(), cb);

    if (it == cb_array.end()) {
        qDebug() << "ERROR - GraphsTab::onCheckBoxStateChanged - Can't find check box in checkbox array";
        return;
    }

    const int&& cb_index = std::distance(cb_array.begin(), it);

    if (state) {
        m_chart->addSeries(test_series[cb_index]);
    }
    else {
        m_chart->removeSeries(test_series[cb_index]);
    }

}
