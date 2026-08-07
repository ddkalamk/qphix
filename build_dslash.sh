cmake -S . -B build_avx2   -Disa=avx2   -Dtesting=TRUE -Dclover=FALSE \
  -DCMAKE_CXX_COMPILER=g++ -DCMAKE_CXX_FLAGS="-O3 -march=haswell -Drestrict=__restrict__"
cmake --build build_avx2   --target time_dslash_noqdp -j

cmake -S . -B build_avx512 -Disa=avx512 -Dtesting=TRUE -Dclover=FALSE \
  -DCMAKE_CXX_COMPILER=g++ -DCMAKE_CXX_FLAGS="-O3 -march=skylake-avx512 -Drestrict=__restrict__"
cmake --build build_avx512 --target time_dslash_noqdp -j

