# QPhiX Dslash and Solver Library: version 1.0.0

Build status:

- `devel` ![](https://api.travis-ci.org/JeffersonLab/qphix.svg?branch=devel)
- `master` ![](https://api.travis-ci.org/JeffersonLab/qphix.svg?branch=master)

## Licensing Copying and Distribution:

Please see the `LICENSE` file for the License. Jefferson Science Associates
Copyright notice and Licens and Intel Corporation `Copyright` notices are also
reproduced in the file `COPYING`.

## Disclaimers

This is research software and while every effort has been made to ensure it
performs correctly it is not certified or warranted to be free of errors, bugs.
Nor is it certified or warranted that it will not crash, hang or in other way
malfunction. We likewise cannot and will not guarantee any specific performance
on any specific system and use of this software is entirely at your own risk,
as is disclaimed in the `COPYRIGHT` and `LICENSE` files

## Getting Building and Installing this library

The library is a C++ library and is mostly a collection of header files. This package also
contains some test programs.

| Program | Description |
| --- | --- |
| `./t_dslash` | Test Wilson Dslash operator (against QDP++) |
| `./t_clov_dslash` | Test the Clover Dslash operators (against implementation outlined from Chroma) |
| `./time_dslash_noqdp` | Time the Dslash operator, Wilson operator, and solvers,  without linkage to QDP++ |
| `./time_clov_noqdp` | Time the "Clover Dslash", Clover operator, and solver without linkage to QDP++ |

The library and its test programs are built with **CMake**. The kernel code
generator (`libqphix_codegen`) is built automatically during the CMake build, so
no separate code-generation step is required. The older GNU Autotools
(`autoconf`/`automake`) flow is no longer used.

### Getting the library

The library can be downloaded from the [GitHub repository](https://github.com/JeffersonLab/qphix.git)

### Dependencies

- `cmake` (>= 3.1) and `make` (or another CMake generator) -- to configure and build
- A C++11 compiler with OpenMP support -- e.g. GCC, Clang or the Intel C++ Compiler
- `python3` with the `jinja2` package -- used by the kernel code generator
- MPI for multi-node (`parscalar`) builds
- The [QDP++ library](http://usqcd-software.github.io/qdpxx/) -- **optional**,
  only required by the correctness tests that compare against QDP++ (e.g.
  `t_dslash`, `t_clov_dslash`). The `*_noqdp` timing benchmarks such as
  `time_dslash_noqdp` build and run without QDP++.
- QMP -- **optional**. Multi-node runs can use either QMP or the built-in native
  MPI backend (see "Multi-node runs with native MPI" below), so QMP is not
  required.

The code builds with modern GCC (tested with GCC 14). Future compiler support is
planned/ongoing. Please see `TODO`.

### Configuring the package

An out-of-source build is recommended. The most commonly used options are
passed to CMake with `-D<option>=<value>`:

| Option | Description |
| --- | --- |
| `-DCMAKE_INSTALL_PREFIX=<dir>` | Install location for headers, libraries and `bin/` test programs |
| `-Disa=ISA` | Target instruction set: `scalar`, `sse`, `avx`, `avx2`, `avx512`, `mic`, `qpx` (default `avx`) |
| `-Dtesting=TRUE` | Build the test / timing programs (default `OFF`) |
| `-Dclover=TRUE` | Build the Clover term kernels (default `ON`) |
| `-Dtwisted_mass=TRUE` | Build the Twisted-Mass kernels (default `OFF`) |
| `-Dtm_clover=TRUE` | Build the Twisted-Mass + Clover kernels (default `OFF`) |
| `-Dparallel_arch=scalar\|parscalar` | Single-node (`scalar`) or multi-node QMP/MPI (`parscalar`) build (default `scalar`) |
| `-DQDPXX_DIR=<dir>` | Location of a QDP++ install. Only needed for the tests that compare against QDP++ |
| `-DQMP_DIR=<dir>` | Location of a QMP install (for `parscalar` builds) |
| `-Dmm_malloc=TRUE` | Use `_mm_malloc` for aligned allocation (default `ON`; `posix_memalign` otherwise) |
| `-DCMAKE_CXX_COMPILER=<cxx>` | C++ compiler (e.g. `g++`, `mpicxx`) |
| `-DCMAKE_CXX_FLAGS="..."` | Compiler flags, including the target architecture flag (see below) |
| `-Dhost_cxx` / `-Dhost_cxxflags` | Compiler / flags used to build the code generator on the build host |

With GCC/Clang the `restrict` keyword is not accepted in C++, so add
`-Drestrict=__restrict__` to `CMAKE_CXX_FLAGS`.

Select the architecture flag to match `-Disa`:

| `-Disa` | Suggested `-march` (GCC/Clang) | SOA length |
| --- | --- | --- |
| `avx`    | `-march=sandybridge` | 4 |
| `avx2`   | `-march=haswell` | 4 |
| `avx512` | `-march=skylake-avx512` (or `-march=knl` for KNL) | 8 |

### Building And Installing

Configure, build and (optionally) install with:

    # Configure an out-of-source build (here for AVX2)
    cmake -S . -B build_avx2 \
        -Disa=avx2 \
        -Dtesting=TRUE \
        -DCMAKE_INSTALL_PREFIX="$PWD/install_avx2" \
        -DCMAKE_CXX_COMPILER=g++ \
        -DCMAKE_CXX_FLAGS="-O3 -march=haswell -Drestrict=__restrict__"

    # Build a specific target (or omit --target to build everything)
    cmake --build build_avx2 --target time_dslash_noqdp -j

    # Optional: install into CMAKE_INSTALL_PREFIX
    cmake --build build_avx2 --target install

The test / timing executables are produced under `build_<isa>/tests/` and, when
installed, in `<CMAKE_INSTALL_PREFIX>/bin/`. The correctness tests that compare
against QDP++ additionally require `-DQDPXX_DIR=<dir>`, and it is recommended to
point them at a double-precision build of QDP++.

### Example: building `time_dslash_noqdp` for AVX2 and AVX512

The `time_dslash_noqdp` Wilson-Dslash timing benchmark does not need QDP++. To
build it for both AVX2 and AVX512:

    cmake -S . -B build_avx2   -Disa=avx2   -Dtesting=TRUE -Dclover=FALSE \
        -DCMAKE_CXX_COMPILER=g++ -DCMAKE_CXX_FLAGS="-O3 -march=haswell -Drestrict=__restrict__"
    cmake --build build_avx2   --target time_dslash_noqdp -j

    cmake -S . -B build_avx512 -Disa=avx512 -Dtesting=TRUE -Dclover=FALSE \
        -DCMAKE_CXX_COMPILER=g++ -DCMAKE_CXX_FLAGS="-O3 -march=skylake-avx512 -Drestrict=__restrict__"
    cmake --build build_avx512 --target time_dslash_noqdp -j

This produces `build_avx2/tests/time_dslash_noqdp` (SOA length 4) and
`build_avx512/tests/time_dslash_noqdp` (SOA length 8). The same commands are
collected in the `build_dslash.sh` helper script. `-Dclover=FALSE` simply trims
code generation for the Wilson-only benchmark; drop it to also build the Clover
kernels.

### Building with the Intel oneAPI compiler and Intel MPI

The same build works with the Intel oneAPI C++ compiler (`icx`/`icpx`). To
enable Intel MPI, use the Intel MPI compiler wrapper `mpiicpx` (which wraps
`icpx` and links Intel MPI) as the C++ compiler. First source the oneAPI
environment, then configure with Intel-style architecture flags:

    source /opt/intel/oneapi/setvars.sh    # adjust to your oneAPI location

    # AVX2 (SOA length 4)
    cmake -S . -B build_avx2_intel   -Disa=avx2   -Dtesting=TRUE -Dclover=FALSE \
        -Dhost_cxx=icpx -Dhost_cxxflags="-O3 -std=c++11" \
        -DCMAKE_CXX_COMPILER=mpiicpx \
        -DCMAKE_CXX_FLAGS="-O3 -xCORE-AVX2 -Drestrict=__restrict__"
    cmake --build build_avx2_intel   --target time_dslash_noqdp -j

    # AVX512 (SOA length 8)
    cmake -S . -B build_avx512_intel -Disa=avx512 -Dtesting=TRUE -Dclover=FALSE \
        -Dhost_cxx=icpx -Dhost_cxxflags="-O3 -std=c++11" \
        -DCMAKE_CXX_COMPILER=mpiicpx \
        -DCMAKE_CXX_FLAGS="-O3 -xCORE-AVX512 -Drestrict=__restrict__"
    cmake --build build_avx512_intel --target time_dslash_noqdp -j

Notes:

- `-Dhost_cxx=icpx` builds the code generator with `icpx`; the target kernels
  and test programs are compiled with `mpiicpx`.
- Use Intel-style ISA flags with `icpx`: `-xCORE-AVX2` for AVX2 and
  `-xCORE-AVX512` for AVX512 (equivalently `-march=haswell` /
  `-march=skylake-avx512` are also accepted).
- The resulting executables link Intel MPI and can be launched with `mpirun`,
  e.g. `mpirun -n 1 ./build_avx2_intel/tests/time_dslash_noqdp ...`. Note that
  the default `scalar` build is single-node; for a genuine multi-node run see
  the next section.

### Multi-node runs with native MPI (no QMP, no QDP++)

QPhiX can perform its halo exchange using MPI directly, without QMP or QDP++.
This is enabled by building for the `parscalar` architecture with `mpi_comms`
turned on (the default) while providing neither QMP nor QDP++:

    source /opt/intel/oneapi/setvars.sh    # adjust to your oneAPI location

    cmake -S . -B build_avx512_mpi -Disa=avx512 -Dtesting=TRUE -Dclover=FALSE \
        -Dparallel_arch=parscalar -Dmpi_comms=ON \
        -Dhost_cxx=icpx -Dhost_cxxflags="-O3 -std=c++11" \
        -DCMAKE_CXX_COMPILER=mpiicpx \
        -DCMAKE_CXX_FLAGS="-O3 -xCORE-AVX512 -Drestrict=__restrict__"
    cmake --build build_avx512_mpi --target time_dslash_noqdp -j

Run it across multiple ranks, using `-geom Px Py Pz Pt` to describe the process
grid (the product must equal the number of MPI ranks). For example, splitting a
16x8x8x16 lattice across two ranks in the time direction:

    mpirun -n 2 ./build_avx512_mpi/tests/time_dslash_noqdp \
        -x 16 -y 8 -z 8 -t 16 -by 4 -bz 4 -c 4 -sy 1 -sz 1 \
        -dslash -prec f -soalen 8 -i 50 -geom 1 1 1 2

Notes:

- The native MPI backend sets up a 4D periodic Cartesian communicator
  (`MPI_Cart_create`) and uses `MPI_Isend`/`MPI_Irecv` for the face exchange.
- At start-up `time_dslash_noqdp` runs a self-contained *comms checksum
  self-test*: every rank exchanges a rank-independent reference pattern with its
  neighbours and verifies, element-by-element, that it receives the same pattern
  back. The number of mismatches (and a checksum) is reduced across all ranks
  and reported as `RESULT: PASS`/`FAIL`, validating the halo exchange and the
  cross-rank reduction independently of QDP++. On single-node builds it reports
  that there are no inter-rank faces.
- `parallel_arch=parscalar` normally selects the QMP backend; if a QMP or QDP++
  installation *is* provided (via `-DQMP_DIR` / `-DQDPXX_DIR`) it takes
  precedence and the QMP backend is used instead.

### Running the test programs

The test programs need the following typical command line parameters:

| Flag | Description |
| --- | --- |
| `-x Lx -y Ly -z Lz -t Lt` | the dimensions of the lattice |
| `-by By -bz Bz` | (`By` × `Bz`) block size parameters: 4×4 for MIC, 8×8 for Sandy Bridge work well |
| `-pxy Pxy -pxyz Pxyz` | padding factors Pxy Pxyz, Can be set to zero, `Pxy=1` may gain a little for particular lattices (e.g 32³×64) |
| `-c Cores` | Number of cores (all the cores in a system): e.g. 59 for Xeon Phi 5110P, 60 for Xeon Phi 7120, 16 for a dual socket 8 core  per socket Xeon  |
| `-sy Sy -sz Sz` | SMT thread configuration (`Sy=1`, `Sz=4` works well on Knights Corner, `Sy=1`, `Sz=2` works well on Xeon) |
| `-minct  Ct` | Minimum number of blocks in time. This should be the number of sockets ideally. |
| `-compress12` | Employ two row compression (12 number compression) |

For timing routines one can also set: `-i <iters>` Number of timing iters to perform.
For multi-node running one can add: `-geom Px Py Pz Pt` to specify a (`Px Py Pz Pt`) virtual node geometry. 
NB: Cores refers to the number of cores per node.

Some tests may occationally support:

`-prec=PRECISION`: precision to work in `h`=half, `f`=single, `d`=double

For the `*_noqdp` timing benchmarks, additionally select the operation(s) to
time (`-dslash`, `-mmat`, `-cg`, `-bicgstab`) and the SOA length with `-soalen`.
E.g. to time the Wilson Dslash built above on a dual socket 8-core-per-socket
Xeon:

    # AVX2 (SOA length 4)
    ./build_avx2/tests/time_dslash_noqdp -x 32 -y 32 -z 32 -t 32 -by 8 -bz 8 \
        -pxy 1 -pxyz 0 -c 16 -sy 1 -sz 2 -minct 1 -compress12 -dslash -prec f -soalen 4 -i 500

    # AVX512 (SOA length 8)
    ./build_avx512/tests/time_dslash_noqdp -x 32 -y 32 -z 32 -t 32 -by 8 -bz 8 \
        -pxy 1 -pxyz 0 -c 16 -sy 1 -sz 2 -minct 1 -compress12 -dslash -prec f -soalen 8 -i 500

E.g. a typical clover dslash test on a dual socket 8 core-per-socket Xeon System would go like:

    ./t_clov_dslash -x 32 -y 32 -z 32 -t 64 -by 8 -bz 8 -pxy 1 -pxyz 0 -c 16 -sy 1 -sz 2 -minct 2 -compress12 -geom 1 1 1 1 -i 500

Whereas on a Xeon Phi 7120 it would be

    ./t_clov_dslash -x 32 -y 32 -z 32 -t 64 -by 4 -bz 4 -pxy 1 -pxyz 0 -c 60 -sy 1 -sz 4 -minct 1 -compress12 -geom 1 1 1 1 -i 500

## Integration with Chroma

### Minimum Chroma Revision

The minimum revision for using this library with Chroma is Git Commit ID:  

    e558743151ff30598b3d0e374b2ccb6fe95a5fbc

from the Chroma distribution on GitHub: https://github.com/JeffersonLab/chroma.git

### Configuring Chroma for building with QPhiX 

QPhiX has been integrated with chroma as a LinOpSysSolver. To use the following
configure options neeed to be passed to Chroma
   
| Flag | Description |
| --- | --- |
| `--with-qphix-solver=<QPhiX Install directory>` | specify the location of the installed QPhiX library |
| `--enable-qphix-solver-arch=PROC` | specify the QPhiX processor architecture (avx,mic) |
| `--enable-qphix-solver-soalen=SOA` | the SOA Length for the problem (4,8,16 etc as appropriate) |
| `--enable-qphix-solver-compress12` | add this option if 12 compression is required |
| `--enable-qphix-solver-inner-type=f` | precision of the inner solver for mixed prec: (`h`=half, `f`=single, `d`=double) |
| `--enable-qphix-solver-inner-soalen=4` | SOA length of inner solver |

### XML Drivers

The XML tag group to use the lin op solver in Propagator applications looks like this for non-mixed precision solvers:

    <InvertParam>
       <invType>QPHIX_CLOVER_INVERTER</invType>
       <SolverType>BICGSTAB</SolverType>
       <MaxIter>1000</MaxIter>		<!-- Maximum iterations before giving up -->
       <RsdTarget>1.0e-7</RsdTarget>		<!-- Desired Target relative residuum -->
       <CloverParams>                         <!-- repeat params in the FermionAction, both Mass and Kappa are OK -->
         <Kappa>0.115</Kappa>
         <clovCoeff>2.0171</clovCoeff>
         <clovCoeffR>2.0171</clovCoeffR>
         <clovCoeffT>0.95</clovCoeffT>
         <AnisoParam>
           <anisoP>true</anisoP>
           <t_dir>3</t_dir>
           <xi_0>2.9</xi_0>
           <nu>0.94</nu>
         </AnisoParam>
       </CloverParams>
       <AntiPeriodicT>false</AntiPeriodicT>    <!-- set to true if BCs are antiperiodic -->
       <Verbose>false</Verbose>
       <NCores>16</NCores>                     <!-- number of cores per node -->
       <ThreadsPerCore>2</ThreadsPerCore>      <!-- number of threads per core -->
       <By>8</By>				  <!-- By and Bz block dimensions -->
       <Bz>8</Bz>
       <Sy>1</Sy>                              <!-- Sy and Sz hyperthread grid dimensions -->
       <Sz>2</Sz>
       <PadXY>1</PadXY>                        <!-- PadXY and PadXYZ padding factors -->
       <PadXYZ>0</PadXYZ>
       <MinCt>2</MinCt>                             <!-- Ct at start -- no of temporal blocks. Set equal to no of sockets -->
       <RsdToleranceFactor>5.0</RsdToleranceFactor> <!-- tolerate slack between iterated and true residua, e,g, due to preconditioning etc -->
       <Tune>true</Tune>                            <!-- tune BLAS routines prior to solve -->
     </InvertParam>

For the Iterative Refinement solver the XML is similar:

    <InvertParam>
      <invType>QPHIX_CLOVER_ITER_REFINE_BICGSTAB_INVERTER</invType>
      <SolverType>BICGSTAB</SolverType>  <!-- Inner solver -->
      <MaxIter>1000</MaxIter>	     <!-- Max outer iters -->
      <RsdTarget>1.0e-7</RsdTarget>      <!-- Target relative residuum for outer solver -->
      <Delta>0.1</Delta>		     <!-- Relative residuum drop factor in inner solve. Ie inner solve reduces residuum this much -->
      <CloverParams>		     <!-- Action params same as in FermionAction -->
        <Kappa>0.115</Kappa>
        <clovCoeff>2.0171</clovCoeff>
        <clovCoeffR>2.0171</clovCoeffR>
        <clovCoeffT>0.95</clovCoeffT>
        <AnisoParam>
          <anisoP>true</anisoP>
          <t_dir>3</t_dir>
          <xi_0>2.9</xi_0>
          <nu>0.94</nu>
        </AnisoParam>
      </CloverParams>
      <AntiPeriodicT>false</AntiPeriodicT> <-- set to true for antiperiodic BCs -->
      <Verbose>false</Verbose>
      <NCores>16</NCores>			<!-- Number of cores per node -->
      <ThreadsPerCore>2</ThreadsPerCore>    <!-- Number of threads per core -->
      <By>8</By>				<!-- Blocking dimensions By and Bz -->
      <Bz>8</Bz>
      <Sy>1</Sy>				<!-- SMT Thread dimensions Sy and Sz -->
      <Sz>2</Sz>
      <PadXY>1</PadXY>			<!-- Padding dimensions PadXY and PadXYZ -->
      <PadXYZ>0</PadXYZ>
      <MinCt>1</MinCt>			<!-- Initial Ct = set to number of sockets -->
      <RsdToleranceFactor>5.0</RsdToleranceFactor>   <!-- Slop factor tolerated between solver and true residua, e.g. due to preconditioning -->
      <Tune>true</Tune>			<!-- set to true to tune BLAS -->
    </InvertParam>

### General pointers about the code

 - this is a C++ code and uses templates
 - The code parameters Floating point type, Vector length, SOA length, and a boolean to indicate 12 compression are typically template parameters e.g.

        Geometry<FT,V,S,compress12>
        Dslash<FT,V,S,compress12> etc...

 -  These parameters typically relate to types or array dimensions and hence need to be set at compile time. This is annoying (especially for the compression) but there it is.

 - The `Geometry<FT,V,S,compress>` class defines the node geometry, blocking, padding etc. It also supplies allocation functions for gauges, spinors, and clover terms.
 - The `Comm<FT,V,S,compress>` class defines multi-node communications.

 - Code generated by QPhiX codegen is copied to the directory: 

        qphix/include/qphix/mic/generated  - for MIC
        qphix/include/qphix/avx/generated  - for AVX

The kernels in the generated directories are included with a combination of:

    qphix/include/qphix/<ARCH>/xxxx_complete_specialization_form.h 

and

    qphix/include/qphix/<ARCH>/xxxx_complete_specialization.h 

where `ARCH=mic` or `ARCH=avx` 
and `xxx` is `dslash_ARCH` or `clov_dslash_ARCH` ie: `dslash_avx clov_dslash_avx` 

The dslash site loops and communications are defined in the functions 

- `DyzPlus` Dslash
- `DyzMinus`: Dslash Dagger 

etc in the files:

    qphix/include/dslash_body.h and qphix/include/clover_dslash_body.h

- Blas linear algebra is implemented by combining site loops which loop over the SOA-s in a vector 
   with Functors to be executed on the vector. The loops are defined in `qphix/include/qphix/site_loops.h`
   and the various functors are defined in `qphix/include/real_functors.h` `qphix/include/complex_functors.h` 

- Solvers are currently implemnented in

    - `invcg.h` -- Conjugate Gradients:   solves `M^\dagger M x = b`
    - `invbicgstab.h` -- BICGStab :       solves `M x = b` 
    - `inv_richardson_multiprec.h` -- Solves `M x = b` using iterative refinement (aka defect correction).

Please see the source code in the tests/ directory on how to set these up:

e.g. in  `tests/testDslashFull.cc` 

The Dslash operators themselves provide two main operations:
      
- `y = D x` (or  `y = A^{-1} D x`)  -- called `Dslash`
- `z = a x - b D y` (or  `z = A x - b D y`) -- so called `achimbdpsi` versions -- `a`, `b` are constants, `A` is the clover term)

From this one can easily construct the Schur preconditioned operators: 
e.g. `y = (A - D A^{-1} D) x`
is constructed as  `z = A^{-1}D x`, `y = Ax - bDz` 

Contact Balint Joo, bjoo@jlab.org
