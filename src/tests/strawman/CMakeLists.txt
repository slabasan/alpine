###############################################################################
# Copyright (c) 2015-2017, Lawrence Livermore National Security, LLC.
# 
# Produced at the Lawrence Livermore National Laboratory
# 
# LLNL-CODE-716457
# 
# All rights reserved.
# 
# This file is part of Strawman. 
# 
# For details, see: http://software.llnl.gov/strawman/.
# 
# Please also read strawman/LICENSE
# 
# Redistribution and use in source and binary forms, with or without 
# modification, are permitted provided that the following conditions are met:
# 
# * Redistributions of source code must retain the above copyright notice, 
#   this list of conditions and the disclaimer below.
# 
# * Redistributions in binary form must reproduce the above copyright notice,
#   this list of conditions and the disclaimer (as noted below) in the
#   documentation and/or other materials provided with the distribution.
# 
# * Neither the name of the LLNS/LLNL nor the names of its contributors may
#   be used to endorse or promote products derived from this software without
#   specific prior written permission.
# 
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL LAWRENCE LIVERMORE NATIONAL SECURITY,
# LLC, THE U.S. DEPARTMENT OF ENERGY OR CONTRIBUTORS BE LIABLE FOR ANY
# DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
# DAMAGES  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
# OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
# STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
# IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
# POSSIBILITY OF SUCH DAMAGE.
# 
###############################################################################


################################
# Unit Tests
################################

################################
# Core Strawman Unit Tests
################################
set(BASIC_TESTS t_strawman_smoke
                t_strawman_empty_pipeline
                t_strawman_render_2d
                t_strawman_render_3d
                t_strawman_web)


set(MPI_TESTS  t_strawman_mpi_empty_pipeline
               t_strawman_mpi_render_2d
               t_strawman_mpi_render_3d)

if(HDF5_FOUND)
    list(APPEND BASIC_TESTS t_strawman_save_blueprint_hdf5)
    list(APPEND MPI_TESTS   t_strawman_mpi_save_blueprint_hdf5)
endif()



################################
# Add main tests
################################
message(STATUS "Adding strawman lib unit tests")
foreach(TEST ${BASIC_TESTS})
    add_cpp_test(TEST ${TEST} DEPENDS_ON strawman)
endforeach()


################################
# Add optional tests
################################

if(MPI_FOUND)
    message(STATUS "MPI enabled: Adding related unit tests")
    foreach(TEST ${MPI_TESTS})
        # this uses 2 procs
        add_cpp_mpi_test(TEST ${TEST} NUM_PROCS 2 DEPENDS_ON strawman_par) 
        set_target_properties( ${TEST} PROPERTIES COMPILE_FLAGS "-D PARALLEL")
    endforeach()
else()
    message(STATUS "MPI disabled: Skipping related tests")
endif()


if(PYTHON_FOUND)
    add_subdirectory("python")
else()
    message(STATUS "Python disabled: Skipping strawman python module tests")
endif()


if(FORTRAN_FOUND)
     add_subdirectory("fortran")
else()
     message(STATUS "Fortran disabled: Skipping strawman fortran interface tests")
endif()


