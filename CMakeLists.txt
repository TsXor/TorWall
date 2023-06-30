cmake_minimum_required(VERSION 3.15)

project(Tallow)

set(AVAILABLE_ARCH "x32" "x64")
# default to x64
if (NOT DEFINED ARCH)
    message(STATUS "default ARCH to x64")
    set(ARCH "x64")
endif()
string(SUBSTRING ${ARCH} 1 -1 ARCH_N)
# architecture check
list(FIND AVAILABLE_ARCH ${ARCH} avail_state)
if (${avail_state} EQUAL -1)
    message(FATAL_ERROR "unsupported architecture")
endif()
if(${ARCH} STREQUAL "x64" AND ${CMAKE_HOST_SYSTEM_PROCESSOR} STREQUAL "x86")
    message(FATAL_ERROR "cannot make 64-bit program on 32-bit toolchain")
endif()

function(import_extern_dll target_name dll_name path)
    if(MINGW)
        set(lib_type MODULE)
    else()
        set(lib_type SHARED)
    endif()
    if(${path} STREQUAL MS_BUILTIN)
        if(MINGW)
            if(${ARCH} STREQUAL "x32" AND ${CMAKE_HOST_SYSTEM_PROCESSOR} STREQUAL "AMD64")
                set(dll_path C:/Windows/SysWoW64/${dll_name}.dll)
                set(lib_path C:/Windows/SysWoW64/${dll_name}.lib)
            else()
                set(dll_path C:/Windows/System32/${dll_name}.dll)
                set(lib_path C:/Windows/System32/${dll_name}.lib)
            endif()
        else()
            set(dll_path ${dll_name}.dll)
            set(lib_path ${dll_name}.lib)
        endif()
    else()
        set(dll_path ${path}/${dll_name}.dll)
        set(lib_path ${path}/${dll_name}.lib)
    endif()
    add_library(${target_name} ${lib_type} IMPORTED)
    set_property(TARGET ${target_name} PROPERTY
                IMPORTED_LOCATION ${dll_path})
    set_property(TARGET ${target_name} PROPERTY
                IMPORTED_IMPLIB ${lib_path})
endfunction()

function(target_uac_admin target_name)
    if(MSVC)
        set_target_properties(${target_name} PROPERTIES
                              LINK_FLAGS " /MANIFESTUAC:\"level='requireAdministrator' uiAccess='false'\" ")
    else()
        target_sources(${target_name} PRIVATE uac.rc)
    endif()
endfunction()


# import Windows dlls
import_extern_dll(comctl comctl32 MS_BUILTIN)
import_extern_dll(winsock ws2_32 MS_BUILTIN)

# import WinDivert
set(WINDIVERT_PATH ${CMAKE_CURRENT_LIST_DIR}/contrib/WinDivert)
import_extern_dll(WinDivert WinDivert ${WINDIVERT_PATH}/${ARCH})

# setup submodules
add_library(domain domain.c)
add_library(redirect redirect.c)
target_link_libraries(redirect WinDivert)
target_include_directories(redirect PRIVATE ${WINDIVERT_PATH})

# main program
add_executable(${PROJECT_NAME} WIN32 main.c)
target_uac_admin(${PROJECT_NAME})
target_sources(${PROJECT_NAME} PRIVATE res/compiled/main.rc) # use rc
target_sources(${PROJECT_NAME} PRIVATE res/compiled/main.manifest)
target_link_libraries(${PROJECT_NAME} comctl)
target_link_libraries(${PROJECT_NAME} winsock)
target_link_libraries(${PROJECT_NAME} domain)
target_link_libraries(${PROJECT_NAME} redirect)

# install
install(TARGETS ${PROJECT_NAME}
        DESTINATION ${CMAKE_CURRENT_LIST_DIR}/install/bin)
# copy WinDivert
install(FILES ${WINDIVERT_PATH}/${ARCH}/WinDivert.dll
              ${WINDIVERT_PATH}/${ARCH}/WinDivert${ARCH_N}.sys
        DESTINATION ${CMAKE_CURRENT_LIST_DIR}/install/bin)
# copy resources
file(GLOB res_files ${CMAKE_CURRENT_LIST_DIR}/res/installed/*)
install(FILES ${res_files}
        DESTINATION ${CMAKE_CURRENT_LIST_DIR}/install/bin)
