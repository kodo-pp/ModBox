#include <tuple>

#include <irrlicht_wrapper.hpp>

using irr::core::vector3d;

template <typename T>
std::tuple<T, T, T> irrvec_extract(const vector3d<T>& vec)
{
    return {vec.X, vec.Y, vec.Z};
}
