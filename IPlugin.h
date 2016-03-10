
/////////////////////////////////////////////////////////////////////////////////////
// Abstract base class ("interface") for the concrete plugin implementations

#define CATEGORY_EFFECT 0
#define CATEGORY_BUILDING_BLOCK 1
#define CATEGORY_UIELEMENT 2


#define CHANNEL_R 2
#define CHANNEL_G 1
#define CHANNEL_B 0
#define CHANNEL_A 3
#define CLAMP255(x) (((x) > (255)) ? (255) : (((x) < (0)) ? (0) : (x)))


#define TYPE_SLIDER 0
#define TYPE_CHECKBOX 1
#define TYPE_LABEL 2
#define TYPE_ONEOFMANY 3
#define TYPE_COLOR 4
#define TYPE_IMGFILE 5
#define TYPE_CHECKBOXDISABLENEXT 6
#define TYPE_EDIT 7
#define TYPE_COMBO 8
#define TYPE_FONTCOMBO 9
#define TYPE_TEXT 10
#define TYPE_EXPSLIDER 11
#define TYPE_PUSHBUTTON 12
#define TYPE_GAMMASLIDER 13
#define TYPE_LOGSLIDER 14
#define TYPE_POSITION 15
#define TYPE_INTEGER 16
#define TYPE_MULTIEDIT 17
#define TYPE_CHECKBOXDISABLENEXTUNTIL 18
#define TYPE_SPACE 19


#define FLAG_NONE 0
#define FLAG_UPDATE_IMMEDIATELY 1
#define FLAG_HELPER 2
#define FLAG_BINDING 4
#define FLAG_DUMMY 8
#define FLAG_SKIPFINAL 16
#define FLAG_LONGPROCESS 32
#define FLAG_NEEDSIZEDATA 64
#define FLAG_NEEDMOUSE 128


#define DRAW_AUTOMATICALLY 0
#define DRAW_SIMPLE_A 1
#define DRAW_SIMPLE_B 2
#define DRAW_SOCKETSONLY 3

typedef struct UIPARAMETERS
{
	// 0...100
	char m_sLabel[255];
	double m_dValue;
	double m_dMin;
	double m_dMax;
	double m_dSpecialValue;
	int m_nType; 
	
} UIParameters;

class IPlugin
{
public:

	//returns the Title of the plugin box
	virtual const char* GetTitle () const = 0;
	
	// returns description visible in help
	virtual const char* GetDescription () const = 0;
	
	// number of inputs and outputs
	virtual int GetInputNumber () = 0;
	virtual int GetOutputNumber () = 0;
	virtual int GetBoxColor () = 0;
	virtual int GetTextColor () = 0;
	virtual int GetBoxWidth () = 0;
	virtual int GetFlags () = 0;

	virtual int GetUIParameters (UIParameters* pParameters) = 0;

	
	virtual int GetDrawingType () = 0;
	virtual void CustomDraw (HDC hDC, int nX,int nY, int nWidth, int nHeight, float scale,BOOL bIsHighlighted, UIParameters* pParameters) = 0;


	virtual BOOL UIParametersChanged (UIParameters* pParameters, int nParameter) = 0;


	virtual void SetSizeData(int nOriginalW, int nOriginalH, int nPreviewW, int nPreviewH, double dCropX1, double dCropY1, double dCropX2, double dCropY2, double dZoom) = 0;


	//does the actual data processing
	virtual void Process_Data (BYTE* pBGRA_out,BYTE* pBGRA_in, int nWidth, int nHeight, UIParameters* pParameters) = 0;
	virtual void Process_Data2 (BYTE* pBGRA_out, BYTE* pBGRA_in1, BYTE* pBGRA_in2, int nWidth, int nHeight, UIParameters* pParameters) = 0;


	virtual BOOL MouseButtonDown (int nX, int nY, int nWidth, int nHeight, UIParameters* pParameters) = 0;
	virtual BOOL MouseMove (int nX, int nY, int nWidth, int nHeight, UIParameters* pParameters) = 0;
	virtual BOOL MouseButtonUp (int nX, int nY, int nWidth, int nHeight, UIParameters* pParameters) = 0;

	virtual BOOL UIButtonPushed (int nParam, int nSubButton, UIParameters* pParameters) = 0;


};

/////////////////////////////////////////////////////////////////////////////////////
// Extern "C" functions that each plugin must implement in order to be recognized 
// as a plugin by us.

// Plugin factory function
//extern "C" IPlugin* Create_Plugin ();

// Plugin cleanup function
//extern "C" void Release_Plugin (IPlugin* p_plugin);


// Plugin Name that appears on Interface
//extern "C" __declspec(dllexport) char* GetPluginName()

//UNIQUE Plugin ID in a form of reverse domain
//ex: 
//com.mediachance.myplugin
//extern "C" __declspec(dllexport) char* GetPluginID()

//Plugin Category
//extern "C" __declspec(dllexport) int GetCategory()

