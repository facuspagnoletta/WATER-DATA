#include <dht.h>
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "lcd.h"
#include "bmp280.h"


#define GPIO_3  3
#define GPIO_4  4
#define GPIO_5  5
#define GPIO_6  6
#define GPIO_7  7
#define GPIO_8  8
#define GPIO_9  9
#define GPIO_17 17

int main() {

    // Habilito los pines de la Raspberry
    gpio_init(3);
    gpio_init(4);
    gpio_init(5);
    gpio_init(6);
    gpio_init(7);
    gpio_init(8);
    gpio_init(9);
    gpio_init(17);

    i2c_init(i2c0, 100*1000);
    gpio_set_function(4, GPIO_FUNC_I2C);
    gpio_set_function(5, GPIO_FUNC_I2C);

    gpio_pull_up(4);
    gpio_pull_up(5);

    // Inicializacion de USB
    stdio_init_all();
    
 
    // Habilito USB
    stdio_init_all();
    
    // Instancia de DHT
    dht_t dht;

    // Inicializacion de DHT usando el PIO0 para controlar el DATA_PIN
    dht_init(&dht, DHT_MODEL, pio0, DATA_PIN, true /* pull_up */);
    
    // Inicializacion del LCD
    lcd_init();
    // Inicializo BMP280
    bmp280_init();

    // Obtengo parametros de compensacion
    struct bmp280_calib_param params;
    bmp280_get_calib_params(&params);

    // Variables para temperatura y presion
    int32_t raw_temperature;
    int32_t raw_pressure;
    // Variable para Nivel del agua
    float nivel_del_agua = 2.0;
   
    // Darle tiempo al BMP que se calibre
    sleep_ms(250);

    while(true) {
    
        // Inicio comunicacion con sensor
        dht_start_measurement(&dht);

        // Variables de humedad y temperatura
        float humidity, temperature_c;
     
        // Esperar a que la comunicacion termine y obtener los datos
        dht_result_t result = dht_finish_measurement_blocking(&dht, &humidity, &temperature_c);

        // Leo temperatura y presion
        bmp280_read_raw(&raw_temperature, &raw_pressure);
        // Calculo temperatura y presion
        int32_t temperature = bmp280_convert_temp(raw_temperature, &params);
        int32_t pressure = bmp280_convert_pressure(raw_pressure, raw_temperature, &params);
        
        // Leo Humedad
        dht_reading reading;
        read_from_dht(&reading);
        humidity = reading.humidity;

        
        // Limpio LCD
        lcd_clear();

        char str[16];
        // Armo string
        sprintf(str, "T=%.2f P=%.0f kPa", temperature / 100.f, pressure / 1000.f);
        // Imprimo string en primer fila
        lcd_string(str);
  
       // En qué altura está el agua
       if(gpio_get(GPIO_3) == 0) {
            // Nivel de agua mas alto
            lcd_string("MAX");
        }
        else if(gpio_get(GPIO_4) == 0) {
            lcd_string("N6");
        }

         else if(gpio_get(GPIO_5) == 0) {
            lcd_string("N5");
        }

         else if(gpio_get(GPIO_6) == 0) {
            lcd_string("N4");
        }

         else if(gpio_get(GPIO_7) == 0) {
            lcd_string("N3");
        }

         else if(gpio_get(GPIO_8) == 0) {
            lcd_string("N2");
        }

         else if(gpio_get(GPIO_9) == 0) {
            //Nivel de agua mas bajo
            lcd_string("MIN");

        // Muevo a segunda fila
        lcd_set_cursor(1, 0);
        // Creo segundo string
        sprintf (str,"A=%.0f H=%.2f%%", nivel_del_agua, humidity);
        // Imprimo string en segunda fila
        lcd_string(str);
        // Espero 500 ms
        sleep_ms(500);


        }
    }
    return 0;
}
