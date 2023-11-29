#include "capteur.h"

 //setup pressure sensor

sixtron::O2SMPB02E capteur(P1_I2C_SDA, P1_I2C_SCL);

int capteurInit(void)
{
  if(capteur.init()) printf("init success\r\n");
    else printf("init failed\r\n");

    capteur.measure_single_shot();
    float pressure = 0;
    printf("Pressure: '%f'\r\n", pressure);
    pressure = capteur.pressure();
    printf("\r\n MAIN : Sensor Value = %.2f\" \r\n", pressure);

  return 0;
}

int capteurSend(void)
{
    capteur.measure_single_shot();
    float pressure = 0;
    printf("Pressure: '%f'\r\n", pressure);
    pressure = capteur.pressure();
    printf("\r\n MAIN : Sensor Value = %.2f\" \r\n", pressure);
    return 0;
}