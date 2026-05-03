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
endif()

mark_as_advanced(MPC_INCLUDE_DIR MPC_LIBRARY)
