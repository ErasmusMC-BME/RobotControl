// NXT++ test.cpp : Defines the entry point for the console application.
//

#include <vtkMatrix4x4.h>
#include <vtkCellArray.h>
#include <vtkTransform.h>



#include "vtkSmartPointer.h"
#include "vtkImageReader.h"
#include "vtkImageReslice.h"
#include "vtkLookupTable.h"
#include "vtkImageMapToColors.h"
#include "vtkImageActor.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkInteractorStyleImage.h"
#include "vtkCommand.h"
#include "vtkImageData.h"
#include "vtkImageMapper3D.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkInformation.h"
#include <vtkMetaImageReader.h>
#include <vtkPolyDataReader.h>
#include <vtkPolyDataMapper.h>
#include <vtkImageViewer2.h>
#include <vtkImageViewer.h>
#include <vtkPoints.h>
#include <vtkLandmarkTransform.h>
#include <vtkTransform.h>
#include <vtkMath.h>
#include <vtkBorderRepresentation.h>
#include <vtkBorderWidget.h>

#include "vtkCamera.h"
#include "vtkColorTransferFunction.h"
#include "vtkPiecewiseFunction.h"
#include "vtkPlanes.h"
#include "vtkProperty.h"
#include "vtkVolume.h"
#include "vtkVolumeProperty.h"
#include "vtkSmartVolumeMapper.h"
#include "vtkImageMapToColors.h"
#include <vtkPlane.h>
#include <vtkCutter.h>
#include <vtkCleanPolyData.h>
#include <vtkPolyDataConnectivityFilter.h>
#include <vtkPolyDataWriter.h>
#include <vtkTransformPolyDataFilter.h>
#include "vtkPolyData.h"
#include "vtkPoints.h"
#include "vtkPointData.h"
#include "vtkCellArray.h"
#include "vtkPolyDataMapper.h"
#include <vtkPolyDataConnectivityFilter.h>
#include <vtkPolyDataWriter.h>
#include <vtkVertexGlyphFilter.h>
#include <vtkParametricFunctionSource.h>
#include <vtkSplineWidget.h>
#include <vtkKochanekSpline.h>
#include <vtkParametricSpline.h>
#include <vtkPlaneSource.h>
#include <vtkImageProperty.h>
#include <utility.h>

// Base Function definitions
template  <class Tin,class Tout>  ViewingThread<Tin,Tout>::ViewingThread( void )
{
	_bViewingReady = false;
	_bViewingFound = false;
}

template  <class Tin,class Tout>  ViewingThread<Tin,Tout>::~ViewingThread( void )
{
}

template <class Tin,class Tout>  ViewingThread<Tin,Tout> *  ViewingThread<Tin,Tout>::New( thread<Tin,Tout> * _thread=NULL )  // Create the object
{
	if(m_Viewingthread==NULL)
	{
		m_Viewingthread= new ViewingThread;
	}
	return (ViewingThread<Tin,Tout> * ) m_Viewingthread;
}

template  <class Tin,class Tout>  void  ViewingThread<Tin,Tout>::Initialize(const char *fmt, ...)
{
};

template  <class Tin,class Tout>  void ViewingThread<Tin,Tout>::ThreadEntryPoint( void )
{

	ImageConverterITKToVTK::Pointer ImageConverterITKToVTKPtr=m_InputData->GetImageConverterITKToVTKPtr();

	 ImageType::Pointer image = ImageType::New();

	 ImageType::IndexType start;
   start.Fill(0);
 
  ImageType::SizeType size;
  size.Fill(512);
  char zval=66;
  ImageType::RegionType region(start, size);
  image->SetRegions(region);
  image->Allocate();
  image->FillBuffer(zval);
	ImageConverterITKToVTKPtr->SetInput(image);
	ImageConverterITKToVTKPtr->Update();

	vtkSmartPointer<vtkRenderer> renderer2DQ1=	vtkSmartPointer<vtkRenderer>::New();
	vtkSmartPointer<vtkRenderer> renderer3DQ2 =	vtkSmartPointer<vtkRenderer>::New();
	
	vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
	renderWindow->SetMultiSamples(0);
	renderWindow->SetSize( 1024,1024);
	renderWindow->AddRenderer(renderer2DQ1);
	renderWindow->AddRenderer(renderer3DQ2);
	vtkSmartPointer<vtkImageActor> XYPlaneimageActor =	vtkSmartPointer<vtkImageActor>::New();
	renderer2DQ1->AddActor(XYPlaneimageActor);
	XYPlaneimageActor->SetInput(	ImageConverterITKToVTKPtr->GetOutput());
	 XYPlaneimageActor->Update();
  // Coordinates are expressed as (xmin,ymin,xmax,ymax), where each
  // coordinate is 0 <= coordinate <= 1.0.

	renderer2DQ1->SetViewport(0,0,0.5,1);
	renderer3DQ2->SetViewport(0.5,0,1,1);
	//renderer3DQ2->SetViewport(0.5,0,1,1);
	vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
	vtkSmartPointer<vtkRenderWindowInteractor> interactor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	interactor->SetRenderWindow(renderWindow);

	//vtkVolume *volume = vtkVolume::New();
	//vtkSmartVolumeMapper *mapper = vtkSmartVolumeMapper::New();
//	volume->SetMapper( mapper );
	//setColorType(4, volume  );
	//mapper->SetInput( ImageConverterITKToVTKPtr->GetOutput() );
	//renderer3DQ2->AddVolume( volume );
	//renderer3DQ2->ResetCamera();
	renderWindow->Render();
	//interactor->Start();
//	volume->Delete();
	//mapper->Delete();
	//renderer3DQ2->Delete();
//	renderWindow->Delete();
//	interactor->Delete();
	do
	{
		if(ImageConverterITKToVTKPtr->GetOutput())
		{
		 XYPlaneimageActor->SetInput(	ImageConverterITKToVTKPtr->GetOutput());
		 XYPlaneimageActor->Update();
		}
	 	} while (1);

	// This is the desired entry-point-function but to get
	// here we have to use a 2 step procedure involving
	// the ThreadStaticEntryPoint() function.
	// should be a defined var
	


	_endthread();
}

