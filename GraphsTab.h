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
#include <QApplication>

#include "ReportUnit.h"

class ChartView: public QChartView
{
    Q_OBJECT

public:
    ChartView(QChart* chart, QWidget *parent = 0)
        : QChartView(chart, parent)
    {
        setDragMode(QGraphicsView::NoDrag);
        this->setMouseTracking(true);

        axisX = std::make_unique<QValueAxis>();
        axisY = std::make_unique<QValueAxis>();

        axisX->setRange(0, xmax);
        axisY->setRange(0, ymax);
    }

    void updateXmax(const uint64_t& new_max) { xmax = new_max; }
    void updateYmax(const uint64_t& new_max) { ymax = new_max; }
    const uint64_t& getXmax() const { return xmax; }
    const uint64_t& getYmax() const { return ymax; }

    QAbstractAxis* getXaxis() const { return axisX.get(); }
    QAbstractAxis* getYaxis() const { return axisY.get(); }

protected:
    void mousePressEvent(QMouseEvent *event) override {
        if (event->button() == Qt::LeftButton) {
            m_dragging = true;
            m_lastMousePos = event->pos();
            setDragMode(QGraphicsView::ScrollHandDrag);
        }
        QChartView::mousePressEvent(event);
    }

    void mouseMoveEvent(QMouseEvent *event) override {
        if (m_dragging) {
            int dx = event->pos().x() - m_lastMousePos.x();
            int dy = event->pos().y() - m_lastMousePos.y();
            chart()->scroll(-dx, dy);
            m_lastMousePos = event->pos();
        }
        QChartView::mouseMoveEvent(event);
    }

    void mouseReleaseEvent(QMouseEvent *event) override {
        if (event->button() == Qt::LeftButton) {
            m_dragging = false;
        }
        else if (event->button() == Qt::RightButton) {
            /* Autoscale axis after RMB clicked */
            chart()->zoomReset();

            auto *axisX = chart()->axes(Qt::Horizontal).front();
            auto *axisY = chart()->axes(Qt::Vertical).front();

            axisX->setRange(0, xmax + 1);
            axisY->setRange(0, ymax + 1);
        }
        QChartView::mouseReleaseEvent(event);
    }

    void wheelEvent(QWheelEvent *event) override {
        qreal factor = event->angleDelta().y() < 0? 0.95: 1.05;
        chart()->zoom(factor);
        event->accept();
        QChartView::wheelEvent(event);
    }

private:
    bool m_dragging = false;
    QPoint m_lastMousePos;

    std::unique_ptr<QValueAxis> axisX;
    std::unique_ptr<QValueAxis> axisY;

    uint64_t xmax = 5;
    uint64_t ymax = 30;
};

class GraphsTab : public QWidget
{
    Q_OBJECT
public:
    explicit GraphsTab(QWidget *parent = nullptr);
    ~GraphsTab();

    Q_SLOT void addDataToChart(const ReportUnit& unit);

private:
    Q_SLOT void onCheckBoxStateChanged(const Qt::CheckState& state);

public:
    std::unique_ptr<QChart> m_chart;
    std::unique_ptr<ChartView> m_chartView;
    std::array<QLineSeries*, 6> test_series;
    std::array<QCheckBox*, 6> cb_array;
};

#endif // GRAPHSTAB_H
