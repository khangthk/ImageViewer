/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkSampleProject.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

Copyright (c) 2001 Insight Consortium
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

 * Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

 * Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

 * The name of the Insight Consortium, nor the names of any consortium members,
   nor of any contributors, may be used to endorse or promote products derived
   from this software without specific prior written permission.

  * Modified source versions must be plainly marked as such, and must not be
    misrepresented as being the original software.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER AND CONTRIBUTORS ``AS IS''
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

=========================================================================*/

// This is a sample project that can be built outside of the Insight source
// tree.  You can use this project as a model for how to build a project that
// "uses" Insight (instead of a project that is within Insight).
//
// To build this project on Windows:
//     1) Copy the contents of this directory somewhere outside the Insight
//             source tree
//     2) Run a copy of CMakeSetup
//     3) Locate this project's source tree and build tree
//     4) Build the project files
//     5) Edit the cache value for ITK_BINARY_DIR (default is NOTFOUND)
//     6) Load SampleProject.dsw into MSDEV and build
//
// To build this project on Unix:
//     1) Copy the contents of this directory somewhere outside the Insight
//             source tree
//     2) cd /path/to/SampleProject
//     3) Run cmake on SampleProject
//            > /path/to/cmake .
//     4) Edit the cache values for ITK_BINARY_DIR (default is NOTFOUND)
//     5) Build the project using make
//
//


#include "itkImage.h"
#include "itkImageRegionIterator.h"

#include <iostream>

int main()
{
  //Allocate Images
  typedef float PixelType;
  typedef itk::Image<PixelType,2>      ImageType;
  enum { ImageDimension = ImageType::ImageDimension };

  ImageType::SizeType size = {{100,100}};
  ImageType::IndexType index = {{0,0}};
  ImageType::RegionType region;
  region.SetSize( size );
  region.SetIndex( index );

  ImageType::Pointer img = ImageType::New();
  img->SetLargestPossibleRegion( region );
  img->SetBufferedRegion( region );
  img->SetRequestedRegion( region );
  img->Allocate();

  // Fill images with a 2D gaussian
  typedef  itk::ImageRegionIterator<ImageType> IteratorType;

  itk::Point<double,2> center;
  center[0] = (double)region.GetSize()[0]/2.0;
  center[1] = (double)region.GetSize()[1]/2.0;

  const double s = (double)region.GetSize()[0]/2.0;

  itk::Point<double,2>  p;
  itk::Vector<double,2> d;

  // Set the displacement
  itk::Vector<double,2> displacement;
  displacement[0] = 7;
  displacement[1] =	3;

  IteratorType ri(img,region);
  while(!ri.IsAtEnd())
  {
    p[0] = ri.GetIndex()[0];
    p[1] = ri.GetIndex()[1];
    d = p-center;
    d += displacement;
    const double x = d[0];
    const double y = d[1];
    ri.Set( (PixelType)( 200.0 * exp( - ( x*x + y*y )/(s*s) ) ) );
    ++ri;
  }

  if( 0 )
    {
    std::cout << "Test failed." << std::endl;
    return EXIT_FAILURE;
    }

  std::cout << "Test passed." << std::endl;
  return EXIT_SUCCESS;


  return 0;
}
