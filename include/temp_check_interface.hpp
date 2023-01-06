#pragma once

#include <midf.hpp>

// temperature check service
MIDF_DECL_PORT(temp_check, 3660);

MIDF_DECL_FUNC_NO_ARGS(float, temp_check, get_temperature);

// test additional service
MIDF_DECL_PORT(test_service, 1234);

MIDF_DECL_FUNC(int, test_service, sum, int, int);

MIDF_DECL_FUNC_NO_ARGS(std::vector<char>, test_service, get_1mb);
