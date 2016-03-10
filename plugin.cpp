// plugin.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "IPlugin.h"

#include <math.h>
#include <cmath>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>//to use cout
#include <algorithm>
#include <exception>


using namespace std;


////////////////////////////////////////////////////////////////////////
// A concrete plugin implementation
////////////////////////////////////////////////////////////////////////

// Photo-Reactor Plugin class

//****************************************************************************
//This code has been generated by the Mediachance photo reactor Code generator.


#define AddParameter(N,S,V,M1,M2,T,D) {strcpy (pParameters[N].m_sLabel,S);pParameters[N].m_dValue = V;pParameters[N].m_dMin = M1;pParameters[N].m_dMax = M2;pParameters[N].m_nType = T;pParameters[N].m_dSpecialValue = D;}

#define GetValue(N) (pParameters[N].m_dValue)
#define GetValueY(N) (pParameters[N].m_dSpecialValue)

#define SetValue(N,V) {pParameters[N].m_dValue = V;}

#define GetBOOLValue(N) ((BOOL)(pParameters[N].m_dValue==pParameters[N].m_dMax))

// if it is not defined, then here it is
//#define RGB(r,g,b) ((COLORREF)(((BYTE)(r)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(b))<<16)))

#define PARAM_RADIUS	0
//#define PARAM_COLORSPACE_NR0	1
#define NUMBER_OF_USER_PARAMS 1

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))


class Plugin1 : public IPlugin	
{
public:

		//Plugin Icon:
	//you can add your own icon by creating 160x100 png file, naming it the same as plugin dll and then placing it in the plugins folder
	//otherwise a generic icon will be use


	//this is the title of the box in workspace. it should be short
	const char* GetTitle () const
	{
		return "SNN";
	}
	
	// this will appear in the help pane, you can put your credits and short info
	const char* GetDescription () const
	{
		return "Symmetric Nearest Neighbor is a non-linear edge preserving smooth that flattens gradients and when turned to higher radius settings, makes the image appear painted.  The filter compair opposite pairs of pixels in the kernel and selects those pixels closest in value to the center pixel, it then returns the mean of those pixels to the center.  This filter takes quite a while to process at higher kernel values.";
	}

	// BASIC PARAMETERS
	// number of inputs 0,1 or 2
	int GetInputNumber ()
	{
		return 1;
	}

	// number of outputs 0 or 1
	int GetOutputNumber ()
	{
		return 1;
	}

	int GetBoxColor ()
	{
		return RGB(44,78,119);
	}

	int GetTextColor ()
	{
		return RGB(165,236,255);
	}

	// width of the box in the workspace
	// valid are between 50 and 100
	int GetBoxWidth ()
	{
		return 80;
	}

	// set the flags
	// see the interface builder
	// ex: nFlag = FLAG_FAST_PROCESS | FLAG_HELPER;

	//FLAG_NONE same as zero	Default, no other flags set
	//FLAG_UPDATE_IMMEDIATELY	It is very fast process that can update immediately. When user turns the sliders on UI the left display will update
	//							Use Update Immediately only for fast and single loop processes, for example Desaturate, Levels.
	//FLAG_HELPER				It is an helper object. Helper objects will remain visible in Devices and they can react to mouse messages. Example: Knob, Monitor, Bridge Pin
	//FLAG_BINDING				Binding object, attach to other objects and can change its binding value. It never goes to Process_Data functions.  Example: Knob, Switch, Slider
	//FLAG_DUMMY				It is only for interface but never process any data. Never goes to Process_Data functions. Example: Text note
	//FLAG_SKIPFINAL			Process data only during designing, doesn't process during final export. Example: Monitor, Vectorscope 
	//FLAG_LONGPROCESS			Process that takes > 1s to finish. Long Process will display the Progress dialog and will prevent user from changing values during the process.
	//FLAG_NEEDSIZEDATA		    Process need to know size of original image, the zoom and what part of image is visible in the preview. When set the plugin will receive SetSizeData
	//FLAG_NEEDMOUSE			Process will receive Mouse respond data from the workplace. This is only if your object is interactive, for example Knob, Slider

	int GetFlags ()
	{
		// it is fast process
		//int nFlag = FLAG_UPDATE_IMMEDIATELY;
		
		int nFlag = FLAG_LONGPROCESS;

		return nFlag;
	}


	// User Interface Build
	// there is maximum 29 Parameters

	int GetUIParameters (UIParameters* pParameters)
	{

		// label, value, min, max, type_of_control, special_value
		// use the UI builder in the software to generate this


		AddParameter( PARAM_RADIUS ,"Radius", 1.0, 1.0, 20.0, TYPE_SLIDER, 0.0);//default  min   max
		//AddParameter( PARAM_COLORSPACE_NR0 ,"LAB|YCbCr", 0, 0, 1, TYPE_ONEOFMANY, 0);

		return NUMBER_OF_USER_PARAMS;
	}
	


	// Actual processing function for 1 input
	//***************************************************************************************************
	// Both buffers are the same size
	// don't change the IN buffer or things will go bad for other objects in random fashion
	// the pBGRA_out comes already with pre-copied data from pBGRA_in
	// Note: Don't assume the nWidth and nHeight will be every run the same or that it contains the whole image!!!! 
	// This function receives buffer of the actual preview (it can be just a crop of image when zoomed in) and during the final calculation of the full buffer
	// this is where the image processing happens
	virtual void Process_Data (BYTE* pBGRA_out,BYTE* pBGRA_in, int nWidth, int nHeight, UIParameters* pParameters)
	{
		//List of Parameters
		double dRadius = GetValue(PARAM_RADIUS);// used to grab radius from control
		int radius = (int)dRadius;//convert radius to an integer

		//int ColorSpace = (int)GetValue(PARAM_COLORSPACE_NR0);


		int size = (radius + radius) + 1;// we want an odd sized kernel //int size = (radius * 2) + 1;

		//int junkvariabletoseeifcppisawake;


		int sumR;
		int sumG;
		int sumB;

		int rc;
		int gc;
		int bc;

		//double delta1;
		//double delta2;

		int divisor;


		for(int y = 0; y < nHeight; y++)
		{
			for(int x = 0; x < nWidth; x++)
			{
				int nIdx = x * 4 + y * 4 * nWidth;


				//small array for each kernel to grab each value, 
				//this will be passed on to another array that adjust its size
				//if you are on the borders of the image
				BYTE* Rkernelarray = new BYTE[size*size]; // Red kernel array
				BYTE* Gkernelarray = new BYTE[size*size]; // Green kernel array
				BYTE* Bkernelarray = new BYTE[size*size]; // Blue kernel array

				int elementsinarray = 0;

				int flexHorizontalKernelSize = 0;//needed when cycling thru smaller kernels generated at edges
				//we will need a flex size for horizontal and vertical since they can vary on the edges

				//This is our Kernel
				//collect pixels of each color into seperate arrays
				for(int i = max(0, x - radius); i <= min(nWidth - 1, x + radius); i++)
				{
					for(int j = max(0, y - radius); j <= min(nHeight - 1, y + radius); j++)
					{
						//this slides / moves along the kernel to collect the neighboring pixels
						int redKernelSlider   = pBGRA_in[(i + j * nWidth) * 4 + CHANNEL_R];
						int greenKernelSlider = pBGRA_in[(i + j * nWidth) * 4 + CHANNEL_G];
						int blueKernelSlider  = pBGRA_in[(i + j * nWidth) * 4 + CHANNEL_B];
						//this slides / moves along the kernel to collect the neighboring pixels

						Rkernelarray[elementsinarray] = redKernelSlider;
						Gkernelarray[elementsinarray] = greenKernelSlider;
						Bkernelarray[elementsinarray] = blueKernelSlider;

						elementsinarray ++; // this counts how many items we place into the first array
											// at the borders, this will be a different size from the
											// kernel
					}//end J
					flexHorizontalKernelSize ++; // this measures how large the array will be horizonally
				}//end I


				int flexVerticalKernelSize = elementsinarray / flexHorizontalKernelSize;
				// to find the vertical size of the array, we will divide the elements in the array
				// or the sum of all of the elements against the flexHorizontalKernel size.

				// these arrays adjust to the size of the actual number of pixels in the kernel
				// so if you are on the border, which will not be the radius specified by the size
				// variable, as the kernel moves away from the edges of the image, the below arrays 
				// will resize accordingly.  When completey away from the edges, it will be the full
				// radius specified in size.

				BYTE* RArray = new BYTE[elementsinarray]; // kernel array
				BYTE* GArray = new BYTE[elementsinarray]; // kernel array
				BYTE* BArray = new BYTE[elementsinarray]; // kernel array


				//at the edges of the image, the kernel size does not match to the selected size
				//we present another array series.  If you are at the edges, the new array will be
				//smaller, away from the edges the kernel array is the same size.  This routine
				//just copies valad pixels
				for(int i = 0; i < elementsinarray; i++)
				{
					RArray[i] = Rkernelarray[i];
					GArray[i] = Gkernelarray[i];
					BArray[i] = Bkernelarray[i];
				}
				
				// lets delete the original arrays (since they are no longer needed) and free memory
				delete [] (BYTE*) Rkernelarray;
				delete [] (BYTE*) Gkernelarray;
				delete [] (BYTE*) Bkernelarray;

				// The below routine is used to perform a check to see if the pixels were gathered properly
				//only uncomment if you want to see how it works, and only if you have a valad array to compare it against
				/*
				{
					//just print pixels in kernel, for testing only //ok to this point
					for(int j = 0; j < flexHorizontalKernelSize; j++)//for(int j = 0; j < size; j++)
					{
						for(int i = 0; i < flexVerticalKernelSize; i++)//for(int i = 0; i < size; i++)
						{
							int redpixel   = RArray[(j * flexVerticalKernelSize) + i];
							int greenpixel = GArray[(j * flexVerticalKernelSize) + i];
							int bluepixel  = BArray[(j * flexVerticalKernelSize) + i];

							
							//if (y > 7 & x > 7)
							{
							
							char sBuffer1[200]; sprintf(sBuffer1,"j = %d \n" "i = %d \n""x = %d \n" "y = %d \n" "flexHorizontalKernelSize = %d \n" "flexVerticalKernelSize = %d \n" "elementsinarray = %d \n" "redpixel = %d"
							,
							j, i, x, y, flexHorizontalKernelSize, flexVerticalKernelSize, elementsinarray, redpixel);MessageBox(NULL,sBuffer1,"ENTIRE KERNEL", MB_OK);
							
							}

						}
					}
					if (y > 7 & x > 7) 
					{char sBuffer22[200]; sprintf(sBuffer22,"STOP");MessageBox(NULL,sBuffer22,"STOP", MB_OK);}
				}
				*/

				rc = pBGRA_in[nIdx + CHANNEL_R];
				gc = pBGRA_in[nIdx + CHANNEL_G];
				bc = pBGRA_in[nIdx + CHANNEL_B];

				sumR = 0;
				sumG = 0;
				sumB = 0;
				divisor = 0;


				int jj = flexHorizontalKernelSize - 1;
				int ii = flexVerticalKernelSize - 1;				
				for(int j = 0; j < flexHorizontalKernelSize; j++)//for(int j = 0; j < size; j++)
				{
					for(int i = 0; i < flexVerticalKernelSize; i++)//for(int i = 0; i < size; i++)
					{
						{
							divisor ++;

							int r1 = RArray[i + j * flexVerticalKernelSize];
							int g1 = GArray[i + j * flexVerticalKernelSize];
							int b1 = BArray[i + j * flexVerticalKernelSize];

							int r2 = RArray[ii + jj * flexVerticalKernelSize];
							int g2 = GArray[ii + jj * flexVerticalKernelSize];
							int b2 = BArray[ii + jj * flexVerticalKernelSize];

							double	delta1 = sqrt ((rc - r1) * (rc - r1) + (gc - g1) * (gc - g1) + (bc - b1) * (bc - b1));
							double	delta2 = sqrt ((rc - r2) * (rc - r2) + (gc - g2) * (gc - g2) + (bc - b2) * (bc - b2));
							

							


							if (delta1 < delta2)
							{
								sumR += r1;
								sumG += g1;
								sumB += b1;
							}
							else
							{
								sumR += r2;
								sumG += g2;
								sumB += b2;
							}
							ii--;
						}
					}//end j
					jj--;
					ii = flexVerticalKernelSize - 1;
				}//end i


				int rSNN = sumR / divisor;
				int gSNN = sumG / divisor;
				int bSNN = sumB / divisor;
					
				{//output
					nIdx = x * 4 + y * 4 * nWidth;

					pBGRA_out[nIdx + CHANNEL_R] = CLAMP255(rSNN);
					pBGRA_out[nIdx + CHANNEL_G] = CLAMP255(gSNN);
					pBGRA_out[nIdx + CHANNEL_B] = CLAMP255(bSNN);
				}//end output

				delete [] (BYTE*) RArray;//free memory
				delete [] (BYTE*) GArray;//free memory
				delete [] (BYTE*) BArray;//free memory


			}//end X
		}//end Y

	}//end routine


						


	// actual processing function for 2 inputs
	//********************************************************************************
	// all buffers are the same size
	// don't change the IN buffers or things will go bad
	// the pBGRA_out comes already with copied data from pBGRA_in1
	virtual void Process_Data2 (BYTE* pBGRA_out, BYTE* pBGRA_in1, BYTE* pBGRA_in2, int nWidth, int nHeight, UIParameters* pParameters)
	{

	}


	//*****************Drawing functions for the BOX *********************************
	//how is the drawing handled
	//DRAW_AUTOMATICALLY	the main program will fully take care of this and draw a box, title, socket and thumbnail
	//DRAW_SIMPLE_A			will draw a box, title and sockets and call CustomDraw
	//DRAW_SIMPLE_B			will draw a box and sockets and call CustomDraw
	//DRAW_SOCKETSONLY      will call CustomDraw and then draw sockets on top of it
	
	// highlighting rectangle around is always drawn except for DRAW_SOCKETSONLY

	virtual int GetDrawingType ()
	{

		int nType = DRAW_AUTOMATICALLY;

		return nType;

	}


	// Custom Drawing
	// custom drawing function called when drawing type is different than DRAW_AUTOMATICALLY
	// it is not always in real pixels but scaled depending on where it is drawn
	// the scale could be from 1.0 to > 1.0
	// so you always multiply the position, sizes, font size, line width with the scale
	
	virtual void CustomDraw (HDC hDC, int nX,int nY, int nWidth, int nHeight, float scale, BOOL bIsHighlighted, UIParameters* pParameters)
	{
	}


	//************ Optional Functions *****************************************************************************************
	// those functions are not necessary for normal effect, they are mostly for special effects and objects


	// Called when FLAG_HELPER set. 
	// When UI data changed (user turned knob) this function will be called as soon as user finish channging the data
	// You will get the latest parameters and also which parameter changed
	// Normally for effects you don't have to do anything here because you will get the same parameters in the process function
	// It is only for helper objects that may not go to Process Data 
	BOOL UIParametersChanged (UIParameters* pParameters, int nParameter)
	{
		
		return FALSE;
	}

	// when button is pressed on UI, this function will be called with the parameter and sub button (for multi button line)
	BOOL UIButtonPushed (int nParam, int nSubButton, UIParameters* pParameters)
	{

		return TRUE;
	}


	// Called when FLAG_NEEDSIZEDATA set
	// Called before each calculation (Process_Data)
	// If your process depends on a position on a frame you may need the data to correctly display it because Process_Data receives only a preview crop
	// Most normal effects don't depend on the position in frame so you don't need the data
	// Example: drawing a circle at a certain position requires to know what is displayed in preview or the circle will be at the same size and position regardless of zoom
	
	// Note: Even if you need position but you don't want to mess with the crop data, just ignore it and pretend the Process_Data are always of full image (they are not). 
	// In worst case this affects only preview when using zoom. The full process image always sends the whole data

	// nOriginalW, nOriginalH - the size of the original - full image. If user sets Resize on input - this will be the resized image
	// nPreviewW, nPreviewH   - this is the currently processed preview width/height - it is the same that Process_Data will receive
	//                        - in full process the nPreviewW, nPreviewH is equal nOriginalW, nOriginalH
	// Crop X1,Y1,X2,Y2       - relative coordinates of preview crop rectangle in <0...1>, for full process they are 0,0,1,1 (full rectangle)	
	// dZoom                  - Zoom of the Preview, for full process the dZoom = 1.0
	void SetSizeData(int nOriginalW, int nOriginalH, int nPreviewW, int nPreviewH, double dCropX1, double dCropY1, double dCropX2, double dCropY2, double dZoom)
	{

		// so if you need the position and zoom, this is the place to get it.
		// Note: because of IBM wisdom the internal bitmaps are on PC always upside down, but the coordinates are not


	}


	// ***** Mouse handling on workplace *************************** 
	// only if FLAG_NEEDMOUSE is set
	//****************************************************************
	//this is for special objects that need to receive mouse, like a knob or slider on workplace
	// normally you use this for FLAG_BINDING objects

	// in coordinates relative to top, left corner of the object (0,0)
	virtual BOOL MouseButtonDown (int nX, int nY, int nWidth, int nHeight, UIParameters* pParameters)
	{
		
		// return FALSE if not handled
		// return TRUE if handled
		return FALSE;
	}

	// in coordinates relative to top, left corner of the object (0,0)
	virtual BOOL MouseMove (int nX, int nY, int nWidth, int nHeight, UIParameters* pParameters)
	{
	

		return FALSE;
	}
	
	// in coordinates relative to top, left corner of the object (0,0)
	virtual BOOL MouseButtonUp (int nX, int nY, int nWidth, int nHeight, UIParameters* pParameters)
	{
		
		// Note: if we changed data and need to recalculate the flow we need to return TRUE

		// return FALSE if not handled
		// return TRUE if handled
		
		return TRUE;
	}


};

extern "C"
{
	// Plugin factory function
	__declspec(dllexport) IPlugin* Create_Plugin ()
	{
		//allocate a new object and return it
		return new Plugin1 ();
	}
	
	// Plugin cleanup function
	__declspec(dllexport) void Release_Plugin (IPlugin* p_plugin)
	{
		//we allocated in the factory with new, delete the passed object
		delete p_plugin;
	}
	
}


// this is the name that will appear in the object library
extern "C" __declspec(dllexport) char* GetPluginName()
{
	return "Andy's SNN";	
}


// This MUST be unique string for each plugin so we can save the data

extern "C" __declspec(dllexport) char* GetPluginID()
{
	
	
	return "com.lumafilters.Symmetric.nearest.neighbour";
	
}


// category of plugin, for now the EFFECT go to top library box, everything else goes to the middle library box
extern "C" __declspec(dllexport) int GetCategory()
{
		
	return CATEGORY_EFFECT;
	
}
