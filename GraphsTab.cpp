#include "GraphsTab.h"
#include "Data_Types.h"

/* TODO: Delete */
#include <QRandomGenerator>
#include <QAbstractAxis>

GraphsTab::GraphsTab(QWidget *parent)
    : QWidget{parent}
{
    /* Draw test graph */
    m_chart = std::make_unique<QChart>();
    m_chartView = std::make_unique<ChartView>(m_chart.get(), this);

    m_chart->addAxis(m_chartView->getXaxis(), Qt::AlignBottom);
    m_chart->addAxis(m_chartView->getYaxis(), Qt::AlignLeft);

    /* Setup Line Series for incoming data */
    for (size_t i = 0; i < test_series.size(); ++i) {
        test_series[i] = new QLineSeries(this);

        m_chart->addSeries(test_series[i]);

        test_series[i]->setName(Graphs::graphs_names_map.at(static_cast<Graphs::DisplayedOptions>(i)));        
        test_series[i]->attachAxis(m_chartView->getXaxis());
        test_series[i]->attachAxis(m_chartView->getYaxis());
    }

    test_series[0]->setColor(QColor("red"));
    test_series[1]->setColor(QColor("magenta"));
    test_series[2]->setColor(QColor("purple"));
    test_series[3]->setColor(QColor("green"));
    test_series[4]->setColor(QColor("blue"));
    test_series[5]->setColor(QColor("black"));

    m_chart->setTitle("График");

    m_chartView->setRenderHint(QPainter::Antialiasing);
    m_chartView->setDragMode(QGraphicsView::ScrollHandDrag);
    m_chartView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_chartView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    /* Create UI */
    QVBoxLayout* main_layout = new QVBoxLayout(this);
    QHBoxLayout* checkbox_layout = new QHBoxLayout(this);

    for (size_t i = 0; i < cb_array.size(); ++i) {
        cb_array[i] = new QCheckBox(this);
        checkbox_layout->addWidget(cb_array[i]);

        /* Apply settings for all check boxes if we can */
        cb_array[i]->setCheckState(Qt::Checked);
        cb_array[i]->setText(Graphs::graphs_names_map.at(static_cast<Graphs::DisplayedOptions>(i)));

        /* Connect check boxes */
        connect(cb_array[i], &QCheckBox::checkStateChanged,
                this, &GraphsTab::onCheckBoxStateChanged, Qt::QueuedConnection);
    }

    main_layout->addLayout(checkbox_layout);
    main_layout->addWidget(m_chartView.get());
    setLayout(main_layout);
}

GraphsTab::~GraphsTab() {
    for (auto cb : cb_array) {
        delete cb;
    }

    for (auto s : test_series) {
        delete s;
    }
}

void GraphsTab::addDataToChart(const ReportUnit &unit)
{

    uint64_t current_time_sec = unit.time.hour() * 60 * 60 +
                                unit.time.minute() * 60 +
                                unit.time.second();

    qDebug() << current_time_sec;

    /* TODO: Maybe add slicing window for N element to display */
    // if (test_series[Graphs::DisplayedOptions::PRESSURE]->count() > 50) {
    //     for (auto& elem : test_series)
    //         elem->remove(0);
    // }

    /* Add data to graph */
    test_series[Graphs::DisplayedOptions::PRESSURE]->
        append(current_time_sec, unit.fill_value);

    test_series[Graphs::DisplayedOptions::PERFUSION_SPEED]->
        append(current_time_sec, unit.flow);

    test_series[Graphs::DisplayedOptions::REGIME]->
        append(current_time_sec, unit.regime);

    test_series[Graphs::DisplayedOptions::RESISTANCE]->
        append(current_time_sec, unit.resistance);

    test_series[Graphs::DisplayedOptions::TEMP1]->
        append(current_time_sec, unit.temp1);

    test_series[Graphs::DisplayedOptions::TEMP2]->
        append(current_time_sec, unit.temp2);

    /* Update axes */
    m_chartView->updateXmax(std::max(m_chartView->getXmax(), current_time_sec));
    m_chartView->getXaxis()->setRange(0, m_chartView->getXmax());

    std::vector<float> vec_to_compare = {unit.fill_value, unit.flow, unit.temp1, unit.temp2};
    uint64_t curr_max = static_cast<uint64_t>(*std::max_element(vec_to_compare.begin(), vec_to_compare.end()));
    const uint64_t& old_max = m_chartView->getYmax();
    if (curr_max > old_max - 1) {
        m_chartView->updateYmax(curr_max + 1);
        m_chartView->getYaxis()->setRange(0, curr_max + 1);
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
        m_chart->axes()[0]->setRange(0, m_chartView->getXmax());
        m_chart->axes()[1]->setRange(0, m_chartView->getYmax());
        test_series[cb_index]->attachAxis(m_chartView->getXaxis());
        test_series[cb_index]->attachAxis(m_chartView->getYaxis());
    }
    else {
        m_chart->removeSeries(test_series[cb_index]);
    }

}
