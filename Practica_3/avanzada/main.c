#include <stdio.h>
#include "44b.h"
#include "button.h"
#include "leds.h"
#include "utils.h"
#include "D8Led.h"
#include "intcontroller.h"
#include "timer.h"
#include "gpio.h"
#include "keyboard.h"

struct RLstat {
	int moving;
	int speed;
	int direction;
	int position0;
	int position1;
};

static struct RLstat RL = {
	.moving = 1,
	.speed = 5,
	.direction = 0,
	.position0 = 0,
	.position1 = 1,
};

void timer0_ISR(void) __attribute__ ((interrupt ("IRQ")));
void timer1_ISR(void) __attribute__ ((interrupt ("IRQ")));
void timer2_ISR(void) __attribute__ ((interrupt ("IRQ")));
void button_ISR(void) __attribute__ ((interrupt ("IRQ")));
void keyboard_ISR(void) __attribute__ ((interrupt ("IRQ")));

void timer0_ISR(void) {

	if(RL.direction == 1){
		if(RL.position0 < 5)
			RL.position0++;
		else
			RL.position0 = 0;

	}
	else if(RL.direction == 0){
		if(RL.position0 > 0)
			RL.position0--;
		else
			RL.position0 = 5;

	}

	D8Led_2segments(RL.position0, RL.position1);

	ic_cleanflag(INT_TIMER0);

	//COMPLETAR: tomar el código de avance de posición del led rotante de la práctica anterior

}

void timer1_ISR(void) {

	if(RL.direction == 1){

		if(RL.position1 < 5)
			RL.position1++;
		else
			RL.position1 = 0;
	}
	else if(RL.direction == 0){

		if(RL.position1 > 0)
			RL.position1--;
		else
			RL.position1 = 5;
	}

	D8Led_2segments(RL.position0, RL.position1);

	ic_cleanflag(INT_TIMER1);
	//COMPLETAR: tomar el código de avance de posición del led rotante de la práctica anterior

}

void timer2_ISR(void) {

	led1_switch();
	led2_switch();

	ic_cleanflag(INT_TIMER2);

}

void button_ISR(void)
{
	unsigned int whicheint = rEXTINTPND;
	unsigned int buttons = (whicheint >> 2) & 0x3;

	if (buttons == BUT1) {
		led1_switch();

		RL.direction ^= 1;

	}

	if (buttons == BUT2) {
		led2_switch();
		if(RL.moving == 0){
			RL.moving = 1;

			tmr_update(TIMER0);
			tmr_start(TIMER0);

			tmr_update(TIMER1);
			tmr_start(TIMER1);
			}
		else{
			RL.moving = 0;

			tmr_stop(TIMER0);
			tmr_stop(TIMER1);
		}
	}

	//COMPLETAR: usar el código de la primera parte parte de atención a los
	//pulsadores

	// eliminamos rebotes
	Delay(2000);

	if (rEXTINTPND & 0x4){
		rEXTINTPND |= (0x1 << 2);
	}
	else if(rEXTINTPND & 0x8){
		rEXTINTPND |= (0x1 << 3);
	}
	ic_cleanflag(INT_EINT4567);

	// borramos el flag en extintpnd
	//COMPLETAR: debemos borrar las peticiones de interrupción en
		         //EXTINTPND escribiendo un 1 en los flags que queremos borrar (los
				 //correspondientes a los pulsadores pulsados)
	//Borrar el flag de interrupción en el controlador de interrupciones
}

void keyboard_ISR(void)
{
	int key;

	/* Eliminar rebotes de presión */
	Delay(200);
	
	/* Escaneo de tecla */
	key = kb_scan();

	if (key != -1) {
		/* Visualizacion en el display */

		D8Led_digit(key);

		//COMPLETAR: mostrar la tecla en el display utilizando el interfaz
		//definido en D8Led.h

		switch (key) {
			case 0:
				tmr_set_divider(TIMER0, D1_8);
				tmr_set_count(TIMER0, 62500, 62000);
				tmr_update(TIMER0);
				//COMPLETAR: poner en timer0 divisor 1/8 y contador 62500
				break;
			case 1:
				tmr_set_divider(TIMER0, D1_8);
				tmr_set_count(TIMER0, 31250, 31000);
				tmr_update(TIMER0);
				//COMPLETAR: poner en timer0 timer divisor 1/8 y contador 31250
				break;
			case 2:
				tmr_set_divider(TIMER0, D1_8);
				tmr_set_count(TIMER0, 15625, 15500);
				tmr_update(TIMER0);
				//COMPLETAR: poner en timer0 timer divisor 1/8 y contador 15625
				break;
			case 3:
				tmr_set_divider(TIMER0, D1_4);
				tmr_set_count(TIMER0, 15625, 15500);
				tmr_update(TIMER0);
				//COMPLETAR: poner en timer0 timer divisor 1/4 y contador 15625
				break;
			case 4:
				tmr_set_divider(TIMER1, D1_8);
				tmr_set_count(TIMER1, 62500, 62000);
				tmr_update(TIMER1);
				//COMPLETAR: poner en timer0 divisor 1/8 y contador 62500
				break;
			case 5:
				tmr_set_divider(TIMER1, D1_8);
				tmr_set_count(TIMER1, 31250, 31000);
				tmr_update(TIMER1);
				//COMPLETAR: poner en timer0 timer divisor 1/8 y contador 31250
				break;
			case 6:
				tmr_set_divider(TIMER1, D1_8);
				tmr_set_count(TIMER1, 15625, 15500);
				tmr_update(TIMER1);
				//COMPLETAR: poner en timer0 timer divisor 1/8 y contador 15625
				break;
			case 7:
				tmr_set_divider(TIMER1, D1_4);
				tmr_set_count(TIMER1, 15625, 15500);
				tmr_update(TIMER1);
				//COMPLETAR: poner en timer0 timer divisor 1/4 y contador 15625
				break;
			case 8:
				led1_on();
				led2_on();
				tmr_stop(TIMER2);
				tmr_set_mode(TIMER2, ONE_SHOT);
				tmr_set_divider(TIMER2, D1_8);
				tmr_set_count(TIMER2, 62500, 62000);
				tmr_update(TIMER2);
				tmr_start(TIMER2);
				break;
			case 9:
				led1_on();
				led2_on();
				tmr_stop(TIMER2);
				tmr_set_mode(TIMER2, ONE_SHOT);
				tmr_set_divider(TIMER2, D1_8);
				tmr_set_count(TIMER2, 31250, 31000);
				tmr_update(TIMER2);
				tmr_start(TIMER2);
				break;
			case 10:
				led1_on();
				led2_on();
				tmr_stop(TIMER2);
				tmr_set_mode(TIMER2, RELOAD);
				tmr_set_divider(TIMER2, D1_8);
				tmr_set_count(TIMER2, 15625, 15500);
				tmr_update(TIMER2);
				tmr_start(TIMER2);
				break;
			case 11:
				led1_off();
				led2_off();
				tmr_stop(TIMER2);
				break;
			default:
				break;
		}
		
		/* Esperar a que la tecla se suelte, consultando el registro de datos */		
		if (rPDATG & 0x2){};/*COMPLETAR: true si está pulsada la tecla (leer del registro rPDATG)*/
	}

    /* Eliminar rebotes de depresión */
    Delay(200);
     
    /* Borrar interrupciones pendientes */

    ic_cleanflag(INT_EINT1);

    //COMPLETAR
	//borrar la interrupción por la línea EINT1 en el registro rI_ISPC
}

int setup(void)
{
	leds_init();
	D8Led_init();
	D8Led_2segments(RL.position0, RL.position1);

	/* Port G: configuración para generación de interrupciones externas,
	 *         botones y teclado
	 **/

	portG_conf(1, EINT);
	portG_conf(6, EINT);
	portG_conf(7, EINT);

	portG_eint_trig(1, FALLING);
	portG_eint_trig(6, FALLING);
	portG_eint_trig(7, FALLING);

	portG_conf_pup(1, ENABLE);
	portG_conf_pup(6, ENABLE);
	portG_conf_pup(7, ENABLE);

	//COMPLETAR: utilizando el interfaz para el puerto G definido en gpio.h
	//configurar los pines 1, 6 y 7 del puerto G para poder generar interrupciones
	//externas por flanco de bajada por ellos y activar las correspondientes
	//resistencias de pull-up.


	/********************************************************************/

	/* Configuración del timer */
	tmr_set_prescaler(TIMER0, 255);
	tmr_set_divider(TIMER0, D1_8);
	tmr_set_count(TIMER0, 62500, 31250);
	tmr_update(TIMER0);
	tmr_set_mode(TIMER0, RELOAD);
	tmr_stop(TIMER0);

	tmr_set_prescaler(TIMER1, 255);
	tmr_set_divider(TIMER1, D1_8);
	tmr_set_count(TIMER1, 62500, 31250);
	tmr_update(TIMER1);
	tmr_set_mode(TIMER1, RELOAD);
	tmr_stop(TIMER1);
	//COMPLETAR: tomar el código de la segunda parte ????????

	tmr_set_prescaler(TIMER2, 255);
	tmr_set_divider(TIMER2, D1_8);
	tmr_set_count(TIMER2, 62500, 31250);
	tmr_update(TIMER2);
	tmr_set_mode(TIMER2, RELOAD);
	tmr_stop(TIMER2);

	if(RL.moving){
		tmr_start(TIMER0);
		tmr_start(TIMER1);
	}
	/***************************/

	// Registramos las ISRs
	pISR_TIMER0   = (unsigned) timer0_ISR; //COMPLETAR: registrar la RTI del timer
	pISR_TIMER1   = (unsigned) timer1_ISR;
	pISR_TIMER2   = (unsigned) timer2_ISR;

	pISR_EINT4567 = (unsigned) button_ISR;//COMPLETAR: registrar la RTI de los botones
	pISR_EINT1    = (unsigned) keyboard_ISR;//COMPLETAR: registrar la RTI del teclado

	/* Configuración del controlador de interrupciones
	 * Habilitamos la línea IRQ, en modo vectorizado 
	 * Configuramos el timer 0 en modo IRQ y habilitamos esta línea
	 * Configuramos la línea EINT4567 en modo IRQ y la habilitamos
	 * Configuramos la línea EINT1 en modo IRQ y la habilitamos
	 */

	ic_init();
	ic_conf_irq(ENABLE, VEC);
	ic_conf_fiq(DISABLE);

	ic_conf_line(INT_TIMER0, IRQ);
	ic_conf_line(INT_TIMER1, IRQ);
	ic_conf_line(INT_TIMER2, IRQ);

	ic_conf_line(INT_EINT4567, IRQ);
	ic_conf_line(INT_EINT1, IRQ);

	ic_enable(INT_TIMER0);
	ic_enable(INT_TIMER1);
	ic_enable(INT_TIMER2);

	ic_enable(INT_EINT4567);
	ic_enable(INT_EINT1);

	//COMPLETAR: utilizando el interfaz definido en intcontroller.h
	//		habilitar la línea IRQ en modo vectorizado
	//		deshabilitar la línea FIQ
	//		configurar la línea INT_TIMER0 en modo IRQ
	//		configurar la línea INT_EINT4567 en modo IRQ
	//		configurar la línea INT_EINT1 en modo IRQ
	//		habilitar la línea INT_TIMER0
	//		habilitar la línea INT_EINT4567
	//		habilitar la línea INT_EINT1

	/***************************************************/

	Delay(0);
	return 0;
}

int loop(void)
{
	return 0;
}


int main(void)
{
	setup();

	while (1) {
		loop();
	}

	return 0;
}
