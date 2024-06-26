/*
 * (c)2017-2023, Cris Luengo.
 * Based on original DIPlib code: (c)1995-2014, Delft University of Technology.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "diplib/linear.h"

#include <utility>

#include "diplib.h"
#include "diplib/math.h"

namespace dip {

void Sharpen(
      Image const& in,
      Image& out,
      dfloat weight,
      FloatArray sigmas,
      String const& method,
      StringArray const& boundaryCondition,
      dfloat truncation
) {
   Image laplace = Laplace( in, std::move( sigmas ), method, boundaryCondition, {}, truncation );
   LinearCombination( in, laplace, out, 1.0, -weight );
}

void UnsharpMask(
      Image const& in,
      Image& out,
      dfloat weight,
      FloatArray sigmas,
      String const& method,
      StringArray const& boundaryCondition,
      dfloat truncation
) {
   Image unsharp = Gauss( in, std::move( sigmas ), { 0 }, method, boundaryCondition, truncation );
   LinearCombination( in, unsharp, out, 1.0 + weight, -weight );
}

}
