cmake_minimum_required(VERSION 3.15)

project(Tallow)

set(ARCH "x64")

function(import_extern_dll target_name dll_name)
    if(${ARGC} EQUAL 3)
        list(GET ARGN 0 path)
        add_library(${target_name} SHARED IMPORTED)
        set_property(TARGET ${target_name} PROPERTY
                     IMPORTED_LOCATION ${path}/${dll_name}.dll)
        set_property(TARGET ${target_name} PROPERTY
                     IMPORTED_IMPLIB ${path}/${dll_name}.lib)
    else()
        add_library(${target_name} SHARED IMPORTED)
        set_property(TARGET ${target_name} PROPERTY
                     IMPORTED_LOCATION ${dll_name}.dll)
        set_property(TARGET ${target_name} PROPERTY
                     IMPORTED_IMPLIB ${dll_name}.lib)
    endif()
endfunction()

function(target_uac_admin target_name)
    if(MSVC)
        set_target_properties(${target_name} PROPERTIES
                              LINK_FLAGS " /MANIFESTUAC:\"level='requireAdministrator' uiAccess='false'\" ")
    else()
        target_sources(${target_name} PRIVATE uac.manifest)
    endif()
endfunction()


# import window dlls
import_extern_dll(comctl comctl32)
import_extern_dll(winsock ws2_32)

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
target_sources(${PROJECT_NAME} PRIVATE main.rc) # use rc
target_sources(${PROJECT_NAME} PRIVATE main.manifest)
target_link_libraries(${PROJECT_NAME} comctl)
target_link_libraries(${PROJECT_NAME} winsock)
target_link_libraries(${PROJECT_NAME} domain)
target_link_libraries(${PROJECT_NAME} redirect)

# install
install(TARGETS ${PROJECT_NAME}
        DESTINATION ${CMAKE_CURRENT_LIST_DIR}/install/bin)