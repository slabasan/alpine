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

###############################################################################
#
# Cloverleaf3D CMake Build for Strawman
#
###############################################################################


set(CLOVER3D_SOURCES
    PdV.f90
    PdV_kernel.f90
    accelerate.f90
    accelerate_kernel.f90
    advec_cell_driver.f90
    advec_cell_kernel.f90
    advec_mom_driver.f90
    advec_mom_kernel.f90
    advection.f90
    build_field.f90
    calc_dt.f90
    calc_dt_kernel.f90
    clover.F90
    clover_leaf.f90
    data.f90
    definitions.f90
    field_summary.f90
    field_summary_kernel.f90
    flux_calc.f90
    flux_calc_kernel.f90
    generate_chunk.f90
    generate_chunk_kernel.f90
    hydro.f90
    ideal_gas.f90
    ideal_gas_kernel.f90
    initialise.f90
    initialise_chunk.f90
    initialise_chunk_kernel.f90
    pack_kernel.f90
    parse.f90
    read_input.f90
    report.f90
    reset_field.f90
    reset_field_kernel.f90
    revert.f90
    revert_kernel.f90
    start.f90
    timer.f90
    timestep.f90
    update_halo.f90
    update_halo_kernel.f90
    viscosity.f90
    viscosity_kernel.f90
    visit.F90
    timer_c.c
    clover_main.cpp)

# cloverleaf3d reqs fortran and mpi
if(MPI_FOUND AND FORTRAN_FOUND)
    # copy over the input deck
    configure_file(clover.in ${CMAKE_CURRENT_BINARY_DIR}/clover.in COPYONLY)
    configure_file(strawman_actions.json ${CMAKE_CURRENT_BINARY_DIR}/strawman_actions.json COPYONLY)
    configure_file(strawman_options.json ${CMAKE_CURRENT_BINARY_DIR}/strawman_options.json COPYONLY)

    add_executable(cloverleaf3d_par ${CLOVER3D_SOURCES})

    if(MPI_Fortran_USE_MODULE )
        set(clover_compile_flags "${MPI_Fortran_COMPILE_FLAGS} -DUSE_MOD")
    else()
        set(clover_compile_flags "${MPI_Fortran_COMPILE_FLAGS} -DUSE_MPIF") 
    endif()

    add_target_compile_flags(TARGET cloverleaf3d_par
                             FLAGS "${clover_compile_flags}")

    target_link_libraries(cloverleaf3d_par
                          strawman_par
                          conduit_blueprint
                          ${MPI_Fortran_LIBRARIES})
endif()






