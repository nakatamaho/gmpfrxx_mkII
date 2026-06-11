find_path(MPC_INCLUDE_DIR NAMES mpc.h)
find_library(MPC_LIBRARY NAMES mpc)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(MPC
    REQUIRED_VARS MPC_LIBRARY MPC_INCLUDE_DIR
)

if(MPC_FOUND AND NOT TARGET MPC::MPC)
    add_library(MPC::MPC UNKNOWN IMPORTED)
    set_target_properties(MPC::MPC PROPERTIES
        IMPORTED_LOCATION "${MPC_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${MPC_INCLUDE_DIR}"
    )
    set(_mpc_link_dependencies)
    if(TARGET MPFR::MPFR)
        list(APPEND _mpc_link_dependencies MPFR::MPFR)
    endif()
    if(TARGET GMP::GMP)
        list(APPEND _mpc_link_dependencies GMP::GMP)
    endif()
    if(_mpc_link_dependencies)
        set_target_properties(MPC::MPC PROPERTIES
            INTERFACE_LINK_LIBRARIES "${_mpc_link_dependencies}"
        )
    endif()
endif()

mark_as_advanced(MPC_INCLUDE_DIR MPC_LIBRARY)
