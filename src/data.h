#include <time.h>

#include "nrf24l01/nrf24l01.h"

class DataSource {
public:
	virtual ~DataSource() {}
	virtual uint8_t getTemp() const = 0;
	virtual uint8_t getHumid() const = 0;
	virtual void polling() = 0;
	virtual void setCallback(void (*callback)())
	{
		m_callback = callback;
	}
protected:
	void (*m_callback)();
};

class RadioDataSource : public DataSource {
public:
	RadioDataSource() : comm(7, 8)
	{
		const uint8_t local_address[5] = {0x01, 0x01, 0x01, 0x01, 0x01};
		const uint8_t broadcast_address[5] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
	    comm.setSourceAddress((uint8_t *) local_address);
	    comm.setDestinationAddress((uint8_t *) broadcast_address);
	    comm.setPayload(MAX_BUFFER);
	    comm.configure();
	    comm.setChannel(99);
	}
	~RadioDataSource() {}

	uint8_t getTemp() const
	{
		return comm.m_rxBuffer[4];
	}

	uint8_t getHumid() const
	{
		return comm.m_rxBuffer[3];
	}

	void polling()
	{
		comm.pollListener();
	}

	virtual void setCallback(void (*callback)())
	{
		m_callback = callback;
		comm.dataRecievedHandler = m_callback;
	}

private:
	upm::NRF24L01 comm;
};

class RandomDataSource : public DataSource {
public:
	RandomDataSource()
	{
		srand (time(NULL));
	}
	~RandomDataSource() {}

	uint8_t getTemp() const
	{
		return (rand() % 100 + 1);
	}

	uint8_t getHumid() const
	{
		return (rand() % 100 + 1);
	}

	void polling()
	{
		sleep(1);
		m_callback();
	}
};
