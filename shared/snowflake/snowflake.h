#pragma once
#include <cstdint>
#include <cassert>
#include <mutex>

namespace sunlight
{
    class IUniqueIDGenerator
    {
    public:
        virtual ~IUniqueIDGenerator() = default;

        virtual auto Generate() -> uint64_t = 0;
    };

    namespace snowflake
	{
		static inline constexpr uint64_t SEQUENCE_BIT = 12;
		static inline constexpr uint64_t MACHINE_BIT = 5;
		static inline constexpr uint64_t DATACENTER_BIT = 5;

		static inline constexpr uint64_t MAX_SEQUENCE_MASK = -1 ^ (-1 << SEQUENCE_BIT);
		static inline constexpr uint64_t MAX_MACHINE_ID = -1 ^ (-1 << MACHINE_BIT);
		static inline constexpr uint64_t MAX_DATACENTER_ID = -1 ^ (-1 << DATACENTER_BIT);

		struct ThreadUnsafe
		{
			void lock() noexcept {}
			void unlock() noexcept {}
		};

		class Clock
		{
		public:
			static auto Now() -> uint64_t;
		};
	}

	template <typename Mutex, typename Clock, uint64_t EPOCH>
	class basic_snowflake : public IUniqueIDGenerator
	{
	public:
		using mutex_t = Mutex;
		using clock_t = Clock;
		static constexpr uint64_t epoch = EPOCH;

	public:
		// ID_BIT = (MACHINE_BIT + DATACENTER_BIT) = 10
		basic_snowflake(uint64_t uniqueID);

		// MACHINE_BIT = 5, DATACENTER_BIT = 5
		basic_snowflake(uint64_t machineID, uint64_t dataCenterID);

		[[nodiscard]]
        auto Generate() -> uint64_t override;

	private:
		const uint64_t _machineId = 0;
		const uint64_t _dataCenterId = 0;
		uint64_t _sequence = 0;
		uint64_t _lastTimePoint = 0;
		mutex_t _mutex;
	};

	template <typename Mutex, typename Clock, uint64_t EPOCH>
	basic_snowflake<Mutex, Clock, EPOCH>::basic_snowflake(uint64_t uniqueID)
		: basic_snowflake((uniqueID & 0b11111), ((uniqueID >> snowflake::MACHINE_BIT) & 0b11111))
	{
	}

	template <typename Mutex, typename Clock, uint64_t EPOCH>
	basic_snowflake<Mutex, Clock, EPOCH>::basic_snowflake(uint64_t machineID, uint64_t dataCenterID)
		: _machineId(machineID)
		, _dataCenterId(dataCenterID)
	{
		assert(_machineId <= snowflake::MAX_MACHINE_ID);
		assert(_dataCenterId <= snowflake::MAX_DATACENTER_ID);
	}

	template <typename Mutex, typename Clock, uint64_t EPOCH>
	auto basic_snowflake<Mutex, Clock, EPOCH>::Generate() -> uint64_t
	{
		uint64_t timePoint = 0;
		uint64_t sequence = 0;
		{
			std::lock_guard lock(_mutex);

			timePoint = clock_t::Now();
			assert(timePoint >= _lastTimePoint);

			if (timePoint == _lastTimePoint)
			{
				_sequence = (_sequence + 1) & snowflake::MAX_SEQUENCE_MASK;
				if (_sequence == 0)
				{
					do
					{
						timePoint = clock_t::Now();
					} while (timePoint <= _lastTimePoint);
				}
			}
			else
			{
				_sequence = 0;
			}

			sequence = _sequence;
			_lastTimePoint = timePoint;
		}

		return
			((timePoint - epoch) << (snowflake::SEQUENCE_BIT + snowflake::MACHINE_BIT + snowflake::DATACENTER_BIT))
			| (_dataCenterId << (snowflake::SEQUENCE_BIT + snowflake::MACHINE_BIT))
			| (_machineId << (snowflake::SEQUENCE_BIT))
			| (sequence);
	}

	template <uint64_t EPOCH = 1704034800>
	using UniqueSnowflake = basic_snowflake<snowflake::ThreadUnsafe, snowflake::Clock, EPOCH>;

	template <uint64_t EPOCH = 1704034800>
	using SharedSnowflake = basic_snowflake<std::mutex, snowflake::Clock, EPOCH>;
}
