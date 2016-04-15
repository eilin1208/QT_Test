#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/input.h>
#include <linux/uinput.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <QtCore>
#include <QFile>
#include <QVector>
#include <QDebug>
struct KeyEvent
{
    quint16 key;
    int time;
};

QVector<KeyEvent> commandList;

int loadSettingFile()
{
    QFile file(QCoreApplication::applicationDirPath() + "/run.txt");
    qDebug() << file.fileName();
    QStringList line;
    QByteArray data;
    KeyEvent command;
    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        data = file.readLine();
        qDebug() << data.data();
        while(!data.isEmpty())
        {
            line = QString(data).split(" ");
            command.key = line.at(0).toUShort();
            command.time = line.at(1).toInt();
            commandList.append(command);
            data = file.readLine();
            qDebug() << data.data();

        }
        file.close();
        return 0;
    }
    else
    {
        return -1;
    }
}

/* Globals */
static int uinp_fd = -1;
struct uinput_user_dev uinp;  // uInput device structure
struct input_event event;  // Input device structure

/* Setup the uinput device */

int setup_uinput_device()
{
    // Temporary variable
    int i=0;

    // Open the input device
    uinp_fd = open("/dev/uinput", O_WRONLY | O_NDELAY);

    if (uinp_fd == NULL)

    {
        printf("Unable to open /dev/uinput\n");
        return -1;
    }

    memset(&uinp,0,sizeof(uinp));   // Intialize the uInput device to NULL

    strncpy(uinp.name, "PolyVision Touch Screen", UINPUT_MAX_NAME_SIZE);
    uinp.id.version = 4;
    uinp.id.bustype = BUS_USB;

    // Setup the uinput device
    ioctl(uinp_fd, UI_SET_EVBIT, EV_KEY);
    ioctl(uinp_fd, UI_SET_EVBIT, EV_REL);
    ioctl(uinp_fd, UI_SET_RELBIT, REL_X);
    ioctl(uinp_fd, UI_SET_RELBIT, REL_Y);

    for (i=0; i < 256; i++) {
        ioctl(uinp_fd, UI_SET_KEYBIT, i);
    }

    ioctl(uinp_fd, UI_SET_KEYBIT, BTN_MOUSE);
    ioctl(uinp_fd, UI_SET_KEYBIT, BTN_TOUCH);

    ioctl(uinp_fd, UI_SET_KEYBIT, BTN_MOUSE);
    ioctl(uinp_fd, UI_SET_KEYBIT, BTN_LEFT);
    ioctl(uinp_fd, UI_SET_KEYBIT, BTN_MIDDLE);
    ioctl(uinp_fd, UI_SET_KEYBIT, BTN_RIGHT);
    ioctl(uinp_fd, UI_SET_KEYBIT, BTN_FORWARD);
    ioctl(uinp_fd, UI_SET_KEYBIT, BTN_BACK);

    /* Create input device into input sub-system */
    write(uinp_fd, &uinp, sizeof(uinp));

    if (ioctl(uinp_fd, UI_DEV_CREATE))
    {
        printf("Unable to create UINPUT device.");
        return -1;
    }
    return 1;
}

void send_click_events( )
{
    // Move pointer to (0,0) location
    memset(&event, 0, sizeof(event));
    gettimeofday(&event.time, NULL);

    event.type = EV_REL;
    event.code = REL_X; event.value = 100;
    write(uinp_fd, &event, sizeof(event));

    event.type = EV_REL;
    event.code = REL_Y;
    event.value = 100;
    write(uinp_fd, &event, sizeof(event));

    event.type = EV_SYN;
    event.code = SYN_REPORT;
    event.value = 0;
    write(uinp_fd, &event, sizeof(event));

    // Report BUTTON CLICK - PRESS event

    memset(&event, 0, sizeof(event));
    gettimeofday(&event.time, NULL);
    event.type = EV_KEY;
    event.code = BTN_LEFT;
    event.value = 1;
    write(uinp_fd, &event, sizeof(event));

    event.type = EV_SYN;
    event.code = SYN_REPORT;
    event.value = 0;
    write(uinp_fd, &event, sizeof(event));

    // Report BUTTON CLICK - RELEASE event

    memset(&event, 0, sizeof(event));
    gettimeofday(&event.time, NULL);
    event.type = EV_KEY;
    event.code = BTN_LEFT;
    event.value = 0;
    write(uinp_fd, &event, sizeof(event));

    event.type = EV_SYN;
    event.code = SYN_REPORT;
    event.value = 0;
    write(uinp_fd, &event, sizeof(event));
}

void send_button(quint16 key)
{
    // Report BUTTON CLICK - PRESS event

    memset(&event, 0, sizeof(event));
    gettimeofday(&event.time, NULL);
    event.type = EV_KEY;
    event.code = key;
    event.value = 1;
    write(uinp_fd, &event, sizeof(event));

    event.type = EV_SYN;
    event.code = SYN_REPORT;
    event.value = 0;
    write(uinp_fd, &event, sizeof(event));


    // Report BUTTON CLICK - RELEASE event

    memset(&event, 0, sizeof(event));
    gettimeofday(&event.time, NULL);
    event.type = EV_KEY;
    event.code = key;
    event.value = 0;
    write(uinp_fd, &event, sizeof(event));
    event.type = EV_SYN;
    event.code = SYN_REPORT;
    event.value = 0;
    write(uinp_fd, &event, sizeof(event));
}


/* This function will open the uInput device. Please make
   sure that you have inserted the uinput.ko into kernel.  */

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    // Return an error if device not found.
    if (setup_uinput_device() < 0)
    {
        printf("Unable to find uinput device\n");
        return -1;
    }

    loadSettingFile();

    int index = 0;
    int mode = 0;
    if(argc >= 2)
    {
        mode = atoi(argv[1]);
    }
    while(1 && !commandList.isEmpty())
    {
        send_button(commandList.at(index).key);    // Send a "A" key
        usleep(commandList.at(index).time * 1000);
        index++;
        if(index == commandList.size())
        {
            index = 0;
        }
        if(mode == 0)
        {
            break;
        }
    }


    /* Destroy the input device */
    ioctl(uinp_fd, UI_DEV_DESTROY);

    /* Close the UINPUT device */
    close(uinp_fd);
    a.exec();
}
