#include "mbed.h"
#include "LSM303D.h"
#include "UoY-serial.h"

LSM303D lsm303d(D14, D15);
Semaphore sem(0, 1);
InterruptIn iin(D2);

void infall() {
    sem.release();
}
int main() {
    iin.rise(infall);
    lsm303d.setAccelRate(LSM303D::AccelRate::rate_100Hz);
    lsm303d.setAccelFilterFreq(LSM303D::AccelFilter::filter_50Hz);
    lsm303d.INT1_enable_DRDY_A();
    serial_settings(USBTX, USBRX, 115200);
    short ax;
    short ay;
    short az;

    lsm303d.getRawAccelInto(ax, ay, az);
    printf("%hd, %hd, %hd\r\n", ax, ay, az);
    while (true) {
        sem.acquire();
        lsm303d.getRawAccelInto(ax, ay, az);
        printf("%hd, %hd, %hd\r\n", ax, ay, az);
    }
}
