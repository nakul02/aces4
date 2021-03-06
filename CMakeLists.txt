cmake_minimum_required (VERSION 2.8 FATAL_ERROR)

project(aces4)

enable_language(Fortran)


#----------------------------------------------------------------------
# Options, Settings, Libraries, Packages
#----------------------------------------------------------------------

# All custom find modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

# Find required packages for aces4.
find_package(LAPACK REQUIRED)
find_package(OpenMP)

##################
# HAVE_MPI Option
##################
option(HAVE_MPI "Whether to use MPI" ON)

# Check if MPI available on system
if (HAVE_MPI)
    find_package(MPI REQUIRED)
endif()


####################
# HAVE_CUDA Options - Whether to use CUDA accelerated super instructions
# Disabled for the time being
####################

#option(HAVE_CUDA "Whether to use CUDA accelerated super instructions" OFF)

## Check if CUDA available on system
#if (HAVE_CUDA)
#    find_package(CUDA REQUIRED)
#endif()

####################
# SIP_DEVEL Options - Whether to enable extra logging for the entire SIP
####################
option(SIP_DEVEL "Whether to enable extra logging for the entire SIP" OFF)

if (SIP_DEVEL)
    message(STATUS "   SIP_DEVEL Flag enabled    ")
    message(STATUS "-----------------------------")
    message(STATUS "There will be lots of print !")
    message(STATUS "-----------------------------")
endif()



#Config file 
configure_file(${CMAKE_SOURCE_DIR}/config.h.cmake ${CMAKE_BINARY_DIR}/config.h)

#----------------------------------------------------------------------
# ACES4 sources, libraries, includes
#----------------------------------------------------------------------
set(ACES_SETUP_FILES 
    src/sip/setup/io_utils.h;
    src/sip/setup/setup_reader.h;
    src/sip/setup/setup_interface.h;
    src/sip/setup/setup_writer.h;
    src/sip/setup/setup_reader.cpp;
    src/sip/setup/setup_interface.cpp;
    src/sip/setup/setup_writer.cpp;
    src/sip/setup/io_utils.cpp)

set(ACES_CORE_FILES 
    src/sip/core/array_constants.h;
    src/sip/core/array_constants.cpp;
    src/sip/core/sip.h;
    src/sip/core/sip.cpp;
    src/sip/core/aces_defs.h;
    src/sip/core/aces_log.h;
    src/sip/core/aces_log.cpp;
    src/sip/core/job_control.h;
    src/sip/core/job_control.cpp)

# MPI - Conditional compile for MPI files
if (HAVE_MPI AND MPI_CXX_FOUND)
    set(ACES_MPI_FILES
        src/sip/mpi/data_distribution.cpp ;
        src/sip/mpi/data_distribution.h;
        src/sip/mpi/sip_mpi_utils.h;
        src/sip/mpi/sip_mpi_utils.cpp;
        src/sip/mpi/sip_mpi_constants.h;
        src/sip/mpi/sip_mpi_constants.cpp;
        src/sip/mpi/barrier_support.h;
        src/sip/mpi/async_acks.h;
        src/sip/mpi/async_acks.cpp;
        src/sip/mpi/sip_server.h;
        src/sip/mpi/sip_server.cpp;
        src/sip/worker/sial_ops_parallel.h;
        src/sip/worker/sial_ops_parallel.cpp;
        src/sip/mpi/server_block.h;
        src/sip/mpi/server_block.cpp;
        src/sip/mpi/disk_backed_block_map.h;
        src/sip/mpi/disk_backed_block_map.cpp;
        src/sip/mpi/server_persistent_array_manager.h;
        src/sip/mpi/server_persistent_array_manager.cpp;
        src/sip/mpi/async_ops.h;
        src/sip/mpi/async_ops.cpp; 
        src/sip/mpi/counter.h;
        src/sip/mpi/counter.cpp;   
        src/sip/mpi/array_file.h;
        src/sip/mpi/array_file.cpp;
        src/sip/mpi/chunk_manager.h;
        src/sip/mpi/chunk_manager.cpp;  
        src/sip/mpi/chunk.h;
        src/sip/mpi/chunk.cpp;          
        src/sip/dynamic_data/mpi_state.h;
        src/sip/dynamic_data/distributed_block_consistency.h;
        src/sip/dynamic_data/distributed_block_consistency.cpp      
 )
else ()
    set(ACES_MPI_FILES "")
endif()

# CUDA - Conditional compile for CUDA super instructions
if (HAVE_CUDA AND CUDA_FOUND)
    set(ACES_CUDA_FILES
        src/sip/cuda/gpu_super_instructions.cu
        src/sip/cuda/cuda_check.h)
else ()
    set(ACES_CUDA_FILES "")
endif()

if (HAVE_JSON)
    set(ACES_JSON_DIR
       ${CMAKE_SOURCE_DIR}/jsoncpp)
    set(JSON_LIBRARIES
       jasoncpp)
else ()
    set(ACES_JSON_FILES "")
endif()
    
    

set(ACES_DATA_FILES
    src/sip/static_data/array_table.h;
    src/sip/static_data/array_table.cpp;
    src/sip/static_data/index_table.h;
    src/sip/static_data/index_table.cpp;
    src/sip/static_data/int_table.h;
    src/sip/static_data/int_table.cpp;
    src/sip/static_data/sip_tables.h;
    src/sip/static_data/sip_tables.cpp;
    src/sip/static_data/opcode.h;
    src/sip/static_data/opcode.cpp;
    src/sip/static_data/op_table.h;
    src/sip/static_data/op_table.cpp;
    src/sip/static_data/create_map.h;
    src/sip/dynamic_data/block_id.h;
    src/sip/dynamic_data/block_id.cpp;
    src/sip/dynamic_data/block_shape.h;
    src/sip/dynamic_data/block_shape.cpp;
    src/sip/dynamic_data/block_selector.h;
    src/sip/dynamic_data/block_selector.cpp;
    src/sip/dynamic_data/block.h;
    src/sip/dynamic_data/block.cpp;
    src/sip/dynamic_data/block_manager.h;
    src/sip/dynamic_data/block_manager.cpp;
    src/sip/dynamic_data/contiguous_array_manager.h;
    src/sip/dynamic_data/contiguous_array_manager.cpp;
    src/sip/dynamic_data/data_manager.h;
    src/sip/dynamic_data/data_manager.cpp;
    src/sip/dynamic_data/id_block_map.h;
    src/sip/dynamic_data/worker_persistent_array_manager.h;
    src/sip/dynamic_data/worker_persistent_array_manager.cpp;
    src/sip/dynamic_data/lru_array_policy.h;
    src/sip/dynamic_data/cached_block_map.h;
    src/sip/dynamic_data/cached_block_map.cpp;
    src/sip/dynamic_data/contiguous_local_array_manager.h;
    src/sip/dynamic_data/contiguous_local_array_manager.cpp;
    src/sip/dynamic_data/memory_tracker.h;
    src/sip/dynamic_data/memory_tracker.cpp;)
#need only header files for templates--these include .cpp

set(ACES_WORKER_FILES
    src/sip/worker/loop_manager.cpp;
    src/sip/worker/loop_manager.h;
    src/sip/worker/fragment_loop_manager.cpp;
    src/sip/worker/fragment_loop_manager.h;
    src/sip/worker/pardo_loop_factory.cpp;
    src/sip/worker/pardo_loop_factory.h;
    src/sip/worker/interpreter.cpp;
    src/sip/worker/interpreter.h;
    src/sip/worker/siox_reader.h;
    src/sip/worker/siox_reader.cpp;
    src/sip/worker/sial_ops_sequential.h;
    src/sip/worker/sial_ops_sequential.cpp;
    src/sip/worker/sial_printer.h;
    src/sip/worker/sial_printer.cpp;
    src/sip/worker/sial_math.h;
    src/sip/worker/sial_math.cpp;
    src/sip/worker/tracer.h;
    src/sip/worker/tracer.cpp;
    src/sip/sip_interface.h;
    src/sip/sip_interface.cpp;
    src/sip/tensor_algebra/tensor_ops_c_prototypes.h;
    src/sip/cuda/gpu_super_instructions.h;
    src/sip/mpi/sip_mpi_attr.h;
    src/sip/mpi/sip_mpi_attr.cpp;
    src/sip/mpi/rank_distribution.h ;
    src/sip/mpi/rank_distribution.cpp ;)
    
#set(aces4_SRCS
#    ${ACES_SETUP_FILES};
#    ${ACES_CORE_FILES};
#    ${ACES_DATA_FILES};
#    ${ACES_MPI_FILES};
#    ${ACES_WORKER_FILES})

# Dmitry's Tensor library
set(tensordil_SRCS src/sip/tensor_algebra/tensor_dil_omp.F90)

set(INCLUDE_DIRS     
    ${CMAKE_BINARY_DIR};
    ${MPI_INCLUDE_PATH};
    ${CMAKE_SOURCE_DIR}/src/sip;
    ${CMAKE_SOURCE_DIR}/src/sip/super_instructions;
    ${CMAKE_SOURCE_DIR}/src/sip/tensor_algebra;
    ${CMAKE_SOURCE_DIR}/src/sip/core;
    ${CMAKE_SOURCE_DIR}/src/sip/setup;
    ${CMAKE_SOURCE_DIR}/src/sip/worker;
    ${CMAKE_SOURCE_DIR}/src/sip/static_data;
    ${CMAKE_SOURCE_DIR}/src/sip/dynamic_data;
    ${CMAKE_SOURCE_DIR}/src/sip/mpi;
    ${CMAKE_SOURCE_DIR}/src/sip/cuda;
    ${ACES_JSON_DIR};
    ${CMAKE_SOURCE_DIR}/test)

set (TOLINK_LIBRARIES
    ${TOLINK_LIBRARIES}
    tensordil;
    ${JSON_LIBRARIES};
    ${MPI_LIBRARIES};
    superinstructions # Points to : ${CMAKE_BINARY_DIR}/src/sip/super_instructions/libsuperinstructions.a;
    ${LAPACK_LIBRARIES})


# CUDA Super instructions
if (CUDA_FOUND)
    set(CUDA_HOST_COMPILATION_CPP ON)
    CUDA_INCLUDE_DIRECTORIES(${INCLUDE_DIRS})
    
    # Support Double precision
    set(CUDA_NVCC_FLAGS ${CUDA_NVCC_FLAGS} -arch=sm_13) 
    CUDA_ADD_LIBRARY(cudasuperinstructions ${ACES_CUDA_FILES})
    CUDA_ADD_CUBLAS_TO_TARGET(cudasuperinstructions)
    set(TOLINK_LIBRARIES ${TOLINK_LIBRARIES} cudasuperinstructions)
endif()

# Accumulating Compile & Link Flags

set(ACES4_COMPILE_FLAGS "")
set(LIBTENSORDIL_DEFINITIONS "")
set(ACES4_LINK_FLAGS "")

# Add LAPACK Linker Flags
set (ACES4_LINK_FLAGS ${ACES4_LINK_FLAGS} ${LAPACK_LINKER_FLAGS})

# Add OpenMP Flags
if (OPENMP_FOUND)
    set(LIBTENSORDIL_DEFINITIONS _OPENMP)
    set(ACES4_LINK_FLAGS ${ACES4_LINK_FLAGS} ${OpenMP_CXX_FLAGS})
endif()

# Add MPI Flags
if (MPI_CXX_FOUND)
    set(ACES4_COMPILE_FLAGS ${ACES4_COMPILE_FLAGS} ${MPI_COMPILE_FLAGS})
    set(ACES4_LINK_FLAGS ${ACES4_LINK_FLAGS} ${MPI_LINK_FLAGS})
endif()

#----------------------------------------------------------------------
# Testing Using Google Tests
#----------------------------------------------------------------------

##############
# Setup Tests
##############
set(gtest_disable_pthreads ON CACHE BOOL "Disable uses of pthreads in gtest.") 
add_subdirectory (test/googletest-release-1.7.0)
enable_testing()
set(INCLUDE_DIRS ${INCLUDE_DIRS} ${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

# COPY FILES NEEDED FOR TESTS TO THE BUILD DIRECTORY
configure_file(${CMAKE_SOURCE_DIR}/test/ccsdpt_test.dat ${CMAKE_BINARY_DIR}/ COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/test/second_ccsdpt_test.dat ${CMAKE_BINARY_DIR}/ COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/test/cis_test.dat ${CMAKE_BINARY_DIR}/ COPYONLY)
#configure_file(${CMAKE_SOURCE_DIR}/test/eom_test.dat ${CMAKE_BINARY_DIR}/ COPYONLY)
#configure_file(${CMAKE_SOURCE_DIR}/test/eom_water_dimer_test.dat ${CMAKE_BINARY_DIR}/ COPYONLY)
#configure_file(${CMAKE_SOURCE_DIR}/test/eom_lccsd_test.dat ${CMAKE_BINARY_DIR}/ COPYONLY)
#configure_file(${CMAKE_SOURCE_DIR}/test/eom_lccd_test.dat ${CMAKE_BINARY_DIR}/ COPYONLY)
#configure_file(${CMAKE_SOURCE_DIR}/test/eom_mp2_test.dat ${CMAKE_BINARY_DIR}/ COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/test/eom_ccsd_water_test.dat ${CMAKE_BINARY_DIR}/ COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/test/eom_ccsd_water_right_test.dat ${CMAKE_BINARY_DIR}/ COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/test/mcpt2_test.dat ${CMAKE_BINARY_DIR}/ COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/test/frag_scf_333_test.dat ${CMAKE_BINARY_DIR}/ COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/test/mcpt2_water_test.dat ${CMAKE_BINARY_DIR}/ COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/test/fef_ccpt2_water_test.dat ${CMAKE_BINARY_DIR}/ COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/test/mcpt2_222_test.dat ${CMAKE_BINARY_DIR}/ COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/test/lindep_test.dat ${CMAKE_BINARY_DIR}/ COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/test/scf_rhf_aguess_test.dat ${CMAKE_BINARY_DIR}/ COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/test/scf_uhfrhf_aguess_test.dat ${CMAKE_BINARY_DIR}/ COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/test/scf_uhf_triplet_aguess_test.dat ${CMAKE_BINARY_DIR}/ COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/test/rlambda_test.dat ${CMAKE_BINARY_DIR}/ COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/test/lccd_test.dat ${CMAKE_BINARY_DIR}/ COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/test/lccd_frozencore_test.dat ${CMAKE_BINARY_DIR}/ COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/test/lccsd_test.dat ${CMAKE_BINARY_DIR}/ COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/test/lamccsdpt_test.dat ${CMAKE_BINARY_DIR}/ COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/test/expected_output/helloworld1.txt ${CMAKE_BINARY_DIR}/ COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/test/expected_output/local_arrays_wild.txt ${CMAKE_BINARY_DIR}/ COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/test/expected_output/local_arrays.txt ${CMAKE_BINARY_DIR}/ COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/test/expected_output/loop_over_simple_indices.txt ${CMAKE_BINARY_DIR}/ COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/test/expected_output/scalar_valued_blocks.txt ${CMAKE_BINARY_DIR}/ COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/test/expected_output/scalars.txt ${CMAKE_BINARY_DIR}/ COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/test/expected_output/static_array_test.txt ${CMAKE_BINARY_DIR}/ COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/test/expected_output/static_array_test2.txt ${CMAKE_BINARY_DIR}/ COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/test/expected_output/tmp_arrays_2.txt ${CMAKE_BINARY_DIR}/ COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/test/expected_output/tmp_arrays.txt ${CMAKE_BINARY_DIR}/ COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/test/test_1el_ecp_ints.dat ${CMAKE_BINARY_DIR}/ COPYONLY)

#----------------------------------------------------------------------
# Build aces4 & other executables
#----------------------------------------------------------------------

include_directories(${INCLUDE_DIRS})

# Static Libraries for ACES4 
add_library(aces4_sip     
            ${ACES_SETUP_FILES}     
            ${ACES_CORE_FILES}
            ${ACES_WORKER_FILES}
            ${ACES_DATA_FILES}
            ${ACES_MPI_FILES})

set(TOLINK_LIBRARIES aces4_sip ${TOLINK_LIBRARIES})

# All Super instructions are in this directory
add_subdirectory(src/sip/super_instructions)

# Add Jsoncpp library
if(HAVE_JSON)
add_library(jsoncpp;
    jsoncpp/jsoncpp.cpp;
    jsoncpp/json/json.h;
    jsoncpp/json/json-forwards.h)
endif()

# The tensor library is added separately
add_library(tensordil ${tensordil_SRCS})
set_target_properties(tensordil PROPERTIES COMPILE_DEFINITIONS "${LIBTENSORDIL_DEFINITIONS}")

# executable for main program
add_executable(aces4 src/aces_main/main.cpp)

# executables for tools in the util directory
add_executable(print_siptables src/util/print_siptables.cpp)
add_executable(print_array_info src/util/print_array_info.cpp)
add_executable(print_init_file src/util/print_init_file.cpp)
add_executable(print_worker_checkpoint src/util/print_worker_checkpoint.cpp)

if(HAVE_MPI)
	add_executable(check_system src/util/check_system.cpp)
endif()

## dump_array_file executable
#add_executable(dump_array_file src/util/dump_array_file.cpp)


# Setting Compile & link flags
set_target_properties(aces4 PROPERTIES COMPILE_FLAGS "${ACES4_COMPILE_FLAGS}")
set_target_properties(aces4 PROPERTIES LINK_FLAGS "${ACES4_LINK_FLAGS}")

set_target_properties(print_siptables PROPERTIES COMPILE_FLAGS "${ACES4_COMPILE_FLAGS}")
set_target_properties(print_siptables PROPERTIES LINK_FLAGS "${ACES4_LINK_FLAGS}")

set_target_properties(print_array_info PROPERTIES COMPILE_FLAGS "${ACES4_COMPILE_FLAGS}")
set_target_properties(print_array_info PROPERTIES LINK_FLAGS "${ACES4_LINK_FLAGS}")

set_target_properties(print_init_file PROPERTIES COMPILE_FLAGS "${ACES4_COMPILE_FLAGS}")
set_target_properties(print_init_file PROPERTIES LINK_FLAGS "${ACES4_LINK_FLAGS}")

set_target_properties(print_worker_checkpoint PROPERTIES COMPILE_FLAGS "${ACES4_COMPILE_FLAGS}")
set_target_properties(print_worker_checkpoint PROPERTIES LINK_FLAGS "${ACES4_LINK_FLAGS}")

if (HAVE_MPI)
	set_target_properties(check_system PROPERTIES COMPILE_FLAGS "${ACES4_COMPILE_FLAGS}")
	set_target_properties(check_system PROPERTIES LINK_FLAGS "${ACES4_LINK_FLAGS}")
endif()



# Libraries to link
target_link_libraries(aces4 ${TOLINK_LIBRARIES}) 
target_link_libraries(print_siptables ${TOLINK_LIBRARIES}) 
target_link_libraries(print_array_info ${TOLINK_LIBRARIES}) 
target_link_libraries(print_init_file ${TOLINK_LIBRARIES})
target_link_libraries(print_worker_checkpoint ${TOLINK_LIBRARIES})

if (HAVE_MPI)
	target_link_libraries(check_system ${TOLINK_LIBRARIES})
endif()

# Dependencies
if(HAVE_CUDA)
	add_dependencies(aces4 tensordil superinstructions cudasuperinstructions)
	add_dependencies(print_siptables tensordil superinstructions cudasuperinstructions)
	add_dependencies(print_array_info tensordil superinstructions cudasuperinstructions)
	add_dependencies(print_init_file tensordil superinstructions cudasuperinstructions)
	add_dependencies(print_worker_checkpoint tensordil superinstructions cudasuperinstructions)
else()
	add_dependencies(aces4 tensordil superinstructions)
	add_dependencies(print_siptables tensordil superinstructions)
	add_dependencies(print_array_info tensordil superinstructions)
	add_dependencies(print_init_file tensordil superinstructions)
	add_dependencies(print_worker_checkpoint tensordil superinstructions)
endif()

add_dependencies(superinstructions aces4_sip tensordil)

#----------------------------------------------------------------------
# Add Tests
#----------------------------------------------------------------------

add_library(test_controllers
            test/test_controller.h;
            test/test_controller.cpp;
            test/test_controller_parallel.h;
            test/test_controller_parallel.cpp;
            test/test_constants.h;
            test/test_constants.cpp)



add_executable(test_basic_sial      test/test_basic_sial.cpp;
                                    test/test_transpose_op.F; 
                                    test/test_transpose4d_op.F;
                                    test/test_contraction_small2.F)
add_executable(test_sial  	    test/test_sial.cpp)
add_executable(test_basic_qm        test/test_basic_qm.cpp)
add_executable(test_qm              test/test_qm.cpp)
add_executable(test_qm_aux          test/test_qm_aux.cpp)
add_executable(test_qm_frag         test/test_qm_frag.cpp)
add_executable(test_unit            test/test_unit.cpp)

# Link test executable against gtest & gtest_main
set_target_properties(test_basic_sial   PROPERTIES COMPILE_FLAGS  "${ACES4_COMPILE_FLAGS}")
set_target_properties(test_sial         PROPERTIES COMPILE_FLAGS  "${ACES4_COMPILE_FLAGS}")
set_target_properties(test_basic_qm     PROPERTIES COMPILE_FLAGS  "${ACES4_COMPILE_FLAGS}")
set_target_properties(test_qm           PROPERTIES COMPILE_FLAGS  "${ACES4_COMPILE_FLAGS}")
set_target_properties(test_qm_aux       PROPERTIES COMPILE_FLAGS  "${ACES4_COMPILE_FLAGS}")
set_target_properties(test_qm_frag      PROPERTIES COMPILE_FLAGS  "${ACES4_COMPILE_FLAGS}")
set_target_properties(test_unit         PROPERTIES COMPILE_FLAGS  "${ACES4_COMPILE_FLAGS}")

target_link_libraries(test_basic_sial   test_controllers gtest gtest_main  ${TOLINK_LIBRARIES})
target_link_libraries(test_sial         test_controllers gtest gtest_main  ${TOLINK_LIBRARIES})
target_link_libraries(test_basic_qm     test_controllers gtest gtest_main  ${TOLINK_LIBRARIES})
target_link_libraries(test_qm           test_controllers gtest gtest_main  ${TOLINK_LIBRARIES})
target_link_libraries(test_qm_aux       test_controllers gtest gtest_main  ${TOLINK_LIBRARIES})
target_link_libraries(test_qm_frag      test_controllers gtest gtest_main  ${TOLINK_LIBRARIES})
target_link_libraries(test_unit         test_controllers gtest gtest_main  ${TOLINK_LIBRARIES})

set_target_properties(test_basic_sial   PROPERTIES LINK_FLAGS  "${ACES4_LINK_FLAGS}")
set_target_properties(test_sial         PROPERTIES LINK_FLAGS  "${ACES4_LINK_FLAGS}")
set_target_properties(test_basic_qm     PROPERTIES LINK_FLAGS  "${ACES4_LINK_FLAGS}")
set_target_properties(test_qm           PROPERTIES LINK_FLAGS  "${ACES4_LINK_FLAGS}")
set_target_properties(test_qm_aux       PROPERTIES LINK_FLAGS  "${ACES4_LINK_FLAGS}")
set_target_properties(test_qm_frag      PROPERTIES LINK_FLAGS  "${ACES4_LINK_FLAGS}")
set_target_properties(test_unit         PROPERTIES LINK_FLAGS  "${ACES4_LINK_FLAGS}")


add_dependencies(test_basic_sial   gtest gtest_main test_controllers)
add_dependencies(test_sial         gtest gtest_main test_controllers)
add_dependencies(test_basic_qm     gtest gtest_main test_controllers)
add_dependencies(test_qm           gtest gtest_main test_controllers)
add_dependencies(test_qm_aux       gtest gtest_main test_controllers)
add_dependencies(test_qm_frag      gtest gtest_main test_controllers)
add_dependencies(test_unit         gtest gtest_main test_controllers)


if (HAVE_MPI) # Multi Node Version Tests
    add_test(NAME test_unit         COMMAND ${MPIEXEC} ${MPIEXEC_NUMPROC_FLAG} 2 ${MPIEXEC_PREFLAGS} test_unit)
    add_test(NAME test_basic_sial   COMMAND ${MPIEXEC} ${MPIEXEC_NUMPROC_FLAG} 2 ${MPIEXEC_PREFLAGS} test_basic_sial)
    add_test(NAME test_sial         COMMAND ${MPIEXEC} ${MPIEXEC_NUMPROC_FLAG} 2 ${MPIEXEC_PREFLAGS} test_sial)
    add_test(NAME test_basic_qm     COMMAND ${MPIEXEC} ${MPIEXEC_NUMPROC_FLAG} 2 ${MPIEXEC_PREFLAGS} test_basic_qm)
    add_test(NAME test_qm           COMMAND ${MPIEXEC} ${MPIEXEC_NUMPROC_FLAG} 2 ${MPIEXEC_PREFLAGS} test_qm)
    add_test(NAME test_qm_frag      COMMAND ${MPIEXEC} ${MPIEXEC_NUMPROC_FLAG} 2 ${MPIEXEC_PREFLAGS} test_qm)
else() # Single Node Version Tests
    add_test(NAME test_unit         COMMAND test_unit)
    add_test(NAME test_basic_sial   COMMAND test_basic_sial)
    add_test(NAME test_sial         COMMAND test_sial)
    add_test(NAME test_basic_qm     COMMAND test_basic_qm)
    add_test(NAME test_qm           COMMAND test_qm)
endif()






#----------------------------------------------------------------------
# Compile all SIALX files
#----------------------------------------------------------------------

set(SIAL_FILES
    src/sialx/test/empty.sialx;
    src/sialx/test/static_array_test.sialx;
    src/sialx/test/ifelse.sialx;
    src/sialx/test/put_test.sialx;
    src/sialx/test/index_decs.sialx;
    src/sialx/test/predef_scalars.sialx;
    src/sialx/test/print_block_test.sialx;
    src/sialx/test/pardo_loop_with_pragma.sialx;
    src/sialx/test/tmp_arrays.sialx;
    src/sialx/test/scalars.sialx;
    src/sialx/test/insert_slice_test.sialx;
    src/sialx/test/print_scalar.sialx;
    src/sialx/test/where_clause.sialx;
    src/sialx/test/loop_over_simple_indices.sialx;
    src/sialx/test/exit_statement_test.sialx;
    src/sialx/test/no_arg_user_sub.sialx;
    src/sialx/test/helloworld.sialx;
    src/sialx/test/pardo_loop.sialx;
    src/sialx/test/pardo_loop_with_pragma.sialx;
    src/sialx/test/subindex_test.sialx;
    src/sialx/test/local_arrays.sialx;
    src/sialx/test/scalarTable.sialx;
    src/sialx/test/stringLiterals.sialx;
    src/sialx/test/aces_defs.sialx;
    src/sialx/test/fill_sequential_test.sialx;
    src/sialx/test/local_arrays_wild.sialx;
    src/sialx/test/local_arrays_wild_fail.sialx;
    src/sialx/test/basic_assign_to_static_array_test.sialx;
    src/sialx/test/contiguous_local.sialx;
    src/sialx/test/contiguous_local_2.sialx;
    src/sialx/test/contiguous_local_allocate_twice_fail.sialx;
    src/sialx/test/block_scale_assign.sialx;
    src/sialx/test/return_sval_test.sialx;
    src/sialx/test/broadcast_static.sialx;
    src/sialx/test/sum_op_test.sialx;
    src/sialx/test/transpose_tmp.sialx;
    src/sialx/test/transpose4d_tmp.sialx;
    src/sialx/test/transpose4d_square_tmp.sialx;
    src/sialx/test/tmp_arrays_2.sialx;
    src/sialx/test/contraction_small_test.sialx;
    src/sialx/test/contraction_small_test2.sialx;
    src/sialx/test/get_int_array_test.sialx;
    src/sialx/test/get_scalar_array_test.sialx;
    src/sialx/test/get_scratch_array_test.sialx;
    src/sialx/test/gpu_contraction_small_test.sialx;
    src/sialx/test/gpu_ops.sialx;
    src/sialx/test/contract_to_scalar.sialx;
    src/sialx/test/gpu_sum_op_test.sialx;
    src/sialx/test/gpu_contract_to_scalar.sialx;
    src/sialx/test/gpu_transpose_tmp.sialx;
    src/sialx/test/self_multiply_test.sialx;
    src/sialx/test/gpu_self_multiply_test.sialx;
    src/sialx/test/simple_indices_assignments.sialx;
    src/sialx/test/add_and_subtract_scalars.sialx;
    src/sialx/test/gpu_contraction_predefined_test.sialx;
    src/sialx/test/assign_to_static_array_test.sialx;
    src/sialx/test/set_persistent_test.sialx;
    src/sialx/test/restore_persistent_test.sialx;
    src/sialx/test/persistent_static_array_test1.sialx;
    src/sialx/test/persistent_static_array_test2.sialx;
    src/sialx/test/persistent_distributed_array_test1.sialx;
    src/sialx/test/persistent_distributed_array_test2.sialx;
    src/sialx/test/put_test_mpi.sialx;
    src/sialx/test/put_initialize.sialx;
    src/sialx/test/put_increment.sialx;
    src/sialx/test/put_accumulate_mpi.sialx;
    src/sialx/test/delete_mpi.sialx;
    src/sialx/test/get_mpi.sialx;
    src/sialx/test/persistent_distributed_array_mpi1.sialx;
    src/sialx/test/persistent_distributed_array_mpi2.sialx;
    src/sialx/test/persistent_distributed_array_one_of_three.sialx;
    src/sialx/test/persistent_distributed_array_two_of_three.sialx;
    src/sialx/test/persistent_distributed_array_three_of_three.sialx;
    src/sialx/test/persistent_empty_mpi1.sialx;
    src/sialx/test/persistent_empty_mpi2.sialx;
    src/sialx/test/persistent_scalars_1.sialx;
    src/sialx/test/persistent_scalars_2.sialx;
    src/sialx/test/unmatched_get.sialx;
    src/sialx/test/all_rank_print_test.sialx;
    src/sialx/test/message_number_wraparound_test.sialx;
    src/sialx/test/pardo_loop_1d.sialx;
    src/sialx/test/pardo_loop_2d.sialx;
    src/sialx/test/pardo_loop_3d.sialx;
    src/sialx/test/pardo_loop_4d.sialx;
    src/sialx/test/pardo_loop_5d.sialx;
    src/sialx/test/pardo_loop_6d.sialx;
    src/sialx/test/scalar_ops.sialx;
    src/sialx/test/int_ops.sialx;
    src/sialx/test/int_self_ops.sialx;
    src/sialx/test/scalar_valued_blocks.sialx;
    src/sialx/test/broadcast_static.sialx;
    src/sialx/test/block_scale_assign.sialx;
    src/sialx/test/return_sval_test.sialx;
    src/sialx/test/index_scalar_cast.sialx;
    src/sialx/test/contig_local3.sialx;
    src/sialx/test/cached_block_map_test.sialx;
    src/sialx/test/cached_block_map_test_no_dangling_get.sialx;
    src/sialx/test/pardo_with_where.sialx;
    src/sialx/test/pardo_load_balance_test.sialx;
    src/sialx/test/read_block_test.sialx;
    src/sialx/test/cast_indices_to_simple.sialx;
    src/sialx/test/aoladder.sialx;
    src/sialx/test/test_ecpint.sialx;
    src/sialx/test/put_accumulate_stress.sialx;
    src/sialx/test/disk_backing_test.sialx;
    src/sialx/test/disk_backing_put_acc_stress.sialx;
    src/sialx/test/decreasing_segs.sialx;
    src/sialx/test/check_block_number_calc.sialx;
    src/sialx/qm/utility/drop_core_in_sial.sialx;
    src/sialx/qm/utility/tran_rhf_no4v.sialx;
    src/sialx/qm/utility/tran_uhf_no4v.sialx;
    src/sialx/qm/utility/tran_rhf_no4v_defs.sialx;
    src/sialx/qm/utility/tran_rhf_no3v.sialx;
    src/sialx/qm/scf/scf_rhf_coreh.sialx;
    src/sialx/qm/scf/scf_defs.sialx;
    src/sialx/qm/scf/scf_uhf.sialx;
    src/sialx/qm/scf/scf_uhf_defs.sialx;
    src/sialx/qm/scf/scf_rhf.sialx; 
    src/sialx/qm/scf/scf_rhf_defs.sialx;
    src/sialx/qm/cc/mp2_rhf_disc.sialx;
    src/sialx/qm/cc/rccsd_rhf.sialx;
    src/sialx/qm/cc/rlambda_rhf.sialx;
    src/sialx/qm/cc/rlccd_rhf.sialx;
    src/sialx/qm/cc/rlccsd_rhf.sialx;
    src/sialx/qm/cc/rccsd_rhf_defs.sialx;
    src/sialx/qm/cc/rccsdpt_aaa.sialx;
    src/sialx/qm/cc/rccsdpt_aab.sialx;
    src/sialx/qm/cc/rlamccsdpt_aaa.sialx;
    src/sialx/qm/cc/rlamccsdpt_aab.sialx;
    src/sialx/qm/cc/rccpt1p2_prop.sialx;
    src/sialx/qm/eom/eom_rhf_defs.sialx  
    src/sialx/qm/eom/eom_rhf_hbar.sialx;
    src/sialx/qm/eom/eom_rhf_tran.sialx;
    src/sialx/qm/eom/eom_rhf_vars.sialx;
    src/sialx/qm/eom/eom_ccsd_rhf_right.sialx;
    src/sialx/qm/eom/eom_ccsd_rhf_left.sialx;
    src/sialx/qm/eom/rcis_rhf_defs.sialx;
    src/sialx/qm/eom/rcis_rhf_vars.sialx;
    src/sialx/qm/eom/rcis_rhf.sialx;
    src/sialx/qm/eom/rcis_d_rhf.sialx;
    src/sialx/qm/eom/rcis_ccpt2_rhf.sialx;
    src/sialx/qm/fef-cc/frag_rhf.sialx;
    src/sialx/qm/fef-cc/frag_pol_rhf.sialx;
    src/sialx/qm/fef-cc/frag_rcis_rhf.sialx;
    src/sialx/qm/fef-cc/tran_frag.sialx;
    src/sialx/qm/fef-cc/MOI_reom_defs.sialx;
    src/sialx/qm/fef-cc/MOI_reom_vars.sialx;
    src/sialx/qm/fef-cc/MOI_cis_guess.sialx;
    src/sialx/qm/fef-cc/MOI_rcis.sialx;
    src/sialx/qm/fef-cc/MOI_reom_elements.sialx;
    src/sialx/qm/fef-cc/MOI_reom_fefccpt2.sialx;
    src/sialx/qm/fef-cc/fef_ccpt2.sialx;
    src/sialx/qm/fef-cc/fef_cis_ccpt2.sialx;
    src/sialx/qm/fef-cc/mcpt2_corr.sialx;
    src/sialx/qm/fef-cc/mcpt2_corr_lowmem.sialx;
    src/sialx/qm/fef-cc/mcpt2_vars.sialx;
    src/sialx/qm/fef-cc/mcpt2_defs.sialx;
    src/sialx/qm/fef-cc/mcpt2_singles.sialx;
    src/sialx/qm/fef-cc/mcpt2_doubles.sialx;
    src/sialx/qm/fef-cc/mcpt2_util.sialx)
    

set(SIAL_COMPILER_JAR ${CMAKE_SOURCE_DIR}/compiler/SialCompiler.jar)
set(SIAL_COMPILER java -jar ${SIAL_COMPILER_JAR})
set (SIAL_COMPILED_FILES "")
foreach(F ${SIAL_FILES})
    get_filename_component(F_WE ${CMAKE_SOURCE_DIR}/${F} NAME_WE)
    get_filename_component(SIALX_FILE_PATH ${CMAKE_SOURCE_DIR}/${F} PATH)
#    get_filename_component(SIALX_FILE_LAST_PATH ${SIALX_FILE_PATH} NAME)

    #set(SIAL_COMPILE_DIR "${CMAKE_BINARY_DIR}/src/sialx/${SIALX_FILE_LAST_PATH}")
    set(SIAL_COMPILE_DIR "${CMAKE_BINARY_DIR}/src/sialx")
    add_custom_command(
        OUTPUT ${SIAL_COMPILE_DIR}/${F_WE}.siox
        COMMAND ${SIAL_COMPILER} -sp ${SIALX_FILE_PATH} ${CMAKE_SOURCE_DIR}/${F}
        COMMAND ${CMAKE_COMMAND} -E make_directory ${SIAL_COMPILE_DIR}
        COMMAND mv -f ${SIALX_FILE_PATH}/${F_WE}.siox ${SIAL_COMPILE_DIR} 
        DEPENDS ${SIAL_COMPILER_JAR} ${CMAKE_SOURCE_DIR}/${F}
        COMMENT "Compiling ${F} to ${SIAL_COMPILE_DIR}/${F_WE}.siox")
    list (APPEND SIAL_COMPILED_FILES "${SIAL_COMPILE_DIR}/${F_WE}.siox")
endforeach()

add_custom_target(SIAL_FILES_COMPILE ALL DEPENDS ${SIAL_COMPILED_FILES})



