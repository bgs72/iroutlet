# The following variables contains the files used by the different stages of the build process.
set(IrOutlet_default_default_XC8_FILE_TYPE_assemble)
set_source_files_properties(${IrOutlet_default_default_XC8_FILE_TYPE_assemble} PROPERTIES LANGUAGE ASM)

# For assembly files, add "." to the include path for each file so that .include with a relative path works
foreach(source_file ${IrOutlet_default_default_XC8_FILE_TYPE_assemble})
        set_source_files_properties(${source_file} PROPERTIES INCLUDE_DIRECTORIES "$<PATH:NORMAL_PATH,$<PATH:REMOVE_FILENAME,${source_file}>>")
endforeach()

set(IrOutlet_default_default_XC8_FILE_TYPE_assemblePreprocess)
set_source_files_properties(${IrOutlet_default_default_XC8_FILE_TYPE_assemblePreprocess} PROPERTIES LANGUAGE ASM)

# For assembly files, add "." to the include path for each file so that .include with a relative path works
foreach(source_file ${IrOutlet_default_default_XC8_FILE_TYPE_assemblePreprocess})
        set_source_files_properties(${source_file} PROPERTIES INCLUDE_DIRECTORIES "$<PATH:NORMAL_PATH,$<PATH:REMOVE_FILENAME,${source_file}>>")
endforeach()

set(IrOutlet_default_default_XC8_FILE_TYPE_compile
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../interrupts.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../main.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../rc5.c")
set_source_files_properties(${IrOutlet_default_default_XC8_FILE_TYPE_compile} PROPERTIES LANGUAGE C)
set(IrOutlet_default_default_XC8_FILE_TYPE_link)
set(IrOutlet_default_default_XC8_FILE_TYPE_objcopy_lss)
set(IrOutlet_default_image_name "default.elf")
set(IrOutlet_default_image_base_name "default")

# The output directory of the final image.
set(IrOutlet_default_output_dir "${CMAKE_CURRENT_SOURCE_DIR}/../../../out/IrOutlet")

# The full path to the final image.
set(IrOutlet_default_full_path_to_image ${IrOutlet_default_output_dir}/${IrOutlet_default_image_name})

# Potential output file extensions
set(output_extensions
    .hex
    .hxl
    .mum
    .o
    .sdb
    .sym
    .cmf)
list(TRANSFORM output_extensions PREPEND "${IrOutlet_default_output_dir}/${IrOutlet_default_image_base_name}")
