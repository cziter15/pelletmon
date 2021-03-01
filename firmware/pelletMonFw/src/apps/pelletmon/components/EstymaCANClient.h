#pragma once
#include <ksIotFrameworkLib.h>

namespace comps
{
	class BoilerStatusUpdater;
	class EstymaCANClient : public ksf::ksComponent
	{
		protected:
			std::weak_ptr<BoilerStatusUpdater> boilerStatusUpdater_wp;

			double calculateTemperature(uint16_t x) const;
			double calculateExhaustTemperature(uint16_t x) const;
			
			bool isBound = false;

		public:
			EstymaCANClient();

			void bindCAN();
			void unbindCAN();

			bool init(class ksf::ksComposable* owner) override;
			bool loop() override;

			virtual ~EstymaCANClient();
	};
}
