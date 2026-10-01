cmake_minimum_required(VERSION 3.21)
project(maoxiu_stm32 LANGUAGES C ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
include(cmake/sources.cmake)
add_executable(maoxiu_stm32 ${FIRMWARE_SOURCES})
set_target_properties(maoxiu_stm32 PROPERTIES SUFFIX ".elf")
target_include_directories(maoxiu_stm32 PRIVATE ${FIRMWARE_INCLUDES})
target_compile_definitions(maoxiu_stm32 PRIVATE USE_HAL_DRIVER STM32F407xx)
target_compile_options(maoxiu_stm32 PRIVATE -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard
  -ffunction-sections -fdata-sections -fstack-usage -Wall -Wextra -Werror=implicit-function-declaration -Werror=uninitialized -Werror=maybe-uninitialized -Werror=return-type
  $<$<CONFIG:Debug>:-Og> $<$<CONFIG:Debug>:-g3> $<$<CONFIG:Release>:-Os>)
target_link_options(maoxiu_stm32 PRIVATE -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard
  -T${CMAKE_CURRENT_SOURCE_DIR}/firmware/boards/stm32/firmware.ld
  --specs=nano.specs --specs=nosys.specs -Wl,-z,noexecstack -Wl,--gc-sections -Wl,--print-memory-usage
  -Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/maoxiu_stm32.map -u _printf_float)
target_link_libraries(maoxiu_stm32 PRIVATE m)
add_custom_command(TARGET maoxiu_stm32 POST_BUILD
 COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:maoxiu_stm32> ${CMAKE_CURRENT_BINARY_DIR}/maoxiu_stm32.hex
 COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:maoxiu_stm32> ${CMAKE_CURRENT_BINARY_DIR}/maoxiu_stm32.bin
 COMMAND ${CMAKE_SIZE} $<TARGET_FILE:maoxiu_stm32>)
find_program(OPENOCD openocd)
if(OPENOCD)
 add_custom_target(flash COMMAND ${OPENOCD} -f ${CMAKE_CURRENT_SOURCE_DIR}/openocd/board.cfg
  -c "program $<TARGET_FILE:maoxiu_stm32> verify reset exit" DEPENDS maoxiu_stm32 USES_TERMINAL)
endif()
