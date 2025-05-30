%CURLVECTOR   Curl of a vector field
%
% SYNOPSIS:
%  image_out = curlvector(image_in,sigma,method,boundary_condition,dim,truncation)
%
%  IMAGE_IN is a M-by-1 tensor image with N dimensions. M must be 2 or 3.
%  IMAGE_OUT is a M-by-1 tensor image.
%
%  DIM determines along which dimensions to apply the operation.
%  It must be an array of integers in the range 1 to NDIMS(IMAGE_IN).
%  The empty array indicates that all dimensions should be processed.
%  If IMAGE_IN has N dimensions but only M tensor elements, and DIM
%  has M unique elements, then the operation is correctly defined as
%  M-dimensional curl.
%
%  See DERIVATIVE for a description of the parameters and the defaults.
%
%  This function is identical to DIP_IMAGE/CURL if IMAGE_IN is
%  a DIP_IMAGE object.
%
% DIPlib:
%  This function calls the DIPlib function <a href="https://diplib.org/diplib-docs/linear.html#dip-Curl-Image-CL-Image-L-FloatArray--String-CL-StringArray-CL-BooleanArray--dfloat-">dip::Curl</a>.

% (c)2017-2018, Cris Luengo.
% Based on original DIPlib code: (c)1995-2014, Delft University of Technology.
% Based on original DIPimage code: (c)1999-2014, Delft University of Technology.
%
% Licensed under the Apache License, Version 2.0 (the "License");
% you may not use this file except in compliance with the License.
% You may obtain a copy of the License at
%
%    http://www.apache.org/licenses/LICENSE-2.0
%
% Unless required by applicable law or agreed to in writing, software
% distributed under the License is distributed on an "AS IS" BASIS,
% WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
% See the License for the specific language governing permissions and
% limitations under the License.

function out = curlvector(varargin)
out = dip_filtering('curl',varargin{:});
