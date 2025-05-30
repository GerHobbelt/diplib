%DT   Euclidean distance transform
%
% SYNOPSIS:
%  image_out = dt(image_in,edgeCondition,method)
%
% PARAMETERS:
%  edgeCondition: the value of pixels outside the image bounds, can be
%      'background' or 'object', or equivalently 0 or 1.
%  method: 'separable', 'fast', 'ties', 'true', 'brute force', or 'square'
%
% DEFAULTS:
%  edgeCondition = 'object'
%  method = 'separable'
%
% DIPlib:
%  This function calls the DIPlib function <a href="https://diplib.org/diplib-docs/distance.html#dip-EuclideanDistanceTransform-Image-CL-Image-L-String-CL-String-CL">dip::EuclideanDistanceTransform</a>

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

function out = dt(varargin)
out = dip_analysis('dt',varargin{:});
