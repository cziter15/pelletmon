#pragma once
#include <ksIotFrameworkLib.h>
#include <atomic>

namespace comps
{
	namespace FloatValueType
	{
		enum TYPE
		{
			Temperature_Boiler,
			Temperature_PotableWater,
			Temperature_Exhaust,
			MAX
		};
	}

	namespace UIntValueType
	{
		enum TYPE
		{
			Rotations,
			MAX
		};
	}

	class BoilerStatusUpdater : public ksf::ksComponent
	{
		protected:
			
			static const char* floatChannelNames[FloatValueType::MAX];
			std::atomic<float> floatValues[FloatValueType::MAX];

			static const char* uIntChannelNames[UIntValueType::MAX];
			std::atomic<unsigned int> uIntValues[UIntValueType::MAX];

			unsigned int lastPublishTime = 0;

			std::weak_ptr<ksf::ksMqttConnector> mqtt_wp;
			std::weak_ptr<ksf::ksLed> led_wp;

			std::atomic<bool> hasData;

			void sendTelemetryValues(std::shared_ptr<class ksf::ksMqttConnector>& mqtt_sp) const;

		public:
			BoilerStatusUpdater();

			void ICACHE_RAM_ATTR updateFloatValue(FloatValueType::TYPE type, float value);
			void ICACHE_RAM_ATTR updateUIntValue(UIntValueType::TYPE type, unsigned int value);

			bool init(ksf::ksComposable* owner) override;
			bool loop() override;
	};
}
