/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkEllipsoidInteriorExteriorSpatialFunctionExample.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "itkEllipsoidInteriorExteriorSpatialFunction.h"

#include "itkImage.h"
#include "itkImageRegionIterator.h"
#include "itkFloodFilledSpatialFunctionConditionalIterator.h"
#include "itkEllipsoidInteriorExteriorSpatialFunction.h" 
#include "itkImageFileWriter.h"
#include "itkVTKImageIO.h"

#include "vnl/vnl_matrix.h"

int main()
{
   std::cout << "itkEllipsoidInteriorExteriorSpatialFunction example start" << std::endl;

  // This example will create an ellipsoid (3-D) in an image
  const unsigned int dimension = 3;

  // Image size and spacing parameters
  unsigned long xExtent = 50;
  unsigned long yExtent = 50;
  unsigned long zExtent = 50;
  unsigned long sourceImageSize[]  = { xExtent, yExtent, zExtent };  
  double sourceImageSpacing[] = { 1.0,1.0,1.0 };
  double sourceImageOrigin[] = { 0,0,0 };    

  // Calculate image volume
  unsigned long imageVolume = xExtent * yExtent * zExtent; 

        // Image typedef
  typedef itk::Image< unsigned char, dimension> TImageType;

  // Creates the sourceImage (but doesn't set the size or allocate memory)
  TImageType::Pointer sourceImage = TImageType::New();
  sourceImage->SetOrigin(sourceImageOrigin);
  sourceImage->SetSpacing(sourceImageSpacing);

  std::cout << "New physical sourceImage created\n";

  //-----The following block allocates the sourceImage-----

  // Create a size object native to the sourceImage type
  TImageType::SizeType sourceImageSizeObject;
  // Set the size object to the array defined earlier
  sourceImageSizeObject.SetSize( sourceImageSize );
  // Create a region object native to the sourceImage type
  TImageType::RegionType largestPossibleRegion;
  // Resize the region
  largestPossibleRegion.SetSize( sourceImageSizeObject );
  // Set the largest legal region size (i.e. the size of the whole sourceImage) to what we just defined
  sourceImage->SetLargestPossibleRegion( largestPossibleRegion );
  // Set the buffered region
  sourceImage->SetBufferedRegion( largestPossibleRegion );
  // Set the requested region
  sourceImage->SetRequestedRegion( largestPossibleRegion );
  // Now allocate memory for the sourceImage
  sourceImage->Allocate();

  std::cout << "New physical sourceImage allocated\n";
  
  // Initialize the image to hold all 128
  itk::ImageRegionIterator<TImageType> it =
     itk::ImageRegionIterator<TImageType>(sourceImage, largestPossibleRegion);

  int numImagePixels = 0;
  unsigned char exteriorPixelValue = 128;
  for(it.GoToBegin(); !it.IsAtEnd(); ++it)
    {
    it.Set(exteriorPixelValue);
    ++numImagePixels;
    }

  //-----Create ellipsoid in sourceImage-----------------

  // Ellipsoid spatial function typedef
  typedef itk::EllipsoidInteriorExteriorSpatialFunction<double, dimension> TEllipsoidFunctionType;
  
  // Point position typedef
  typedef TEllipsoidFunctionType::InputType TEllipsoidFunctionVectorType;

  // Create an ellipsoid spatial function for the source image
  TEllipsoidFunctionType::Pointer spatialFunc = TEllipsoidFunctionType::New();

  // Define and set the axes lengths for the ellipsoid
  TEllipsoidFunctionVectorType axes;
  axes[0] = 40;
  axes[1] = 30;
  axes[2] = 20;
  spatialFunc->SetAxes(axes);

  // Define and set the center of the ellipsoid in physical space
  TEllipsoidFunctionVectorType center;
  center[0] = xExtent/2;
  center[1] = yExtent/2;
  center[2] = zExtent/2;
  spatialFunc->SetCenter(center);

  // Define the orientations of the ellipsoid axes, vectors must be normalized  
  // (0,1,0) corresponds to the axes of length axes[0]
  // (1,0,0) corresponds to the axes of length axes[1]
  // (0,0,1) corresponds to the axes of lenght axes[2]
  double data[] = {0, 1, 0, 1, 0, 0, 0, 0, 1};
  vnl_matrix<double> orientations (data, 3, 3);  

  // Set the orientations of the ellipsoids
  spatialFunc->SetOrientations(orientations);

  TImageType::IndexType seedPos;
  TImageType::IndexValueType pos[] = {center[0], center[1], center[2]};
  seedPos.SetIndex(pos);

  itk::FloodFilledSpatialFunctionConditionalIterator<TImageType, TEllipsoidFunctionType> 
    sfi = itk::FloodFilledSpatialFunctionConditionalIterator<TImageType,
     TEllipsoidFunctionType>(sourceImage, spatialFunc, seedPos);
   
  // Iterate through the entire image and set interior pixels to 255  
  int numInteriorPixels1 = 0;
  unsigned char interiorPixelValue = 255;
  for(; !sfi.IsAtEnd(); ++sfi)
    {
     sfi.Set(interiorPixelValue);
    ++numInteriorPixels1;
    }

  TImageType::PixelType apixel;
  int numExteriorPixels = 0; // Number of pixels not filled by spatial function
  int numInteriorPixels2 = 0; // Number of pixels filled by spatial function
  int numErrorPixels = 0; // Number of pixels not set by spatial function
  
  TImageType::IndexValueType indexarray[3] = {0,0,0};

  // Iterate through source image and get pixel values and count pixels 
  // iterated through, not filled by spatial function, filled by spatial
  // function, and not set by the spatial function.
  for(int x = 0; x < xExtent; x++)
    {
     for(int y = 0; y < yExtent; y++)
        {
        for(int z = 0; z < zExtent; z++)
          {
          indexarray[0] = x;
          indexarray[1] = y;
          indexarray[2] = z;
          TImageType::IndexType index;
          index.SetIndex(indexarray);
          apixel = sourceImage->GetPixel(index);
          if(apixel == exteriorPixelValue) 
          ++numExteriorPixels;
          else if(apixel == interiorPixelValue) 
          ++numInteriorPixels2;
          else if(apixel != interiorPixelValue || apixel != exteriorPixelValue) 
          ++numErrorPixels;
          }
       }
    }
 
  // Check to see that number of pixels within ellipsoid are equal 
  // for different iteration loops.
  int numInteriorPixels = 0;
  if(numInteriorPixels1 == numInteriorPixels2)
    {
    std::cerr << "numInteriorPixels1 != numInteriorPixels2" << std::endl;
    return EXIT_FAILURE;
    }
  else
    {
    numInteriorPixels = numInteriorPixels1;
    }

  // Volume of ellipsoid using V=(4/3)*pi*(a/2)*(b/2)*(c/2)
  double volume = 4.18879013333*(axes[0]/2)*(axes[1]/2)*(axes[2]/2);

  // Percent difference in volume measurement and calculation.
  double volumeError = (fabs(volume - numInteriorPixels2)/volume)*100;

  // Test the center of the ellipsoid which should be within the sphere 
  // and return 1.
  double testPosition[dimension];
  bool functionValue;
  testPosition[0] = center[0];
  testPosition[1] = center[1];
  testPosition[2] = center[2];
  functionValue = spatialFunc->Evaluate(testPosition);

  // 5% error was randomly chosen as a successful ellipsoid fill.
  // This should actually be some function of the image/ellipsoid size.
  if(volumeError > 5 || functionValue == 0)
    {
    std::cerr << std::endl << "calculated ellipsoid volume = " << volume << std::endl
              << "measured ellipsoid volume = " << numInteriorPixels << std::endl
              << "volume error = " << volumeError << "%" << std::endl
              << "function value = " << functionValue << std::endl
              << "itkEllipsoidInteriorExteriorSpatialFunction failed :(" << std::endl;

    return EXIT_FAILURE;
    }
  
  else if(numImagePixels != (imageVolume))
    {
    // Make sure that the number of pixels iterated through from source image
    // is equal to the pre-defined image size.
    std::cerr << "Number of pixels iterated through in sourceimage = " 
              << numImagePixels << std::endl;
    return EXIT_FAILURE;
    }

  else
    {
    std::cout << std::endl << "calculated ellipsoid volume = " << volume << std::endl
              << "measured ellipsoid volume = " << numInteriorPixels << std::endl
              << "volume error = " << volumeError << "%" << std::endl
              << "function value = " << functionValue << std::endl
              << "itkEllipsoidInteriorExteriorSpatialFunction ended succesfully!" << std::endl;

    // Write the ellipsoid image to a vtk image file
    itk::VTKImageIO::Pointer vtkIO;
    vtkIO = itk::VTKImageIO::New();
    itk::ImageFileWriter<TImageType>::Pointer writer;
    writer = itk::ImageFileWriter<TImageType>::New();
    writer->SetInput(sourceImage);
    writer->SetFileName("ellipsoid.vtk");
    writer->SetImageIO(vtkIO);
    writer->Write();

    return EXIT_SUCCESS;
    }
}
