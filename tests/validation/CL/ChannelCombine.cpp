/*
 * Copyright (c) 2017-2018 ARM Limited.
 *
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include "arm_compute/core/Types.h"
#include "arm_compute/runtime/CL/CLMultiImage.h"
#include "arm_compute/runtime/CL/CLTensor.h"
#include "arm_compute/runtime/CL/CLTensorAllocator.h"
#include "arm_compute/runtime/CL/functions/CLChannelCombine.h"
#include "tests/CL/CLAccessor.h"
#include "tests/PaddingCalculator.h"
#include "tests/datasets/ConvertPolicyDataset.h"
#include "tests/datasets/ShapeDatasets.h"
#include "tests/framework/Asserts.h"
#include "tests/framework/Macros.h"
#include "tests/framework/datasets/Datasets.h"
#include "tests/validation/Validation.h"
#include "tests/validation/fixtures/ChannelCombineFixture.h"

namespace arm_compute
{
namespace test
{
namespace validation
{
namespace
{
inline void validate_configuration(const TensorShape &shape, Format format)
{
    const int num_planes = num_planes_from_format(format);

    // Create tensors
    CLMultiImage          dst     = create_multi_image<CLMultiImage>(shape, format);
    std::vector<CLTensor> ref_src = create_tensor_planes<CLTensor>(shape, format);

    // Create and configure function
    CLChannelCombine channel_combine;

    if(num_planes == 1)
    {
        const CLTensor *tensor_extra = ((Format::RGBA8888 == format) ? &ref_src[3] : nullptr);

        channel_combine.configure(&ref_src[0], &ref_src[1], &ref_src[2], tensor_extra, dst.cl_plane(0));
    }
    else
    {
        channel_combine.configure(&ref_src[0], &ref_src[1], &ref_src[2], &dst);
    }

}
} // namespace

TEST_SUITE(CL)
TEST_SUITE(ChannelCombine)

template <typename T>
using CLChannelCombineFixture = ChannelCombineValidationFixture<CLMultiImage, CLTensor, CLAccessor, CLChannelCombine, T>;

TEST_SUITE(Configuration)
DATA_TEST_CASE(RGBA, framework::DatasetMode::ALL, combine(concat(datasets::Small2DShapes(), datasets::Large2DShapes()), framework::dataset::make("FormatType", { Format::RGB888, Format::RGBA8888 })),
               shape, format)
{
    validate_configuration(shape, format);
}
DATA_TEST_CASE(YUV, framework::DatasetMode::ALL, combine(concat(datasets::Small2DShapes(), datasets::Large2DShapes()), framework::dataset::make("FormatType", { Format::YUYV422, Format::UYVY422 })),
               shape, format)
{
    validate_configuration(shape, format);
}

DATA_TEST_CASE(YUVPlanar, framework::DatasetMode::ALL, combine(concat(datasets::Small2DShapes(), datasets::Large2DShapes()), framework::dataset::make("FormatType", { Format::IYUV, Format::YUV444, Format::NV12, Format::NV21 })),
               shape, format)
{
    validate_configuration(shape, format);
}
TEST_SUITE_END()

TEST_SUITE(RGBA)
FIXTURE_DATA_TEST_CASE(RunSmall, CLChannelCombineFixture<uint8_t>, framework::DatasetMode::PRECOMMIT, combine(datasets::Small2DShapes(), framework::dataset::make("FormatType", { Format::RGB888, Format::RGBA8888 })))
{
    // Validate output
    for(unsigned int plane_idx = 0; plane_idx < _num_planes; ++plane_idx)
    {
        validate(CLAccessor(*_target.cl_plane(plane_idx)), _reference[plane_idx]);
    }
}
FIXTURE_DATA_TEST_CASE(RunLarge, CLChannelCombineFixture<uint8_t>, framework::DatasetMode::NIGHTLY, combine(datasets::Large2DShapes(), framework::dataset::make("FormatType", { Format::RGB888, Format::RGBA8888 })))
{
    // Validate output
    for(unsigned int plane_idx = 0; plane_idx < _num_planes; ++plane_idx)
    {
        validate(CLAccessor(*_target.cl_plane(plane_idx)), _reference[plane_idx]);
    }
}
TEST_SUITE_END()

TEST_SUITE(YUV)
FIXTURE_DATA_TEST_CASE(RunSmall, CLChannelCombineFixture<uint8_t>, framework::DatasetMode::PRECOMMIT, combine(datasets::Small2DShapes(), framework::dataset::make("FormatType", { Format::YUYV422, Format::UYVY422 })))
{
    // Validate output
    for(unsigned int plane_idx = 0; plane_idx < _num_planes; ++plane_idx)
    {
        validate(CLAccessor(*_target.cl_plane(plane_idx)), _reference[plane_idx]);
    }
}
FIXTURE_DATA_TEST_CASE(RunLarge, CLChannelCombineFixture<uint8_t>, framework::DatasetMode::NIGHTLY, combine(datasets::Large2DShapes(), framework::dataset::make("FormatType", { Format::YUYV422, Format::UYVY422 })))
{
    // Validate output
    for(unsigned int plane_idx = 0; plane_idx < _num_planes; ++plane_idx)
    {
        validate(CLAccessor(*_target.cl_plane(plane_idx)), _reference[plane_idx]);
    }
}
TEST_SUITE_END()

TEST_SUITE(YUVPlanar)
FIXTURE_DATA_TEST_CASE(RunSmall, CLChannelCombineFixture<uint8_t>, framework::DatasetMode::PRECOMMIT, combine(datasets::Small2DShapes(), framework::dataset::make("FormatType", { Format::NV12, Format::NV21, Format::IYUV, Format::YUV444 })))
{
    // Validate output
    for(unsigned int plane_idx = 0; plane_idx < _num_planes; ++plane_idx)
    {
        validate(CLAccessor(*_target.cl_plane(plane_idx)), _reference[plane_idx]);
    }
}
FIXTURE_DATA_TEST_CASE(RunLarge, CLChannelCombineFixture<uint8_t>, framework::DatasetMode::NIGHTLY, combine(datasets::Large2DShapes(), framework::dataset::make("FormatType", { Format::NV12, Format::NV21, Format::IYUV, Format::YUV444 })))
{
    // Validate output
    for(unsigned int plane_idx = 0; plane_idx < _num_planes; ++plane_idx)
    {
        validate(CLAccessor(*_target.cl_plane(plane_idx)), _reference[plane_idx]);
    }
}
TEST_SUITE_END()

TEST_SUITE_END()
TEST_SUITE_END()

} // namespace validation
} // namespace test
} // namespace arm_compute
