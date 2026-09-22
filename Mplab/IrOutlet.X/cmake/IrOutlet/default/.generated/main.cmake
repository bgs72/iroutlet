include("${CMAKE_CURRENT_LIST_DIR}/rule.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/file.cmake")

set(IrOutlet_default_library_list )

# Handle files with suffix (s|as|asm|AS|ASM|As|aS|Asm), for group default-XC8
if(IrOutlet_default_default_XC8_FILE_TYPE_assemble)
add_library(IrOutlet_default_default_XC8_assemble OBJECT ${IrOutlet_default_default_XC8_FILE_TYPE_assemble})
    IrOutlet_default_default_XC8_assemble_rule(IrOutlet_default_default_XC8_assemble)
    list(APPEND IrOutlet_default_library_list "$<TARGET_OBJECTS:IrOutlet_default_default_XC8_assemble>")

endif()

# Handle files with suffix S, for group default-XC8
if(IrOutlet_default_default_XC8_FILE_TYPE_assemblePreprocess)
add_library(IrOutlet_default_default_XC8_assemblePreprocess OBJECT ${IrOutlet_default_default_XC8_FILE_TYPE_assemblePreprocess})
    IrOutlet_default_default_XC8_assemblePreprocess_rule(IrOutlet_default_default_XC8_assemblePreprocess)
    list(APPEND IrOutlet_default_library_list "$<TARGET_OBJECTS:IrOutlet_default_default_XC8_assemblePreprocess>")

endif()

# Handle files with suffix [cC], for group default-XC8
if(IrOutlet_default_default_XC8_FILE_TYPE_compile)
add_library(IrOutlet_default_default_XC8_compile OBJECT ${IrOutlet_default_default_XC8_FILE_TYPE_compile})
    IrOutlet_default_default_XC8_compile_rule(IrOutlet_default_default_XC8_compile)
    list(APPEND IrOutlet_default_library_list "$<TARGET_OBJECTS:IrOutlet_default_default_XC8_compile>")

endif()

# Handle files with suffix elf, for group default-XC8
if(IrOutlet_default_default_XC8_FILE_TYPE_objcopy_lss)
add_library(IrOutlet_default_default_XC8_objcopy_lss OBJECT ${IrOutlet_default_default_XC8_FILE_TYPE_objcopy_lss})
    IrOutlet_default_default_XC8_objcopy_lss_rule(IrOutlet_default_default_XC8_objcopy_lss)
    list(APPEND IrOutlet_default_library_list "$<TARGET_OBJECTS:IrOutlet_default_default_XC8_objcopy_lss>")

endif()


# Main target for this project
add_executable(IrOutlet_default_image_DONtBRyu ${IrOutlet_default_library_list})

set_target_properties(IrOutlet_default_image_DONtBRyu PROPERTIES
    OUTPUT_NAME "default"
    SUFFIX ".elf"
    ADDITIONAL_CLEAN_FILES "${output_extensions}"
    RUNTIME_OUTPUT_DIRECTORY "${IrOutlet_default_output_dir}")
target_link_libraries(IrOutlet_default_image_DONtBRyu PRIVATE ${IrOutlet_default_default_XC8_FILE_TYPE_link})
# Add the link options from the rule file.
IrOutlet_default_link_rule( IrOutlet_default_image_DONtBRyu)


#Add objcopy steps
IrOutlet_default_objcopy_lss_rule(IrOutlet_default_image_DONtBRyu)

