
#ifndef XRSFM_SRC_MAPPER_INCREMENTAL_MAPPER_H
#define XRSFM_SRC_MAPPER_INCREMENTAL_MAPPER_H

#include "base/map.h"
#include "geometry/pnp.h"
#include "geometry/track_processor.h"
#include "geometry/error_corrector.h"
#include "geometry/map_initializer.h"
#include "optimization/ba_solver.h"
#include "utility/timer.h"

namespace xrsfm {
struct IncrementalMapperOptions {
    bool correct_pose = false;
    bool only_with_sim3 = false;
    bool stop_when_register_fail = false;
    int init_id1 = -1;
    int init_id2 = -1;
    double th_rpe_lba = 16;
    double th_angle_lba = 1.5;
    double th_rpe_gba = 16;
    double th_angle_gba = 1.5;
};

class IncrementalMapper {
  public:
    IncrementalMapper();
    void Reconstruct(Map &map);
    TimerArray timer;
    BASolver ba_solver;
    Point3dProcessor p3d_processor;
    ErrorCorrector error_corrector;
    IncrementalMapperOptions options;
};
} // namespace xrsfm

#endif // XRSFM_SRC_MAPPER_INCREMENTAL_MAPPER_H