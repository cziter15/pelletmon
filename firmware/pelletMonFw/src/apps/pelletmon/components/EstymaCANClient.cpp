#include "EstymaCANCLient.h"
#include "BoilerStatusUpdater.h"
#include "../../../board.h"
#include <CAN.h>

#define ESTYMA_CAN_ROTATIONS 0x2D7
#define ESTYMA_CAN_TEMEPRATURES 0x2D6
#define ESTYMA_CAN_CWUTEMPS 0x3D6
#define ESTYMA_CAN_EXHAUST_TEMPS 0x4D6

namespace comps
{
	EstymaCANClient::EstymaCANClient()
	{
		CAN.setPins(CAN_RX_PIN, CAN_TX_PIN);
	}

	double EstymaCANClient::calculateTemperature(short x) const
	{
		double a = 1.445633283634090E+02;
		double b = -1.386947619796149E-01;
		double c = 9.837913934334235E-05;
		double d = -4.486094388070098E-08;
		double e = 1.076415627857750E-11;
		double f = -1.066640689001453E-15;

		return a + b * x + c * pow(x, 2) + d * pow(x, 3) + e * pow(x, 4) + f * pow(x, 5);
	}

	double EstymaCANClient::calculateExhaustTemperature(short x) const
	{
		double a = 7.371348541128694E+05;
		double b = -1.577562311085309E+03;
		double c = 1.348102462543938E+00;
		double d = -5.750965910950425E-04;
		double e = 1.224810606468546E-07;
		double f = -1.041666667023733E-11;

		return a + b * x + c * pow(x, 2) + d * pow(x, 3) + e * pow(x, 4) + f * pow(x, 5);
	}


	bool EstymaCANClient::init(ksf::ksComposable* owner)
	{
		statusLed_wp = owner->findComponent<ksf::ksLed>();
		boilerStatusUpdater_wp = owner->findComponent<BoilerStatusUpdater>();
		CAN.begin(125E3);
		return true;
	}

	bool EstymaCANClient::loop()
	{
		int packetSize = CAN.parsePacket();

		auto bsu_sp = boilerStatusUpdater_wp.lock();
		
		if (packetSize > 0 && bsu_sp)
		{
			switch (CAN.packetId())
			{
				case ESTYMA_CAN_ROTATIONS:
				{
					unsigned short val;
					CAN.readBytes((uint8_t*)&val, 2);
					bsu_sp->rotations = val;
				}
				break;

				case ESTYMA_CAN_TEMEPRATURES:
				{
					uint16_t temps[4];
					CAN.readBytes((uint8_t*)&temps, sizeof(temps));

					if (auto statusLed_sp = statusLed_wp.lock())
						statusLed_sp->setBlinking(50, 1);

					bsu_sp->updateTemperature(TemperatureType::Boiler, calculateTemperature(temps[0]));
				}
				break;

				case ESTYMA_CAN_CWUTEMPS:
				{
					uint16_t temps[4];
					CAN.readBytes((uint8_t*)&temps, sizeof(temps));
					bsu_sp->updateTemperature(TemperatureType::Cwu, calculateTemperature(temps[0]));
				}
				break;

				case ESTYMA_CAN_EXHAUST_TEMPS:
				{
					uint16_t temps[4];
					CAN.readBytes((uint8_t*)&temps, sizeof(temps));
					bsu_sp->updateTemperature(TemperatureType::Exhaust, calculateExhaustTemperature(temps[3]));
				}
				break;
			}
		}

		return true;
	}

	EstymaCANClient::~EstymaCANClient()
	{
		CAN.end();
	}
}