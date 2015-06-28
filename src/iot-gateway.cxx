#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <signal.h>
#include <stdio.h>

extern "C" {
#include "iot_agent.h"
}

#include "data.h"

static DataSource *dataSource;
static int running = 0;

void sig_handler(int signo)
{
    printf("got signal\n");
    if (signo == SIGINT) {
        printf("exiting application\n");
        running = 1;
    }
}

void nrf_handler()
{
	char humid[16];
	snprintf(humid, 16, "%d", dataSource->getHumid());
	std::cout << "humid: " << humid << std::endl;
	iot_agent_send((char *) "humid", humid);

	char temp[16];
	snprintf(temp, 16, "%d", dataSource->getTemp());
	std::cout << "temp: " << temp << std::endl;
	iot_agent_send((char *) "temp", temp);
}

int main(int argc, char **argv)
{
	signal(SIGINT, sig_handler);

	if (argc > 1) {
		dataSource = new RandomDataSource();
		std::cout << "Creating RandomDataSource" << std::endl;
	} else {
		dataSource = new RadioDataSource();
		std::cout << "Creating RadioDataSource" << std::endl;
	}

	dataSource->setCallback(nrf_handler);

    while (!running) {
    	dataSource->polling();
    }

    std::cout << "exiting application" << std::endl;

    return 0;
}

