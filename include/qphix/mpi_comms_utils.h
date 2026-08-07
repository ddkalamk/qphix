#pragma once

#include "qphix/qphix_config.h"

#ifdef QPHIX_MPI_COMMS

#include <mpi.h>
#include <cstdio>
#include <cstdlib>

/*!
 * Native MPI communications backend for QPhiX.
 *
 * This provides a minimal, QMP-free process-grid abstraction built directly on
 * top of MPI. It sets up a 4D periodic Cartesian communicator and exposes the
 * small amount of topology information (logical dimensions, coordinates, rank
 * and neighbour lookup) that the QPhiX `Comms` class needs. It is used when the
 * library is built with `-Dparallel_arch=parscalar` and `-Dmpi_comms=ON`
 * without QMP or QDP++.
 */

namespace QPhiX
{
namespace MPIComms
{

struct State {
  MPI_Comm cart = MPI_COMM_NULL;
  int dims[4] = {1, 1, 1, 1};
  int coords[4] = {0, 0, 0, 0};
  int rank = 0;
  int nranks = 1;
  bool initialized = false;
};

/*! Process-wide state, using a function-local static to remain header-only. */
inline State &state()
{
  static State s;
  return s;
}

inline void abort(const char *msg)
{
  std::fprintf(stderr, "QPhiX MPI Comms error: %s\n", msg);
  MPI_Abort(MPI_COMM_WORLD, 1);
}

/*! Declare a 4D periodic Cartesian topology matching `geom` (x, y, z, t). */
inline void declareTopology(const int geom[4])
{
  State &s = state();
  for (int d = 0; d < 4; ++d) {
    s.dims[d] = geom[d];
  }
  int periods[4] = {1, 1, 1, 1};
  // reorder = 0 keeps a predictable rank <-> coordinate mapping.
  if (MPI_Cart_create(MPI_COMM_WORLD, 4, s.dims, periods, 0, &s.cart) !=
      MPI_SUCCESS) {
    abort("MPI_Cart_create failed (does the process count match the geometry?)");
  }
  MPI_Comm_rank(s.cart, &s.rank);
  MPI_Comm_size(s.cart, &s.nranks);
  MPI_Cart_coords(s.cart, s.rank, 4, s.coords);
  s.initialized = true;
}

inline void freeTopology()
{
  State &s = state();
  if (s.cart != MPI_COMM_NULL) {
    MPI_Comm_free(&s.cart);
    s.cart = MPI_COMM_NULL;
  }
  s.initialized = false;
}

inline MPI_Comm comm() { return state().cart; }
inline const int *logicalDims() { return state().dims; }
inline const int *logicalCoords() { return state().coords; }
inline int nodeNumber() { return state().rank; }
inline int numNodes() { return state().nranks; }
inline bool isPrimary() { return state().rank == 0; }

/*! Rank of the process at the given logical coordinates. */
inline int nodeNumberFrom(const int coords[4])
{
  int r = 0;
  int c[4] = {coords[0], coords[1], coords[2], coords[3]};
  if (MPI_Cart_rank(state().cart, c, &r) != MPI_SUCCESS) {
    abort("MPI_Cart_rank failed");
  }
  return r;
}

} // namespace MPIComms
} // namespace QPhiX

#endif // QPHIX_MPI_COMMS
