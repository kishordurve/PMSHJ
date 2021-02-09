#if !defined(AFX_DEFINES__INCLUDED_)
#define AFX_DEFINES__INCLUDED_
#include "IOutputLog.h"
#include "IHighResTimer.h"
#include <afxtempl.h>

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define	EPSILON						0.0001
#define PI							3.14159265358979
#define RADIANS_TO_DEGREES			57.29577951308238
#define KG_TO_LBS					2.20462262
#define KG_TO_Oz					35.23796195
const int GRID_ROWS					= 5;
const int GRID_COLS					= 5;

enum e3DCamType { BASLER = 0, ORBBEC, PGR_3D};
enum eCamNum { CAM1 = 0, CAM2, CAM3, CAMALL};
enum eScreenType { SCAN_SCREEN1 = 0, SCAN_SCREEN2, SCAN_SCREEN3, SCAN_SCREEN4, SCAN_SCREEN5, SCAN_SCREEN6,SCAN_SCREEN_NONE};
enum PwdType { ADMIN = 0, SUP, OPR, PWD_NONE };
enum PORT { INPUTPORT, OUTPUTPORT };
enum eRegDataType { REG_DT_NONE, REG_DT_STRING, REG_DT_INT };
enum eBackColor {BLACK = 0, WHITE};
enum eDimUnits {DIM_MM = 0, DIM_CM, DIM_M, DIM_INCH, DIM_FT};
enum eWtUnits { Oz = 0, Kg, Gm, Lb, T, INVALID_WT };
enum eLanguage { ENGLISH = 0, CHINESE, MALAYSIAN};
// CONT means scale sends data continuously. Stable - sends data when weight is stable. So in this mode the last sent reading is the correct one
enum eWeighScaleMode { CONT = 0, STABLE };
enum eWeighScaleType { STANDARD = 0, EXCELL, CH340, P5, GRAM };// STANDARD is the old scale, EXCELL is the Taiwanese co. scale , P5 is the Chinese customer scale
enum eScrTimeOut { OFF = 0, Min1, Min5, Min10, Min15 };
enum eCodeType { Barcode = 0, DMcode, QRcode};// DMcode is added as QRcode is '2' in HikVision library MVID_CODE_TDCR_QR = 2
enum eDataLoggingStatus { DATA_OK = 0, DATA_NOT_DONE, DATA_FAIL};
enum eFreightMode {FREIGHT_AIR = 1, FREIGHT_SEA, FREIGHT_WT}; // Purposely starting with '1' 

const float DEF_XCALIB			= 0.055f;
const float DEF_YCALIB			= 0.050f;

const COLORREF	FORM_BACK_COLOR(RGB(192, 224, 255)),
				GREEN_COLOR(RGB(0, 255, 0)),
				RED_COLOR(RGB(255, 0, 0)),
				GRAY_COLOR (RGB(192, 192, 192)),
				DIALOG_BACK_COLOR(RGB(10, 10, 13)),	// was 160,190,160 - Decides color of the 2 RH side panes
				FINETUNEROI_COLOR(RGB(120, 120, 120)),
				MAGENTA_COLOR(RGB(255, 0, 255)),
				WHITE_COLOR(RGB(255, 255, 255)),
				BLUE_COLOR(RGB(0, 0, 255)),
				YELLOW_COLOR(RGB(255, 255, 0)),
				CYAN_COLOR(RGB(0,255,255)),
				LIGHT_CYAN_COLOR(RGB(220,255,255)),
				ORANGE_COLOR(RGB(255,128,0)),
				BUTTON_BK_COLOR(RGB(55,156,247)),
				BUTTON_COLOR(RGB(11,138,232)),
				BTN_BACK_COLOR(RGB(149, 202, 255)),
				DIMENSIONERO_BACK_COLOR(RGB(10, 10, 13)),
				DIMENSIONERO_GRAY_COLOR(RGB(31, 31, 33)),
				DIMENSIONERO_GRAY_LIGHT_COLOR(RGB(44, 44, 48)),
				CAPTION_BACK_COLOR(RGB(255, 230, 200));

const int 		DEFAULT_ASYNC_VAL		= 0,
				DEFAULT_RESULT_LOG_TYPE = 0,
				DEFAULT_LOG_AT_STOP		= 0,
				DEFAULT_PWD_TIMEOUT		= 10,
				DEFAULT_RESET_COUNT		= 1000,
				DEFAULT_RESET_DAYS		= 1,
				DEFAULT_RESET_OPTION	= 0,
				DEFAULT_SAVE_IMAGE_VAL	= 0,
				DEFAULT_DATA_LOGGING_OPTION = 0,
				DEFAULT_DATALOG_COUNT = 0,
				DEFAULT_DATA_LOGTYPE = 0,
				DEFAULT_LOG_LOTWISE = 0,
				DEFAULT_MANUAL_RESET_COUNT = 3000;

const float		DEFAULT_YIELD_THRESHOLD = 0;

#define IMSG_SETBIT				WM_USER + 200
#define IMSG_GETBIT				WM_USER + 201
#define IMSG_SENDBIT			WM_USER + 202
#define IMSG_INITAPP			WM_USER + 203
#define IMSG_TRIGGER			WM_USER + 204
#define IMSG_RESULT				WM_USER + 205
#define IMSG_EXITAPP			WM_USER + 206
#define WM_UPDATE_WEIGHT		WM_USER + 207
#define WM_UPDATE_CODE			WM_USER + 208
#define WM_FOOT_SW_TRG_RCD		WM_USER + 209
#define WM_FOOT_SW_READ_FAIL	WM_USER + 210
#define WM_DATA_LOG_STATUS		WM_USER + 211
#define WM_WEIGHT_ERROR			WM_USER + 212

// camera pane information
#define CAM_PANE_WIDTH_FULL_SCREEN      1024
#define CAM_PANE_HEIGHT_FULL_SCREEN     480 //
#define CAM_PANE_WIDTH_HALF_SCREEN      320 // Common for both half as well as full screen displays
#define CAM_PANE_HEIGHT_HALF_SCREEN     240 // 
#define CAM_PANE_MIN_WIDTH			    100
#define CAM_PANE_MIN_HEIGHT				100 //
#define STATUS_VIEW_PANE				65
#define RESULT_VIEW_PANE				310 // Not used as of now
#define IMAGE_TN_WIDTH					333// Thumbnail to be used in Data first page (History) 
#define IMAGE_TN_HEIGHT					222
// Registry Location for JobInfo
#define DEVICE_INFO_KEY					"Software\\Duravision\\PMSHJ\\Settings\\DeviceInfo"
#define RATIO_F_T0_H_COMPRESS			2.0		//ratio of full compress image to half compress image
// Async - exposure time in MILLISECONDS
#define	MIN_EXPOSURE_TIME				"0.03"
#define	MAX_EXPOSURE_TIME				"50.0"
#define IMAGE_FOLDER_NAME				"Images"
#define MIN_ROWS						17// For Grid in CViewUserDetails dlg
#define ASCII_VALUE_SPACE				32
#define STRING_LENGTH_MAX				64L
#define MIN_ROWS						17

//default registry values.
#define	DEFAULT_THRESHOLD	125			
#define	DEFAULT_EXPOSURE	"10.0"
#define DEFAULT_EXPOSURE_CALIB	"2.0"
#define	DEFAULT_OPERMAX		"100"
#define	DEFAULT_OPERMIN		"50"
#define	DEFAULT_MMPERPIXEL	"0.005"
#define	DEFAULT_MMPERPIXELTODSP		"1"
#define	DEFAULT_ALLCOUNT	0
#define	DEFAULT_MAXCOUNT	0

const float	DEFAULT_EXPOSURE_TIME	=		0.2f;

const int  DISPLAY_WIDTH		=	1367;
const int  DISPLAY_HEIGHT		=	769;// With 768 it screws up!
const int  IMAGE_WIDTH			=	640;
const int  IMAGE_HEIGHT			=	480;
const int  IMAGE_PANE_WIDTH		=	1024;
const int  IMAGE_PANE_HEIGHT	=	600;// 480;
const int  IMAGE_WIDTH_2D		=	1280;
const int  IMAGE_HEIGHT_2D		=	960;
const int  IMAGE_WIDTH_SCAN5	=	512;// Size of image shown in Scan5
const int  IMAGE_HEIGHT_SCAN5	=	312;
const int  IMAGE_WIDTH_SCAN6	=	400;// Size of image shown in Scan6
const int  IMAGE_HEIGHT_SCAN6	=	700;

typedef CArray<CRect, CRect> RECTARRAY;

const unsigned int DEVELOPER_PWD_TIMEOUT_PERIOD = 300000;// 300 seconds (5 minutes)!
const int DEFAULT_INT_VAL = 0;
const long NUM_INTENSITIES = 256;
const CString STATUS_DUMP_FOLDER_NAME = L"Status";
const CString APP_SIGNATURE = L"DimensionerO";
const CString VERSION_NO = L"0.9";
const CString REGISTRY_NAME = L"DimensionerO";
//Default Folder Names
const CString g_strYieldLogFolderName = L"YieldLog", g_strConfigFileFolderName = L"Config Files", g_strStatusFolder = L"Status", g_strImagesFolder = L"Images", g_strScanData = L"ScanData", g_strUserData = L"UserData";
const int TIMER_TIMEOUT = 10000;		// Currently set to 10 sec
//Default Params
const float DEFAULT_MIN_BLOB_AREA	= 80;
const float DEFAULT_MAX_BLOB_AREA	= 800;
// Calib Factors
const float DEFAULT_XCALIB			= 0.250f;
const float DEFAULT_YCALIB			= 0.250f;
const float MIN_XCALIB				= 0.070f;
const float MAX_XCALIB				= 0.40f;
const float MIN_YCALIB				= 0.070f;
const float MAX_YCALIB				= 0.40f;
const int MAX_INSP_TIME				= 100;// MS
const int WAIT_TIME					= 10;
const int CROSS_HALFSCREEN_SIZE		= 3;
const int CROSS_FULLSCREEN_SIZE		= 6;
// IO Bits
const int MIN_BIT_NUM				=	0;
const int MAX_BIT_NUM				=	32;
// Camera Sl Numbers
const int DEFAULT_CAMERA2D_1_SERIAL	= 22467479;
const CString DEFAULT_CAMERA3D_1_SERIAL = L"22839279";

#endif
