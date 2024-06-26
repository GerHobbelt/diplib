/*
 * (c)2016-2024, Cris Luengo.
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

#include "diplib/framework.h"

#include <cstdlib>
#include <vector>

#include "diplib.h"

#include "framework_support.h"

namespace dip {
namespace Framework {

// Part of the next two functions
void SingletonExpandedSize(
      UnsignedArray& size,
      UnsignedArray const& size2
) {
   if( size.size() < size2.size() ) {
      size.resize( size2.size(), 1 );
   }
   for( dip::uint jj = 0; jj < size2.size(); ++jj ) {
      if( size[ jj ] != size2[ jj ] ) {
         if( size[ jj ] == 1 ) {
            size[ jj ] = size2[ jj ];
         } else if( size2[ jj ] != 1 ) {
            DIP_THROW( E::SIZES_DONT_MATCH );
         }
      }
   }
}

// Figure out what the size of the images must be.
UnsignedArray SingletonExpandedSize(
      ImageConstRefArray const& in
) {
   UnsignedArray size = in[ 0 ].get().Sizes();
   for( dip::uint ii = 1; ii < in.size(); ++ii ) {
      UnsignedArray size2 = in[ ii ].get().Sizes();
      SingletonExpandedSize( size, size2 );
   }
   return size;
}

// Idem as above.
UnsignedArray SingletonExpandedSize(
      ImageArray const& in
) {
   UnsignedArray size = in[ 0 ].Sizes();
   for( dip::uint ii = 1; ii < in.size(); ++ii ) {
      UnsignedArray size2 = in[ ii ].Sizes();
      SingletonExpandedSize( size, size2 );
   }
   return size;
}

dip::uint SingletonExpendedTensorElements(
      ImageArray const& in
) {
   dip::uint tsize = in[ 0 ].TensorElements();
   for( dip::uint ii = 1; ii < in.size(); ++ii ) {
      dip::uint tsize2 = in[ ii ].TensorElements();
      if( tsize != tsize2 ) {
         if( tsize == 1 ) {
            tsize = tsize2;
         } else if( tsize2 != 1 ) {
            DIP_THROW( E::SIZES_DONT_MATCH );
         }
      }
   }
   return tsize;
}


namespace {

dip::uint OptimalProcessingDim_internal(
      UnsignedArray const& sizes,
      IntegerArray const& strides
) {
   constexpr dip::uint SMALL_IMAGE = 63;  // A good value would depend on the size of cache.
   dip::uint processingDim = 0;
   for( dip::uint ii = 1; ii < strides.size(); ++ii ) {
      // The processing dimension should not have a stride of 0
      if( strides[ ii ] != 0 ) {
         if(   // the current processing dimension is 0
               ( strides[ processingDim ] == 0 ) ||
               // or this is not a small dimension and the stride is smaller than the current processing dimension
               (( sizes[ ii ] > SMALL_IMAGE ) && ( std::abs( strides[ ii ] ) < std::abs( strides[ processingDim ] ))) ||
               // or the current processing dimension is a small dimension, and this dimension is larger
               (( sizes[ processingDim ] <= SMALL_IMAGE ) && ( sizes[ ii ] > sizes[ processingDim ] ))) {
            // then update the processing dimension to this one
            processingDim = ii;
               }
      }
   }
   return processingDim;
}

} // namespace

// Find best processing dimension, which is the one with the smallest stride,
// except if that dimension is very small and there's a longer dimension.
dip::uint OptimalProcessingDim(
      Image const& in
) {
   DIP_THROW_IF( !in.IsForged(), E::IMAGE_NOT_FORGED );
   return OptimalProcessingDim_internal( in.Sizes(), in.Strides() );
}

// Find the best processing dimension as above, but giving preference to a dimension
// where `kernelSizes` is large also.
dip::uint OptimalProcessingDim(
      Image const& in,
      UnsignedArray const& kernelSizes
) {
   DIP_THROW_IF( !in.IsForged(), E::IMAGE_NOT_FORGED );
   UnsignedArray sizes = in.Sizes();
   DIP_THROW_IF( sizes.size() != kernelSizes.size(), E::ARRAY_PARAMETER_WRONG_LENGTH );
   for( dip::uint ii = 0; ii < sizes.size(); ++ii ) {
      if( kernelSizes[ ii ] == 1 ) {
         sizes[ ii ] = 1; // this will surely force the algorithm to not return this dimension as optimal processing dimension
      }
   }
   // TODO: a kernel of 1000x2 should definitely return the dimension where it's 1000 as the optimal dimension. Or?
   return OptimalProcessingDim_internal( sizes, in.Strides() );
}

std::vector< UnsignedArray >  SplitImageEvenlyForProcessing(
   UnsignedArray const& sizes,
   dip::uint nBlocks,
   dip::uint nPixelsPerBlock,
   dip::uint processingDim // set to sizes.size() or larger if there's none
) {
   // DIP_ASSERT( nBlocks * nPixelsPerBlock >= sizes.product() ); // except we shouldn't count the processing dimension here!
   std::vector< UnsignedArray > startCoords( nBlocks );
   dip::uint nDims = sizes.size();
   startCoords[ 0 ] = UnsignedArray( nDims, 0 );
   for( dip::uint ii = 1; ii < nBlocks; ++ii ) {
      startCoords[ ii ] = startCoords[ ii - 1 ];
      // To advance the iterator nLinesPerThread times, we increment it in whole-line steps.
      dip::uint firstDim = processingDim == 0 ? 1 : 0;
      dip::uint remaining = nPixelsPerBlock;
      do {
         for( dip::uint dd = 0; dd < nDims; ++dd ) {
            if( dd == firstDim ) {
               dip::uint n = sizes[ dd ] - startCoords[ ii ][ dd ];
               if( remaining >= n ) {
                  // Rewinding, next loop iteration will increment the next coordinate
                  remaining -= n;
                  startCoords[ ii ][ dd ] = 0;
               } else {
                  // Forward by `remaining`, then we're done.
                  startCoords[ ii ][ dd ] += remaining;
                  remaining = 0;
                  break;
               }
            } else if( dd != processingDim ) {
               // Increment coordinate
               ++startCoords[ ii ][ dd ];
               // Check whether we reached the last pixel of the line
               if( startCoords[ ii ][ dd ] < sizes[ dd ] ) {
                  break;
               }
               // Rewind, the next loop iteration will increment the next coordinate
               startCoords[ ii ][ dd ] = 0;
            }
         }
      } while( remaining > 0 );
   }
   return startCoords;
}

} // namespace Framework
} // namespace dip
