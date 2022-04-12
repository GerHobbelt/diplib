/*
 * (c)2016-2022, Cris Luengo.
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


namespace dip {
namespace Feature {


class FeatureGreyDimensionsEllipsoid : public Composite {
   public:
      FeatureGreyDimensionsEllipsoid() : Composite( { "GreyDimensionsEllipsoid", "Extent along the principal axes of an ellipsoid (grey-weighted, 2D & 3D)", true } ) {};

      virtual ValueInformationArray Initialize( Image const& label, Image const& grey, dip::uint /*nObjects*/ ) override {
         DIP_THROW_IF( !grey.IsScalar(), E::IMAGE_NOT_SCALAR );
         nD_ = label.Dimensionality();
         DIP_THROW_IF(( nD_ < 2 ) || ( nD_ > 3 ), E::DIMENSIONALITY_NOT_SUPPORTED );
         ValueInformationArray out( nD_ );
         Units units;
         std::tie( units, scales_ ) = MuEigenDecompositionUnitsAndScaling( nD_, label.PixelSize() );
         for( dip::uint ii = 0; ii < nD_; ++ii ) {
            out[ ii ].units = units;
            out[ ii ].name = String( "axis" ) + std::to_string( ii );
         }
         hasIndex_ = false;
         return out;
      }

      virtual StringArray Dependencies() override {
         StringArray out( 1 );
         out[ 0 ] = "GreyMu";
         return out;
      }

      virtual void Compose( Measurement::IteratorObject& dependencies, Measurement::ValueIterator output ) override {
         auto it = dependencies.FirstFeature();
         if( !hasIndex_ ) {
            muIndex_ = dependencies.ValueIndex( "GreyMu" );
            hasIndex_ = true;
         }
         dfloat data[ 6 ]; // We never have more than 6 components.
         dfloat const* input = &it[ muIndex_ ];
         for( dip::uint ii = 0; ii < scales_.size(); ++ii ) {
            data[ ii ] = input[ ii ] * scales_[ ii ];
         }
         dfloat eig[ 3 ]; // We never have more than 3 eigenvectors.
         SymmetricEigenDecompositionPacked( nD_, data, eig );
         if( nD_ == 2 ) {
            output[ 0 ] = std::sqrt( 16.0 * eig[ 0 ] );
            output[ 1 ] = std::sqrt( 16.0 * eig[ 1 ] );
         } else { // nD_ == 3
            output[ 0 ] = std::sqrt( 10.0 * (   eig[ 0 ] + eig[ 1 ] - eig[ 2 ] ));
            output[ 1 ] = std::sqrt( 10.0 * (   eig[ 0 ] - eig[ 1 ] + eig[ 2 ] ));
            output[ 2 ] = std::sqrt( 10.0 * ( - eig[ 0 ] + eig[ 1 ] + eig[ 2 ] ));
         }
      }

      virtual void Cleanup() override {
         scales_.clear();
      }

   private:
      FloatArray scales_;
      dip::uint muIndex_;
      bool hasIndex_;
      dip::uint nD_;
};


} // namespace feature
} // namespace dip
