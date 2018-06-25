#include "L5_Testing/testing_frameworks.hpp"
#include "max7456.hpp"

FAKE_VOID_FUNC(vChipSelect, bool);
FAKE_VALUE_FUNC(uint8_t, vSPITransfer, uint8_t);

namespace max_testing
{
	MAX7456 Max(vChipSelect, vSPITransfer);
}

TEST_CASE("Testing MAX Initialization", "[max]")
{
	SECTION("init 0")
	{
		vSPITransfer_fake.custom_fake = [](uint8_t Data)
		{
		    printf("[STUB] Transfer = %c\n", Data);
		    return Data;
		};

		max_testing::Max.Initialize();

		printf("ChipSelect_fake.call_count = %d\n", vChipSelect_fake.call_count);
		printf("SPITransfer_fake.call_count = %d\n", vSPITransfer_fake.call_count);

		CHECK(vChipSelect_fake.call_count <= 14);
	}
	SECTION("init 1")
	{
		vSPITransfer_fake.custom_fake = [](uint8_t Data)
		{
		    printf("[NEW STUB] Transfer = %c\n", Data);
		    return Data;
		};

		max_testing::Max.Initialize();

		printf("ChipSelect_fake.call_count = %d\n", vChipSelect_fake.call_count);
		printf("SPITransfer_fake.call_count = %d\n", vSPITransfer_fake.call_count);

		REQUIRE(vChipSelect_fake.call_count <= 28);
	}
}