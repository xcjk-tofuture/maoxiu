cmake_minimum_required(VERSION 3.21)
project(maoxiu_tm4c LANGUAGES C ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
include(cmake/sources.cmake)
add_executable(maoxiu_tm4c ${FIRMWARE_SOURCES})
set_target_properties(maoxiu_tm4c PROPERTIES SUFFIX ".elf")
set_property(TARGET maoxiu_tm4c APPEND PROPERTY LINK_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/boards/tm4c/firmware.ld)
target_include_directories(maoxiu_tm4c PRIVATE ${FIRMWARE_INCLUDES})
target_compile_definitions(maoxiu_tm4c PRIVATE PART_TM4C1233H6PM TARGET_IS_BLIZZARD_RB1 gcc)
target_compile_options(maoxiu_tm4c PRIVATE -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard
  -ffunction-sections -fdata-sections -fstack-usage -Wall -Wextra -Werror=implicit-function-declaration -Werror=uninitialized -Werror=maybe-uninitialized -Werror=return-type
  $<$<CONFIG:Debug>:-Og> $<$<CONFIG:Debug>:-g3> $<$<CONFIG:Release>:-Os>)
target_link_options(maoxiu_tm4c PRIVATE -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard
  -T${CMAKE_CURRENT_SOURCE_DIR}/boards/tm4c/firmware.ld
  --specs=nano.specs --specs=nosys.specs -Wl,-z,noexecstack -Wl,--gc-sections -Wl,--print-memory-usage
  -Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/maoxiu_tm4c.map -u _printf_float)
target_link_libraries(maoxiu_tm4c PRIVATE m)
add_custom_command(TARGET maoxiu_tm4c POST_BUILD
 COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:maoxiu_tm4c> ${CMAKE_CURRENT_BINARY_DIR}/maoxiu_tm4c.hex
 COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:maoxiu_tm4c> ${CMAKE_CURRENT_BINARY_DIR}/maoxiu_tm4c.bin
 COMMAND ${CMAKE_SIZE} $<TARGET_FILE:maoxiu_tm4c>)
find_program(OPENOCD openocd)
if(OPENOCD)
 add_custom_target(flash COMMAND ${OPENOCD} -f ${CMAKE_CURRENT_SOURCE_DIR}/openocd/board.cfg
  -c "program $<TARGET_FILE:maoxiu_tm4c> verify reset exit" DEPENDS maoxiu_tm4c USES_TERMINAL)
endif()
