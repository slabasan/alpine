##################################
# uberenv host-config
##################################
# chaos_5_x86_64_ib-intel@14.0.3
##################################

# cmake from uberenv
# cmake exectuable path: /usr/gapps/visit/strawman/uberenv_libs/spack/opt/spack/chaos_5_x86_64_ib/intel-14.0.3/cmake-3.0.2-u4v44d2fngyp3brjbtfuky5cgwulhcqa/bin/cmake

#######
# using intel@14.0.3 compiler spec
#######

# c compiler used by spack
set(CMAKE_C_COMPILER "/usr/tce/packages/gcc/gcc-4.9.3/bin/gcc" CACHE PATH "")

# cpp compiler used by spack
set(CMAKE_CXX_COMPILER "/usr/tce/packages/gcc/gcc-4.9.3/bin/g++" CACHE PATH "")

# fortran compiler used by spack
set(CMAKE_Fortran_COMPILER  "/usr/tce/packages/gcc/gcc-4.9.3/bin/gfortran" CACHE PATH "")

set(ENABLE_PYTHON OFF CACHE PATH "")

# OPENMP Support
set(ENABLE_OPENMP ON CACHE PATH "")

# MPI Support
set(ENABLE_MPI  ON CACHE PATH "")

set(MPI_CC_COMPILER "/usr/tce/packages/mvapich2/mvapich2-2.2-intel-16.0.3/bin/mpicc" CACHE PATH "")

set(MPI_CXX_COMPILER "/usr/tce/packages/mvapich2/mvapich2-2.2-intel-16.0.3/bin/mpicc" CACHE PATH "")

set(MPI_Fortran_COMPILER "/usr/tce/packages/mvapich2/mvapich2-2.2-intel-16.0.3/bin/mpif90" CACHE PATH "")

set(MPIEXEC /usr/bin/srun CACHE PATH "")

set(MPIEXEC_NUMPROC_FLAG -n CACHE PATH "")



# CUDA support
set(ENABLE_CUDA OFF CACHE PATH "")
#set(ENABLE_CUDA ON CACHE PATH "")

set(CUDA_BIN_DIR /opt/cudatoolkit-7.0/bin CACHE PATH "")

# sphinx from uberenv
# not built ...
# conduit from uberenv
set(CONDUIT_DIR "/usr/workspace/wsa/labasan1/alpine-paviz/conduit/INSTALL" CACHE PATH "")
#set(CONDUIT_DIR " /usr/gapps/visit/strawman/uberenv_libs/spack/opt/spack/chaos_5_x86_64_ib/intel-14.0.3/conduit-github-naws5eho7jxgjaaldoubcarv5v3x4sgt/" CACHE PATH "")

# icet from uberenv
set(ICET_DIR "/usr/workspace/wsa/labasan1/alpine-paviz/icet/INSTALL" CACHE PATH "")

# vtkm support from uberenv
#

# tbb from uberenv
#set(STRAWMAN_VTKM_USE_TBB OFF CACHE PATH "")
set(TBB_DIR "/usr/gapps/visit/strawman/uberenv_libs/spack/opt/spack/chaos_5_x86_64_ib/intel-14.0.3/tbb-4.4.3-al6fuqhyuhr6ju4daik3mfwk5j7gcyvw" CACHE PATH "")

# vtkm from uberenv
set(VTKM_DIR "/usr/workspace/wsa/labasan1/alpine-paviz/vtk-m/INSTALL" CACHE PATH "")

# hdf5 from uberenv
set(HDF5_DIR "/usr/gapps/conduit/thirdparty_libs/stable/spack/opt/spack/chaos_5_x86_64_ib/gcc-4.9.3/hdf5-1.8.16-msbowehgkgvhlnl62fy6tb7bvefbr7h4" CACHE PATH "")

# silo from uberenv
set(SILO_DIR "/usr/gapps/conduit/thirdparty_libs/stable/spack/opt/spack/chaos_5_x86_64_ib/gcc-4.9.3/silo-4.10.1-jnuhe4xm3vtwq4mevsobhahlriuqafrg" CACHE PATH "")
# CUDA support

##################################
# end uberenv host-config
##################################
