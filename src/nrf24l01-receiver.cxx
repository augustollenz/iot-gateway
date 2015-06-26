#include <string.h>
#include <unistd.h>
#include <iostream>
#include "nrf24l01/nrf24l01.h"
#include <signal.h>

static int running = 0;

static upm::NRF24L01 comm(7, 8);
static uint8_t local_address[5]     = {0x01, 0x01, 0x01, 0x01, 0x01};
static uint8_t broadcast_address[5] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

void sig_handler(int signo)
{
    printf("got signal\n");
    if (signo == SIGINT) {
        printf("exiting application\n");
        running = 1;
    }
}

void nrf_handler ()
{
    std::cout << "Receiver: " << comm.m_rxBuffer << std::endl;
}

int main(int argc, char **argv)
{
    comm.setSourceAddress ((uint8_t *) local_address);
    comm.setDestinationAddress ((uint8_t *) broadcast_address);
    comm.setPayload (MAX_BUFFER);
    comm.configure ();
    comm.setSpeedRate (upm::NRF_250KBPS);
    comm.setChannel (99);
    comm.dataRecievedHandler = nrf_handler;

    signal(SIGINT, sig_handler);

    while (!running) {
        comm.pollListener();
    }

    std::cout << "exiting application" << std::endl;

    return 0;
}
