/**
 * @file gpio.c
 * @author Daniel M. David L. Oliver W. 
 * @brief Funktionen für das Benutzen und konfiguirenen der GPIO Pins als Ein- und Ausgang
 * @version 0.1
 * @date 2020-06-08
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#include "gpio.h"
#include <stdlib.h>

/**
 * @brief interrupt tabelle
 * hier werden für jeden möglichen gpio Pin (port0/ port2) (rise/ fall)
 * die Interupthandler-funktionen als function_pointer gespeichert
 */
interrupt_ptr_t interrupt_lookup[128];

/**
 * @brief argumente tabelle
 * hier werden für jeden möglichen gpio Pin (port0/ port2) (rise/ fall)
 * ein Argument gespeichert das dem Interupthandler übergeben wird
 * dieses Argument ist im normalfall eine Adresse auf eine Struktur die dem Interrupt zugeordnet ist
 */
void * args[128];


/**
 * @brief die bitpositon des bits in einer bitmaske berechnen
 * 
 * @param mask bitmaske 
 * @return uint32_t berechnete bit position
 */
uint32_t mask_to_bit(uint32_t mask) {
    for (uint32_t i = 0; i < 32; i++)
    {
        if(mask>>i & 0x01) {
            return i;
        }
    }
    return 0;
}

/**
 * @brief erzeugt einen @link(GPIO_pin) Struct indem alle pointer zu den config-registern zwischengespeichert werden sowie bitmaske für den pin
 * 
 * @param port port an dem derr gewüschte pin liegt
 * @param pin  pin position inerhalb des ports
 * @return gpio_pin struct
 */
GPIO_pin gpio_init(uint8_t port, uint8_t pin) {

    GPIO_pin pin_s;
    pin_s.iocon = ((IOCON_D_GPIO_T*)(&(LPC_IOCON->p[port][pin])));     //iocon register adresse as bitfield pointer
    pin_s.port = &(LPC_GPIO->p[port]);                                 //address von dem beginn der port register holen
    pin_s.bitmask = (1 << pin);
    pin_s.iocon->func = 0x00;

    if (port == 0)
    {
        pin_s.intr_port = &(LPC_GPIO_INTR->port0);
    } 
    else if (port == 2)
    {
        pin_s.intr_port = &(LPC_GPIO_INTR->port2);
    } else 
    {
        pin_s.intr_port = 0;
    }

    return pin_s;
}

/**
 * @brief den mode für den GPIO_pin konfigurieren
 * 
 * @param pin GPIO_pin(struct) des zu konfigurierenden pins
 * @param mode Mode in die der pin gesetz werden soll
 */
void gpio_set_mode(GPIO_pin * pin, GPIO_mode_t mode) {

    switch (mode)
    {
    case INPUT: //Pin als INPUT konfigurieren
        pin->iocon->mode = 0x00;
        pin->port->dir = (pin->port->dir & (~(pin->bitmask)));

        break;
    case INPUT_PULLDOWN:    //Pin als INPUT mit Pulldown konfigurieren
        pin->iocon->mode = 0x01;
        pin->port->dir = (pin->port->dir & (~(pin->bitmask)));

        break;
    case INPUT_PULLUP:      //Pin als INPUT mit Pullup konfigurieren
        pin->iocon->mode = 0x02;
        pin->port->dir = (pin->port->dir & (~(pin->bitmask)));

        break;
    case INPUT_PULLUP_HYS:      //Pin als INPUT mit Pullup konfigurieren und HSY
        pin->iocon->mode = 0x02;
        pin->iocon->hys = 1;
        pin->port->dir = (pin->port->dir & (~(pin->bitmask)));

        break;
    case OUTPUT:            //Pin als OUTPUT konfigurieren
        pin->iocon->mode = 0x00; // keine pullup/down aktivieren     
        pin->port->dir = (pin->port->dir | (pin->bitmask));
    break;

    default:                //für debugging, wird normalerweise nie betreten
        __nop();            
        break;
}


}

/**
 * @brief Den Wert von einem als INPUT konfiguierten GPIO_pin einlesen
 * 
 * @param p GPIO_pin(struct) des pins der gelesen werden soll
 * @return uint8_t gelesener wert 0|1
 */
uint8_t gpio_read(GPIO_pin * p) {
    return (p->port->pin &(p->bitmask)) > 0; 
}

/**
 * @brief einen Wert an einem als OUTPUT konfiguierten GPIO_pin ausgeben
 * 
 * @param p GPIO_pin(struct) des pins an den ausgebeben werden soll
 * @param value wert der geschieben werden soll 0|1
 */
void gpio_write(GPIO_pin * p, uint8_t value) {

    if (value)
    {
        p->port->set = p->bitmask;
    } 
    else
    {
        p->port->clr = p->bitmask;
    }

}


/**
 * @brief GPIO interrupts anschalten
 */
void gpio_init_interrupts() {
    	
    //Interrupt Set-Enable Register 0 
    uint32_t *iser1_adress = ISER1_REGISTER;
    (*iser1_adress) = (*iser1_adress) | ISE_GPIO_MASK;

    // lookup tabelle zurückseten
	for (uint32_t i = 0; i < 128; i++)
    {
         interrupt_lookup[i] = NULL;
         args[i] = NULL;
	}
}

/**
 * @brief einen interrupt an einen gpio pin anhängen
 * 
 * @param pin GPIO_pin an den ein interrupt angehängt werden soll RISING|FALLING|BOTH
 * @param mode GPIO_intr_mode_t welche interrupt mode verwendet werden soll 
 * @param func_ptr function_pointer zu der handler_funktion die den interrupt verarbeiten soll
 * @param arg argument das der handler_funktion zusäzlich übergeben werden soll um den interrupt besser zuordnen zu können 
 */
void gpio_interrupt(GPIO_pin * pin, GPIO_intr_mode_t mode, interrupt_ptr_t func_ptr, void* arg) {

    //bitmaske zu bit nummer konvertiern
    uint32_t pin_bit = mask_to_bit(pin->bitmask);
    
    //zwischen port0 und port2 unterscheiden
    //und an die richtige stelle in der interrupt_lookup (und argumenten) tabelle verweisen 
    interrupt_ptr_t * interrupt_lookup_ptr;
    void ** args_p;
    if (pin->intr_port == &(LPC_GPIO_INTR->port0))
    {
        interrupt_lookup_ptr =interrupt_lookup + 0;
        args_p = args + 0;
    } else if (pin->intr_port == &(LPC_GPIO_INTR->port2))
    {
        interrupt_lookup_ptr =interrupt_lookup + 64;
        args_p = args + 64;
    }

    // je nach mode werden register aktivert und lookup einträge gesetzt
    switch (mode)
    {

    case FALLING:
        pin->intr_port->enable_f |= pin->bitmask;
        interrupt_lookup_ptr[pin_bit] = func_ptr;
        args_p[pin_bit] = arg;
        break;

    case RISING:
        pin->intr_port->enable_r |= pin->bitmask;
        interrupt_lookup_ptr[pin_bit+32] = func_ptr;
        args_p[pin_bit+32] = arg;
        break;

    case BOTH:
        pin->intr_port->enable_r |= pin->bitmask;
        pin->intr_port->enable_f |= pin->bitmask;
        interrupt_lookup_ptr[pin_bit] = func_ptr;
        interrupt_lookup_ptr[pin_bit+32]  = func_ptr;
        args_p[pin_bit] = arg;
        args_p[pin_bit+32] = arg;
        break;

    case NONE:
        pin->intr_port->enable_r &= (~pin->bitmask);
        pin->intr_port->enable_f &= (~pin->bitmask);
        interrupt_lookup_ptr[pin_bit] = NULL;
        interrupt_lookup_ptr[pin_bit+32]  = NULL;
        break;

    default:                //für debugging, wird normalerweise nie betreten
        __nop();            
        break;
    }
}


/**
 * @brief einen port auf interrupts überprüfen
 * 
 * alle pins eines ports auf interrupts überprüfen
 * falls ein interrupt vorliegt in der and der interrupt_lookup_ptr stelle nach dem interrtupt handler suchen 
 * und diesen mit dem argument aus der argument_tabelle aufrufen
 * 
 * @param port LPC_GPIO_PORT_INTR zeiger zu dem interupt port der geprüft werden soll
 * @param interrupt_lookup_ptr interupt_handler_lookup_tabellen bereich der verwendet werden soll
 * @param args_p argument_tabellen bereich der verwendet werden soll
 */
void GPIO_IRQ_PORTHandler( volatile LPC_GPIO_PORT_INTR * port , interrupt_ptr_t * interrupt_lookup_ptr, void ** args_p) {

    //alle pins des ports auf interrupts prüfen
    for (size_t i = 0; i < 32; i++)
    {
        
        //falling intterupt prüfen
        if((port->stat_f & (1<<i)) > 0){
            //wenn eine interupt_handler funktion hinterlegt ist diese mit dem dafür hinterlegen argument ausführen
            if(interrupt_lookup_ptr[i] != NULL) { interrupt_lookup_ptr[i](args_p[i]); }
            port->clr |= 1<<i;  //zurücksetzen
        }

        //rising intterupt prüfen
        if((port->stat_r & (1<<i)) > 0){
            //wenn eine interupt_handler funktion hinterlegt ist diese mit dem dafür hinterlegen argument ausführen
            if(interrupt_lookup_ptr[i+32] != NULL) { interrupt_lookup_ptr[i+32](args_p[i+32]); }
            port->clr |= 1<<i; //zurücksetzen
        }
        
    }


}


/**
 * @brief Gobaler GPIO interrupt handler
 * 
 * wertet alle interrupts auf GPIO pins aus
 * 
 * Ruft die GPIO_IRQ_PORTHandler für die port0 und port2 auf wenn in dem port ein interrupt vorliegt 
 */
void GPIO_IRQHandler() {
		
	//clear IRQ Pending
    uint32_t *icpr0_adress = ICPR0_REGISTER;
    (*icpr0_adress) |= (ISE_GPIO_MASK);
	
    interrupt_ptr_t * interrupt_lookup_ptr; //position der interrupt_lookup die verwendet werden soll
    void ** args_p;                         //position der argument_lookup die verwendet werden soll
    volatile LPC_GPIO_PORT_INTR * port;     //port0 oder port2
    volatile uint32_t status_int = LPC_GPIO_INTR->status & 0x05; //status der ausgewertet werden soll ob in port0 oder port2 ein interrupt augetreten ist

    // interrupt in port0 ist aufgetreten
    if ((status_int & 0x01 ) > 0) {
        //zeiger für PORT 0 konfigureiren
        interrupt_lookup_ptr =interrupt_lookup + 0;
        args_p = args + 0;
        port = &(LPC_GPIO_INTR->port0);

         GPIO_IRQ_PORTHandler(port, interrupt_lookup_ptr, args_p);
    }

    // interrupt in port2 ist aufgetreten
    if ((status_int & 0x04) > 0)
    {
        //zeiger für PORT 2 konfigureiren
        interrupt_lookup_ptr =interrupt_lookup + 64;
        args_p = args + 64;
        port = &(LPC_GPIO_INTR->port2);
        GPIO_IRQ_PORTHandler(port, interrupt_lookup_ptr,args_p);
    }

}
