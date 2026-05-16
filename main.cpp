#include <QApplication>
#include <QWidget>
#include <QSlider>
#include <QLabel>
#include <QVBoxLayout>
#include <QComboBox>
#include <QSpinBox>
#include <QMessageBox>
#include <QDir>
#include <QFile>
#include <QTextStream>

#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/hidraw.h>

QString getHidrawName(const QString &deviceName) {
    QString ueventPath = QString("/sys/class/hidraw/%1/device/uevent").arg(deviceName);
    QFile file(ueventPath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString line = in.readLine();
            if (line.startsWith("HID_NAME=")) {
                return line.section('=', 1); // Returns everything after the '='
            }
        }
    }
    return "Unknown Device";
}

QStringList getHidRawDevices() {
    // Scan for available HIDs
    QDir devDir("/dev");
    QStringList hidRawDevices = devDir.entryList({"hidraw*"}, QDir::System);

    if (hidRawDevices.size() == 0) {
        QMessageBox::critical(nullptr, "No device",
                              "The application cannot find any HID devices installed on this system.");
        exit(0);
    }

    // Check access permission
    QStringList enumeratedDevices;
    for (const QString &dev : hidRawDevices) {
        QString devicePath = devDir.absoluteFilePath(dev);

        int testFd = open(devicePath.toUtf8().constData(), O_RDWR);
        if (testFd >= 0) {
            enumeratedDevices << dev;
            close(testFd);
        }
    }

    // Show error dialog if not found, and limit options if some are inaccessible.
    if (enumeratedDevices.size() == 0) {
        QMessageBox::critical(nullptr, "Permission Denied",
                              "Cannot access devices.\n\n"
                              "Please run this application as root (sudo) or adjust your udev rules.");
        exit(0);
    }
    return enumeratedDevices;
}

int getHapticIntensity(const char *devicePath) {
    int fd = open(devicePath, O_RDONLY);
    if (fd < 0)
        return 0;

    unsigned char buffer[2] = {0};
    buffer[0] = 11;

    int bytesRead = ioctl(fd, HIDIOCGFEATURE(2), buffer);
    close(fd);

    if (bytesRead >= 1) {
        return (int)buffer[1];
    }

    return 0;
}

void setHapticIntensity(const char *devicePath, int value) {
    int fd = open(devicePath, O_RDWR);
    if (fd < 0) {
        // TODO: Warning prompt for open failure
        return;
    }

    unsigned char report[2];
    report[0] = 11;
    report[1] = (unsigned char)value;

    ioctl(fd, HIDIOCSFEATURE(2), report);
    close(fd);
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    QStringList deviceList = getHidRawDevices();

    QWidget window;
    window.setWindowTitle("TrackPad Haptics Control");

    QVBoxLayout *layout = new QVBoxLayout(&window);

    QLabel *comboLabel = new QLabel("Device:", &window);
    QComboBox *deviceSelector = new QComboBox(&window);
    for (const QString &dev : deviceList) {
        QString devicePath = "/dev/" + dev;
        QString friendlyName = getHidrawName(dev);
        deviceSelector->addItem(QString("%1 - %2").arg(devicePath, friendlyName), devicePath);
    }

    QLabel *idLabel = new QLabel("Report ID:", &window);
    QSpinBox *reportIdSelector = new QSpinBox(&window);
    reportIdSelector->setRange(0, 255);
    reportIdSelector->setValue(11);

    QLabel *label = new QLabel("Intensity: 0%", &window);
    label->setAlignment(Qt::AlignCenter);

    QSlider *slider = new QSlider(Qt::Horizontal, &window);
    slider->setRange(0, 100);
    slider->setValue(0);

    QObject::connect(deviceSelector, &QComboBox::currentTextChanged, [slider, label, deviceSelector]() {
        QString currentDevice = deviceSelector->currentData().toString();
        int currentIntensity = getHapticIntensity(currentDevice.toUtf8().constData());

        slider->blockSignals(true);
        slider->setValue(currentIntensity);
        label->setText(QString("Intensity: %1%").arg(currentIntensity));
        slider->blockSignals(false);
    });

    QObject::connect(slider, &QSlider::valueChanged, label, [label, deviceSelector](int value) {
        label->setText(QString("Intensity: %1%").arg(value));
        QString currentDevice = deviceSelector->currentData().toString();
        setHapticIntensity(currentDevice.toUtf8().constData(), value);
    });

    layout->addWidget(comboLabel);
    layout->addWidget(deviceSelector);
    layout->addWidget(idLabel);
    layout->addWidget(reportIdSelector);
    layout->addWidget(label);
    layout->addWidget(slider);

    window.setFixedSize(300, 200);
    window.show();

    return app.exec();
}
