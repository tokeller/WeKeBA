#include "stdint.h"
#include "sensor_config.h"

/* ------------------------------------------------------------------
 * -- Constants
 * --------------------------------------------------------------- */



/* ------------------------------------------------------------------
 * -- Functions
 * --------------------------------------------------------------- */
 
/*
 * See header file
 */
uint8_t sensor_config_to_str(SensorConfig *sc, char *result)
{
  // nicht printf sondern halt in den string...
  result = fscanf(FILE *output, "{%hhu, %x, %hhu, %hu, %hu, %hu, %hhu}\n", &sensorID, &serial, &fs, &threshold, &baseline, &timeout, &mode);
  if result == 0 alles ok
  if result < 0 nicht ok
}

/*
 * See header file
 */
uint8_t sensor_config_from_str(char *input, SensorConfig *sc)
{
  result = fscanf(FILE *input, "{%hhu, %x, %hhu, %hu, %hu, %hu, %hhu}", &sensorID, &serial, &fs, &threshold, &baseline, &timeout, &mode);
  // prüfen...
  if result == EOF war das file zu früh fertig => failed, Fehlermeldung raus und defaults einlesen
  if result == 7 wurden alle 7 parameter richtig eingelesen
    validiere die Werte, sende sie dann an den sensor und speichere sie im sensors array
  else da war irgendwo ein problem...
}
#endif
