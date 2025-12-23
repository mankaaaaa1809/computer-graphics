// main.cpp
#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <vector>
#include <cmath>

using namespace std;

// Вершины буквы "М"
vector<vector<float>> vertices = {
    {2, 2, 2}, {4, 2, 2}, {8, 2, 2}, {10, 2, 2}, {5, 5, 2}, {7, 5, 2},
    {4, 7, 2}, {6, 7, 2}, {8, 7, 2}, {2, 10, 2}, {5, 10, 2}, {7, 10, 2}, {10, 10, 2},
    {2, 2, 4}, {4, 2, 4}, {8, 2, 4}, {10, 2, 4}, {5, 5, 4}, {7, 5, 4},
    {4, 7, 4}, {6, 7, 4}, {8, 7, 4}, {2, 10, 4}, {5, 10, 4}, {7, 10, 4}, {10, 10, 4}
};

// Рёбра
vector<pair<int, int>> edges = {
    {0, 1}, {1, 4}, {4, 6}, {6, 9}, {9, 10}, {10, 11}, {11, 12}, {12, 3}, {3, 2}, {2, 5},
    {5, 7}, {7, 8}, {8, 11}, {1, 13}, {13, 14}, {14, 17}, {17, 19}, {19, 21}, {21, 22},
    {22, 23}, {23, 24}, {24, 15}, {15, 16}, {16, 18}, {18, 20}, {20, 23}, {0, 13}, {9, 21},
    {12, 24}, {3, 15}, {2, 16}, {5, 18}, {7, 20}, {8, 20}, {6, 19}, {4, 17}, {10, 22}, {11, 23}
};

class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions {
    Q_OBJECT
private:
    float scaleFactor = 1.0f;
    float rotateX = 30.0f;
    float rotateY = -45.0f;
    QPoint lastMousePos;

public:
    GLWidget(QWidget* parent = nullptr) : QOpenGLWidget(parent) {
        setFocusPolicy(Qt::StrongFocus);
    }

    void setScale(float scale) {
        scaleFactor = scale;
        update();
    }

protected:
    void initializeGL() override {
        initializeOpenGLFunctions();
        glClearColor(0.95f, 0.95f, 0.98f, 1.0f);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_LINE_SMOOTH);
        glEnable(GL_POINT_SMOOTH);
        glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
        glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
    }

    void resizeGL(int w, int h) override {
        glViewport(0, 0, w, h);
    }

    void paintGL() override {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(45.0, (double)width()/height(), 0.1, 100.0);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        gluLookAt(15, 15, 15, 0, 0, 0, 0, 1, 0);
        glRotatef(rotateX, 1, 0, 0);
        glRotatef(rotateY, 0, 1, 0);

        drawAxes();
        drawLetter();
    }

    void drawAxes() {
        glLineWidth(2.0);

        glBegin(GL_LINES);
        glColor3f(1.0, 0.0, 0.0);
        glVertex3f(0, 0, 0);
        glVertex3f(10, 0, 0);

        glColor3f(0.0, 1.0, 0.0);
        glVertex3f(0, 0, 0);
        glVertex3f(0, 10, 0);

        glColor3f(0.0, 0.0, 1.0);
        glVertex3f(0, 0, 0);
        glVertex3f(0, 0, 10);
        glEnd();

        glLineWidth(1.0);
    }

    void drawLetter() {
        glLineWidth(2.0);
        glBegin(GL_LINES);
        glColor3f(0.2, 0.2, 0.8);

        for (const auto& edge : edges) {
            int v1 = edge.first;
            int v2 = edge.second;
            glVertex3f(vertices[v1][0] * scaleFactor, vertices[v1][1] * scaleFactor, vertices[v1][2] * scaleFactor);
            glVertex3f(vertices[v2][0] * scaleFactor, vertices[v2][1] * scaleFactor, vertices[v2][2] * scaleFactor);
        }
        glEnd();

        glPointSize(6.0);
        glBegin(GL_POINTS);
        glColor3f(1.0, 0.0, 0.0);

        for (const auto& vertex : vertices) {
            glVertex3f(vertex[0] * scaleFactor, vertex[1] * scaleFactor, vertex[2] * scaleFactor);
        }
        glEnd();

        glLineWidth(1.0);
    }

    void mousePressEvent(QMouseEvent* event) override {
        if (event->button() == Qt::LeftButton) {
            lastMousePos = event->pos();
        }
    }

    void mouseMoveEvent(QMouseEvent* event) override {
        if (event->buttons() & Qt::LeftButton) {
            float dx = event->x() - lastMousePos.x();
            float dy = event->y() - lastMousePos.y();

            rotateY += dx * 0.5f;
            rotateX += dy * 0.5f;

            lastMousePos = event->pos();
            update();
        }
    }

    void wheelEvent(QWheelEvent* event) override {
        float delta = event->angleDelta().y() > 0 ? 1.1f : 0.9f;
        scaleFactor *= delta;
        emit scaleChanged(scaleFactor);
        update();
    }

signals:
    void scaleChanged(float scale);
};

class MainWindow : public QMainWindow {
    Q_OBJECT
private:
    GLWidget* glWidget;
    QLineEdit* scaleInput;
    QLabel* currentScaleLabel;

public:
    MainWindow() {
        setWindowTitle("Лабораторная работа 6 - 3D Буква М (Qt + OpenGL)");
        resize(1200, 700);

        QWidget* centralWidget = new QWidget(this);
        setCentralWidget(centralWidget);

        QHBoxLayout* mainLayout = new QHBoxLayout(centralWidget);

        glWidget = new GLWidget(this);
        mainLayout->addWidget(glWidget, 3);

        QWidget* controlPanel = new QWidget(this);
        controlPanel->setStyleSheet("background-color: #d8d8e0;");
        controlPanel->setFixedWidth(400);

        QVBoxLayout* panelLayout = new QVBoxLayout(controlPanel);

        QLabel* titleLabel = new QLabel("Масштабирование");
        titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; margin: 10px;");
        panelLayout->addWidget(titleLabel);

        QLabel* inputLabel = new QLabel("Коэффициент масштабирования:");
        inputLabel->setStyleSheet("margin: 10px;");
        panelLayout->addWidget(inputLabel);

        scaleInput = new QLineEdit("1.0");
        scaleInput->setStyleSheet("padding: 5px; margin: 10px;");
        scaleInput->setValidator(new QDoubleValidator(0.01, 100.0, 2, this));
        panelLayout->addWidget(scaleInput);

        QPushButton* applyButton = new QPushButton("Применить");
        applyButton->setStyleSheet("padding: 10px; margin: 10px; background-color: #7a7a8c; color: white;");
        panelLayout->addWidget(applyButton);

        currentScaleLabel = new QLabel("Текущий масштаб: 1.00");
        currentScaleLabel->setStyleSheet("margin: 10px; font-weight: bold;");
        panelLayout->addWidget(currentScaleLabel);

        QLabel* infoLabel = new QLabel("Информация:");
        infoLabel->setStyleSheet("margin: 10px; font-size: 14px;");
        panelLayout->addWidget(infoLabel);

        QLabel* verticesLabel = new QLabel("Вершин: 25");
        verticesLabel->setStyleSheet("margin: 5px 10px;");
        panelLayout->addWidget(verticesLabel);

        QLabel* edgesLabel = new QLabel("Рёбер: 38");
        edgesLabel->setStyleSheet("margin: 5px 10px;");
        panelLayout->addWidget(edgesLabel);

        QLabel* instructionLabel = new QLabel("Используйте:\n• ЛКМ + движение - вращение\n• Колесо мыши - масштабирование");
        instructionLabel->setStyleSheet("margin: 20px 10px; color: #8a2a2a;");
        panelLayout->addWidget(instructionLabel);

        panelLayout->addStretch();

        mainLayout->addWidget(controlPanel, 1);

        connect(applyButton, &QPushButton::clicked, this, &MainWindow::applyScale);
        connect(glWidget, &GLWidget::scaleChanged, this, &MainWindow::updateScaleDisplay);
        connect(scaleInput, &QLineEdit::returnPressed, this, &MainWindow::applyScale);
    }

private slots:
    void applyScale() {
        bool ok;
        float scale = scaleInput->text().toFloat(&ok);
        if (ok && scale > 0) {
            glWidget->setScale(scale);
            currentScaleLabel->setText(QString("Текущий масштаб: %1").arg(scale, 0, 'f', 2));
        } else {
            scaleInput->setText("1.0");
            glWidget->setScale(1.0);
            currentScaleLabel->setText("Текущий масштаб: 1.00");
        }
    }

    void updateScaleDisplay(float scale) {
        scaleInput->setText(QString::number(scale, 'f', 2));
        currentScaleLabel->setText(QString("Текущий масштаб: %1").arg(scale, 0, 'f', 2));
    }
};

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    MainWindow window;
    window.show();

    return app.exec();
}

#include "main.moc"
