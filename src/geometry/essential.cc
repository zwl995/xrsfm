#include "essential.h"

#include <math.h>

#include "utility/random.h"

namespace xrsfm {
namespace {
    class Polynomial {
      public:
        // clang-format off
            enum GRevLexMonomials {
                XXX = 0, XXY = 1, XYY = 2, YYY = 3, XXZ = 4, XYZ = 5, YYZ = 6, XZZ = 7, YZZ = 8, ZZZ = 9,
                XX = 10, XY = 11, YY = 12, XZ = 13, YZ = 14, ZZ = 15, X = 16, Y = 17, Z = 18, I = 19
            };
        // clang-format on

        vector<20> v;

        Polynomial(const vector<20> &coeffcients) :
            v(coeffcients) {
        }

      public:
        Polynomial() :
            Polynomial(vector<20>::Zero()) {
        }

        Polynomial(double w) {
            v.setZero();
            v[I] = w;
        }

        void set_xyzw(double x, double y, double z, double w) {
            v.setZero();
            v[X] = x;
            v[Y] = y;
            v[Z] = z;
            v[I] = w;
        }

        Polynomial operator-() const {
            return Polynomial(-v);
        }

        Polynomial operator+(const Polynomial &b) const {
            return Polynomial(v + b.v);
        }

        Polynomial operator-(const Polynomial &b) const {
            return Polynomial(v - b.v);
        }

        Polynomial operator*(const Polynomial &b) const {
            Polynomial r;

            r.v[I] = v[I] * b.v[I];

            r.v[Z] = v[I] * b.v[Z] + v[Z] * b.v[I];
            r.v[Y] = v[I] * b.v[Y] + v[Y] * b.v[I];
            r.v[X] = v[I] * b.v[X] + v[X] * b.v[I];

            r.v[ZZ] = v[I] * b.v[ZZ] + v[Z] * b.v[Z] + v[ZZ] * b.v[I];
            r.v[YZ] = v[I] * b.v[YZ] + v[Z] * b.v[Y] + v[Y] * b.v[Z] + v[YZ] * b.v[I];
            r.v[XZ] = v[I] * b.v[XZ] + v[Z] * b.v[X] + v[X] * b.v[Z] + v[XZ] * b.v[I];
            r.v[YY] = v[I] * b.v[YY] + v[Y] * b.v[Y] + v[YY] * b.v[I];
            r.v[XY] = v[I] * b.v[XY] + v[Y] * b.v[X] + v[X] * b.v[Y] + v[XY] * b.v[I];
            r.v[XX] = v[I] * b.v[XX] + v[X] * b.v[X] + v[XX] * b.v[I];

            r.v[ZZZ] = v[I] * b.v[ZZZ] + v[Z] * b.v[ZZ] + v[ZZ] * b.v[Z] + v[ZZZ] * b.v[I];
            r.v[YZZ] = v[I] * b.v[YZZ] + v[Z] * b.v[YZ] + v[Y] * b.v[ZZ] + v[ZZ] * b.v[Y] + v[YZ] * b.v[Z] + v[YZZ] * b.v[I];
            r.v[XZZ] = v[I] * b.v[XZZ] + v[Z] * b.v[XZ] + v[X] * b.v[ZZ] + v[ZZ] * b.v[X] + v[XZ] * b.v[Z] + v[XZZ] * b.v[I];
            r.v[YYZ] = v[I] * b.v[YYZ] + v[Z] * b.v[YY] + v[Y] * b.v[YZ] + v[YZ] * b.v[Y] + v[YY] * b.v[Z] + v[YYZ] * b.v[I];
            r.v[XYZ] = v[I] * b.v[XYZ] + v[Z] * b.v[XY] + v[Y] * b.v[XZ] + v[X] * b.v[YZ] + v[YZ] * b.v[X] + v[XZ] * b.v[Y] + v[XY] * b.v[Z] + v[XYZ] * b.v[I];
            r.v[XXZ] = v[I] * b.v[XXZ] + v[Z] * b.v[XX] + v[X] * b.v[XZ] + v[XZ] * b.v[X] + v[XX] * b.v[Z] + v[XXZ] * b.v[I];
            r.v[YYY] = v[I] * b.v[YYY] + v[Y] * b.v[YY] + v[YY] * b.v[Y] + v[YYY] * b.v[I];
            r.v[XYY] = v[I] * b.v[XYY] + v[Y] * b.v[XY] + v[X] * b.v[YY] + v[YY] * b.v[X] + v[XY] * b.v[Y] + v[XYY] * b.v[I];
            r.v[XXY] = v[I] * b.v[XXY] + v[Y] * b.v[XX] + v[X] * b.v[XY] + v[XY] * b.v[X] + v[XX] * b.v[Y] + v[XXY] * b.v[I];
            r.v[XXX] = v[I] * b.v[XXX] + v[X] * b.v[XX] + v[XX] * b.v[X] + v[XXX] * b.v[I];

            return r;
        }

        const vector<20> &coeffcients() const {
            return v;
        }
    };

    Polynomial operator*(const double &scale, const Polynomial &poly) {
        return Polynomial(scale * poly.coeffcients());
    }

    inline matrix3 to_matrix(const vector<9> &vec) {
        return (matrix3() << vec.segment<3>(0), vec.segment<3>(3), vec.segment<3>(6)).finished();
    }

    inline matrix<9, 4> generate_nullspace_basis(const std::array<vector2, 5> &points1,
                                                 const std::array<vector2, 5> &points2) {
        matrix<5, 9> A;
        for (size_t i = 0; i < 5; ++i) {
            matrix3 h = vector3(points1[i].homogeneous()) * points2[i].homogeneous().transpose();
            for (size_t j = 0; j < 3; ++j) {
                A.block<1, 3>(i, j * 3) = h.row(j);
            }
        }
        return A.jacobiSvd(Eigen::ComputeFullV).matrixV().block<9, 4>(0, 5);
    }

    inline matrix<10, 20> generate_polynomials(const matrix<9, 4> &basis) {
        typedef matrix<3, 3, false, Polynomial> matrix_poly;
        matrix3 Ex = to_matrix(basis.col(0));
        matrix3 Ey = to_matrix(basis.col(1));
        matrix3 Ez = to_matrix(basis.col(2));
        matrix3 Ew = to_matrix(basis.col(3));

        matrix_poly Epoly;
        for (size_t i = 0; i < 3; ++i) {
            for (size_t j = 0; j < 3; ++j) {
                Epoly(i, j).set_xyzw(Ex(i, j), Ey(i, j), Ez(i, j), Ew(i, j));
            }
        }

        matrix<10, 20> polynomials;

        matrix_poly EEt = Epoly * Epoly.transpose();
        matrix_poly singular_value_constraints = (EEt * Epoly) - (0.5 * EEt.trace()) * Epoly;
        for (size_t i = 0; i < 3; ++i) {
            for (size_t j = 0; j < 3; ++j) {
                polynomials.row(i * 3 + j) = singular_value_constraints(i, j).coeffcients();
            }
        }

        Polynomial detE = Epoly.determinant();
        polynomials.row(9) = detE.coeffcients();

        return polynomials;
    }

    inline matrix<10> generate_action_matrix(matrix<10, 20> &polynomials) {
        std::array<size_t, 10> perm;
        for (size_t i = 0; i < 10; ++i) {
            perm[i] = i;
        }
        for (size_t i = 0; i < 10; ++i) {
            for (size_t j = i + 1; j < 10; ++j) {
                if (abs(polynomials(perm[i], i)) < abs(polynomials(perm[j], i))) {
                    std::swap(perm[i], perm[j]);
                }
            }
            if (polynomials(perm[i], i) == 0) continue;
            polynomials.row(perm[i]) /= polynomials(perm[i], i);
            for (size_t j = i + 1; j < 10; ++j) {
                polynomials.row(perm[j]) -= polynomials.row(perm[i]) * polynomials(perm[j], i);
            }
        }
        for (size_t i = 9; i > 0; --i) {
            for (size_t j = 0; j < i; ++j) {
                polynomials.row(perm[j]) -= polynomials.row(perm[i]) * polynomials(perm[j], i);
            }
        }

        matrix<10> action;
        action.row(0) = -polynomials.block<1, 10>(perm[Polynomial::XXX], Polynomial::XX);
        action.row(1) = -polynomials.block<1, 10>(perm[Polynomial::XXY], Polynomial::XX);
        action.row(2) = -polynomials.block<1, 10>(perm[Polynomial::XYY], Polynomial::XX);
        action.row(3) = -polynomials.block<1, 10>(perm[Polynomial::XXZ], Polynomial::XX);
        action.row(4) = -polynomials.block<1, 10>(perm[Polynomial::XYZ], Polynomial::XX);
        action.row(5) = -polynomials.block<1, 10>(perm[Polynomial::XZZ], Polynomial::XX);
        action.row(6) = vector<10>::Unit(Polynomial::XX - Polynomial::XX).transpose();
        action.row(7) = vector<10>::Unit(Polynomial::XY - Polynomial::XX).transpose();
        action.row(8) = vector<10>::Unit(Polynomial::XZ - Polynomial::XX).transpose();
        action.row(9) = vector<10>::Unit(Polynomial::X - Polynomial::XX).transpose();

        return action;
    }

    inline std::vector<vector3> solve_grobner_system(const matrix<10> &action) {
        Eigen::EigenSolver<matrix<10>> eigen(action, true);
        vector<10, std::complex<double>> xs = eigen.eigenvalues();

        std::vector<vector3> results;
        for (size_t i = 0; i < 10; ++i) {
            if (abs(xs[i].imag()) < 1.0e-10) {
                vector<10> h = eigen.eigenvectors().col(i).real();
                double xw = h(Polynomial::X - Polynomial::XX);
                double yw = h(Polynomial::Y - Polynomial::XX);
                double zw = h(Polynomial::Z - Polynomial::XX);
                double w = h(Polynomial::I - Polynomial::XX);
                results.emplace_back(xw / w, yw / w, zw / w);
            }
        }
        return results;
    }
} // namespace

void decompose_essential(const matrix3 &E, matrix3 &R1, matrix3 &R2, vector3 &T) {
#ifdef ESSENTIAL_DECOMPOSE_HORN
    matrix3 EET = E * E.transpose();
    double halfTrace = 0.5 * EET.trace();
    vector3 b;

    vector3 e0e1 = E.col(0).cross(E.col(1));
    vector3 e1e2 = E.col(1).cross(E.col(2));
    vector3 e2e0 = E.col(2).cross(E.col(0));

#if ESSENTIAL_DECOMPOSE_HORN == 1
    if (e0e1.norm() > e1e2.norm() && e0e1.norm() > e2e0.norm()) {
        b = e0e1.normalized() * sqrt(halfTrace);
    } else if (e1e2.norm() > e0e1.norm() && e1e2.norm() > e2e0.norm()) {
        b = e1e2.normalized() * sqrt(halfTrace);
    } else {
        b = e2e0.normalized() * sqrt(halfTrace);
    }
#else
    matrix3 bbT = halfTrace * matrix3::Identity() - EET;
    vector3 bbT_diag = bbT.diagonal();
    if (bbT_diag(0) > bbt_diag(1) && bbT_diag(0) > bbT_diag(2)) {
        b = bbT.row(0) / sqrt(bbT_diag(0));
    } else if (bbT_diag(1) > bbT_diag(0) && bbT_diag(1) > bbT_diag(2)) {
        b = bbT.row(1) / sqrt(bbT_diag(1));
    } else {
        b = bbT.row(2) / sqrt(bbT_diag(2));
    }
#endif

    matrix3 cofactorsT;
    cofactorsT.col(0) = e1e2;
    cofactorsT.col(1) = e2e0;
    cofactorsT.col(2) = e0e1;

    matrix3 skew_b;
    skew_b << 0, -b.z(), b.y(), b.z(), 0, -b.x(), -b.y(), b.x(), 0;
    matrix3 bxE = skew_b * E;

    double bTb = b.dot(b);
    R1 = (cofactorsT - bxE) / bTb;
    R2 = (cofactorsT + bxE) / bTb;
    T = b;
#else
    Eigen::JacobiSVD<matrix3> svd(E, Eigen::ComputeFullU | Eigen::ComputeFullV);
    matrix3 U = svd.matrixU();
    matrix3 VT = svd.matrixV().transpose();
    if (U.determinant() < 0) {
        U = -U;
    }
    if (VT.determinant() < 0) {
        VT = -VT;
    }
    matrix3 W;
    W << 0, 1, 0, -1, 0, 0, 0, 0, 1;
    R1 = U * W * VT;
    R2 = U * W.transpose() * VT;
    T = U.col(2);
#endif
}

double sampson_error(const matrix3 &E, const vector2 &p1, const vector2 &p2) {
    vector3 Ep1 = E * p1.homogeneous();
    vector3 Etp2 = E.transpose() * p2.homogeneous();
    double r = p2.homogeneous().transpose() * Ep1;
    return r * r * (1 / Ep1.segment<2>(0).squaredNorm() + 1 / Etp2.segment<2>(0).squaredNorm());
}

std::vector<matrix3> solve_essential_5pt(const std::array<vector2, 5> &points1,
                                         const std::array<vector2, 5> &points2) {
    matrix<9, 4> basis = generate_nullspace_basis(points1, points2);
    matrix<10, 20> polynomials = generate_polynomials(basis);
    matrix<10> action = generate_action_matrix(polynomials);
    std::vector<vector3> solutions = solve_grobner_system(action);
    std::vector<matrix3> results(solutions.size());
    for (size_t i = 0; i < solutions.size(); ++i) {
        results[i] = to_matrix(basis * solutions[i].homogeneous());
    }
    return results;
}

matrix3 find_essential_matrix(const std::vector<vector2> &points1, const std::vector<vector2> &points2,
                              double threshold, double confidence, size_t max_iteration, int seed) {
    // Diagnosis::TimingItem timing = Diagnosis::time(DI_F_ESSENTIAL_RANSAC_TIME);
#if 0
        ITSLAM_UNUSED_EXPR(max_iteration);
    std::vector<cv::Point2f> cvPoints1, cvPoints2;
    for (size_t i = 0; i < points1.size(); ++i) {
        cvPoints1.emplace_back(float(points1[i].x()), float(points1[i].y()));
        cvPoints2.emplace_back(float(points2[i].x()), float(points2[i].y()));
    }
    cv::Mat cvE = cv::findEssentialMat(cvPoints1, cvPoints2, cv::Mat::eye(3, 3, CV_32FC1), cv::RANSAC, confidence, threshold, cv::noArray());
    // workaround: fuck opencv
    if (cvE.rows != 3 || cvE.cols != 3) {
        return matrix3::Identity() * std::numeric_limits<double>::quiet_NaN();
    }
    matrix3 E;
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            E(i, j) = cvE.at<double>(i, j);
        }
    }
    return E;
#else
    LotBox lotbox(points1.size());
    lotbox.seed(seed);
    double K = log(1 - confidence);
    double threshold_sq = 2 * threshold * threshold;

    matrix3 best_E = matrix3::Constant(std::numeric_limits<double>::quiet_NaN());
    size_t best_inlier = 0;
    double best_score = std::numeric_limits<double>::max();

    size_t iter_max = max_iteration;
    for (size_t iter = 0; iter < iter_max; ++iter) {
        // generate hypothesis
        lotbox.refill_all();
        std::array<vector2, 5> pts1, pts2;
        for (size_t i = 0; i < 5; ++i) {
            size_t sample_id = lotbox.draw_without_replacement();
            pts1[i] = points1[sample_id];
            pts2[i] = points2[sample_id];
        }

        // solve essential matrix
        std::vector<matrix3> Es = solve_essential_5pt(pts1, pts2);

        // find best hypothesis
        for (size_t i = 0; i < Es.size(); ++i) {
            const matrix3 &E = Es[i];
            size_t inlier = 0;
            double score = 0.0;
            for (size_t j = 0; j < points1.size(); ++j) {
                double se = sampson_error(E, points1[j], points2[j]);
                score += log(1.0 + se * se / threshold_sq);
                if (se < threshold_sq) {
                    inlier++;
                }
            }
            // update current best model and iteration limit
            if (inlier > best_inlier || (inlier == best_inlier && best_inlier > 0 && score < best_score)) {
                best_E = E;
                best_inlier = inlier;
                best_score = score;

                double inlier_ratio = best_inlier / (double)points1.size();
                double N = K / log(1 - pow(inlier_ratio, 5));
                if (N < (double)iter_max) { // guard for NaN
                    iter_max = (size_t)ceil(N);
                }
            }
        }
    }
    // printf("iter_num: %d inlier_num: %d\n", iter_max, best_inlier);
    return best_E;
#endif
}

void solve_essential(const std::vector<vector2> &points1, const std::vector<vector2> &points2,
                     const double threshold, matrix3 &E, int &inlier_num, std::vector<char> &inlier_mask) {
    E = find_essential_matrix(points1, points2, threshold, 0.9, 50, 0); // slow
    inlier_num = 0;
    inlier_mask.assign(points1.size(), 0);
    double threshold_sq = 2 * threshold * threshold;
    for (size_t i = 0; i < points1.size(); ++i) {
        double se = sampson_error(E, points1[i], points2[i]);
        if (se < threshold_sq) {
            inlier_num++;
            inlier_mask[i] = 1;
        }
    }
}

vector<4> triangulate_point(const matrix<3, 4> P1, const matrix<3, 4> P2, const vector2 point1,
                            const vector2 point2) {
    matrix<4> A;
    A.row(0) = point1(0) * P1.row(2) - P1.row(0);
    A.row(1) = point1(1) * P1.row(2) - P1.row(1);
    A.row(2) = point2(0) * P2.row(2) - P2.row(0);
    A.row(3) = point2(1) * P2.row(2) - P2.row(1);
    vector<4> q = A.jacobiSvd(Eigen::ComputeFullV).matrixV().col(3);
    return q;
}

bool check_point(const matrix<3, 4> P1, const matrix<3, 4> P2, const vector2 point1, const vector2 point2,
                 vector3 &p) {
    vector<4> q = triangulate_point(P1, P2, point1, point2);

    vector3 q1 = P1 * q;
    vector3 q2 = P2 * q;

    if (q1[2] * q[3] > 0 && q2[2] * q[3] > 0 && q1[2] / q[3] < 100 && q2[2] / q[3] < 100) {
        p = q.hnormalized();
        return true;
    }
    return false;
}

int check_essential_rt(const std::vector<vector2> &points1, const std::vector<vector2> &points2, const matrix3 R,
                       const vector3 T, std::vector<vector3> &result_points, std::vector<char> &result_status) {
    // set P1 P2
    matrix<3, 4> P1, P2;
    P1.setIdentity();
    P2 << R, T;
    // triangulate
    result_points.resize(points1.size());
    result_status.resize(points1.size());

    int count = 0;
    for (size_t i = 0; i < points1.size(); ++i) {
        vector3 p;
        if (check_point(P1, P2, points1[i], points2[i], result_points[i])) {
            result_status[i] = 1;
            count++;
        } else {
            result_status[i] = 0;
        }
    }

    // printf("%d %d\n", count, points1.size());

    return count;
}

void decompose_rt(const matrix3 &E, const std::vector<vector2> &points1,
                  const std::vector<vector2> &points2, matrix3 &R, vector3 &T,
                  std::vector<vector3> &result_points, std::vector<char> &result_status) {
    // decompose RT
    matrix3 R1, R2;
    vector3 T1;
    decompose_essential(E, R1, R2, T1);
    matrix3 Rs[] = {R1, R1, R2, R2};
    vector3 Ts[] = {T1, -T1, T1, -T1};
    // get good R T
    std::array<std::vector<vector3>, 4> points_array;
    std::array<std::vector<char>, 4> status_array;
    int best_num = 0;
    int best_k = 0;
    for (int k = 0; k < 4; ++k) {
        int goods = check_essential_rt(points1, points2, Rs[k], Ts[k], points_array[k], status_array[k]);
        if (goods > best_num) {
            best_num = goods;
            best_k = k;
        }
    }
    R = Rs[best_k];
    T = Ts[best_k];
    result_points = points_array[best_k];
    result_status = status_array[best_k];
}
} // namespace xrsfm
