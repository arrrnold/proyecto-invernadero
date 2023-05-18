#include <msp430.h>

int contador;
int humedad;

void main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer

	contador = 0;

	P1DIR |= BIT4 + BIT5; // P1.4 y 1.5 como salidas
	P1OUT &=~ BIT4; // bomba desactivada
	P1OUT |= BIT5; // luz activada
	P6DIR &=~ BIT0; // P6.0 como entrada (para el adc)

	P1DIR |= BIT0; // led rojo como saldia
	P1OUT &=~ BIT0; // apagar led rojo

	P4DIR |= BIT7; // led verde como saldia
	P4OUT |= BIT7; // encender led verde

	TA0CCTL0 = CCIE;
	TA0CTL = TASSEL__ACLK + MC__UPDOWN;
	TA0CCR0 = 0xFFFF;
	
	/* para el ADC */
	ADC12CTL0 |= ADC12ON;	// encendemos el ADC
	ADC12CTL1 |= ADC12SHP;	// conectamos el reloj
	ADC12CTL0 |= ADC12ENC;	// activar el ADC para conversión
	P6SEL |= BIT0; // habilita el pin donde va a entrar el voltaje

	/*sensar humedad y controlar iluminacion*/
	while(1)
	{
		ADC12CTL0 |= ADC12SC; // inicial el muestreo

		if (ADC12MEM0 > 500)
		{
			humedad = ADC12MEM0;
			if (humedad < 1750) // 1750 = 70% de humedad
			{
				P1OUT |= BIT0;	// encender led testigo
				P1OUT |= BIT4; // encender la bomba
			}
			else
			{
				P1OUT &=~ BIT0;	// apagar led testigo
				P1OUT &=~ BIT4; // apagar la bomba
			}
			_BIS_SR(GIE); // habilitar interrupciones
		}
	}
}

#pragma vector=TIMER0_A0_VECTOR
__interrupt void ILUMINAR (void)
{
	contador++;

	/* alternar la luz */
	if(contador > 10800) // 10800*4s = 12h
	{
		contador = 0;
		P4OUT ^= BIT7; // encender led testigo
		P1OUT ^= BIT5; // encender luz
		__bis_SR_register_on_exit(GIE);
	}
}
