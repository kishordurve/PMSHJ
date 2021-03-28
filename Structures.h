#if !defined(AFX_STRUCTERS__INCLUDED_)
#define AFX_STRUCTERS__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "afxtempl.h"

#include "Defines.h"
#include "PMSHJ.h"


// Insp Params
struct STInspParams
{
	int		nMaxZDeviation;
	int		nMinZDeviation;
	int		nSpotRectSize;
	double	d3DCameraHeight;
	int		nMinObjectHeight;
	int		nMaxObjectHeight;
	BOOL	bIsCuboid;
	BOOL	bUseTracingMethod;
	BOOL	bFlipImageVert;
	BOOL	bFlipImageHorz;
	int		nNumFramesToGrab;
	int		nBarcodeLengthMin;
	int		nBarcodeLengthMax;
	double	dMinWt;
	int		nDsplAccuracy;// Added to show less variation in displayed dims. Display will be changed only if variation is more than this.
	int		nCamAlignRectSize;
	CString	strBarcodeValMin;
	CString	strBarcodeValMax;
	BOOL	bUseCodeRange;// Barcode is considered valid only if it is within a certain range

	STInspParams()
	{
		InitVar();
	}
	void InitVar()
	{
		nMaxZDeviation		= 30;
		nMinZDeviation		= 20;
		nSpotRectSize		= 20;
		d3DCameraHeight		= 1200;// This will be overwritten later when read from registry
		nMinObjectHeight	= 15;// This will be overwritten later when read from registry
		nMaxObjectHeight	= 700;// This will be overwritten later when read from registry
		bIsCuboid			= TRUE;
		bUseTracingMethod	= TRUE;
		nNumFramesToGrab	= 30;
		bFlipImageVert		= TRUE;
		bFlipImageHorz		= TRUE;
		nBarcodeLengthMin	= 16;
		nBarcodeLengthMax	= 18;
		dMinWt				= 0.5;
		nDsplAccuracy		= 5;
		nCamAlignRectSize	= 240;
		bUseCodeRange		= FALSE;
		strBarcodeValMin	= strBarcodeValMax = "0";// Code considered valid only if within this range. If both are '0' the range funda is not applicable.
	}
	STInspParams(STInspParams &other)
	{
		nMaxZDeviation		= other.nMaxZDeviation;
		nMinZDeviation		= other.nMinZDeviation;
		nSpotRectSize		= other.nSpotRectSize;
		d3DCameraHeight		= other.d3DCameraHeight;
		nMinObjectHeight	= other.nMinObjectHeight;
		nMaxObjectHeight	= other.nMaxObjectHeight;
		bIsCuboid			= other.bIsCuboid;
		bUseTracingMethod	= other.bUseTracingMethod;
		nNumFramesToGrab	= other.nNumFramesToGrab;
		bFlipImageVert		= other.bFlipImageVert;
		bFlipImageHorz		= other.bFlipImageHorz;
		nBarcodeLengthMin	= other.nBarcodeLengthMin;
		nBarcodeLengthMax	= other.nBarcodeLengthMax;
		dMinWt				= other.dMinWt;
		nDsplAccuracy		= other.nDsplAccuracy;
		nCamAlignRectSize	= other.nCamAlignRectSize;
		bUseCodeRange		= other.bUseCodeRange;
		strBarcodeValMin	= other.strBarcodeValMin;
		strBarcodeValMax	= other.strBarcodeValMax;
	}

	STInspParams& operator = (STInspParams& other)
	{
		nMaxZDeviation		= other.nMaxZDeviation;
		nMinZDeviation		= other.nMinZDeviation;
		nSpotRectSize		= other.nSpotRectSize;
		d3DCameraHeight		= other.d3DCameraHeight;
		nMinObjectHeight	= other.nMinObjectHeight;
		nMaxObjectHeight	= other.nMaxObjectHeight;
		bIsCuboid			= other.bIsCuboid;
		bUseTracingMethod	= other.bUseTracingMethod;
		nNumFramesToGrab	= other.nNumFramesToGrab;
		bFlipImageVert		= other.bFlipImageVert;
		bFlipImageHorz		= other.bFlipImageHorz;
		nBarcodeLengthMin	= other.nBarcodeLengthMin;
		nBarcodeLengthMax	= other.nBarcodeLengthMax;
		dMinWt				= other.dMinWt;
		nDsplAccuracy		= other.nDsplAccuracy;
		nCamAlignRectSize	= other.nCamAlignRectSize;
		bUseCodeRange		= other.bUseCodeRange;
		strBarcodeValMin	= other.strBarcodeValMin;
		strBarcodeValMax	= other.strBarcodeValMax;

		return *this;
	}
}; // STInspParams

struct STOptions
{
	BOOL bUseCodeAsTrigger;// Stands for all types of code QRCode, Barcode
	BOOL bQty;
	BOOL bBeepSound;
	BOOL bWtRqd;
	BOOL bSerialPortScanner;
	BOOL bHikVisionScanner;
	BOOL bPrintLabel;
	BOOL bUseFootSwitch;// If selected saves data and prints label only on switch press
	BOOL bSaveImage;
	BOOL bShowNetWt;
	BOOL bShowGrossWt;
	BOOL bShowTareWt;
	BOOL bShowPreTareWt;

	eDimUnits	DimUnits;
	eWtUnits	WtUnits;
	eLanguage	Language;
	int			nDecimalUnits;
	int			nWtAccuracy;// Number of places after decimal (x.xxx (3), x.xx (2), x.x (1), x (0))
	CString		strWtUnit;

	STOptions()
	{
		InitVar();
	}
	void InitVar()
	{
		bUseCodeAsTrigger = bQty = bBeepSound = bWtRqd = TRUE;
		DimUnits = DIM_MM;
		WtUnits = Kg;
		Language = CHINESE;
		nDecimalUnits = 0;
		nWtAccuracy = 2;
		bSerialPortScanner = FALSE;
		bHikVisionScanner = FALSE;
		bPrintLabel = FALSE;

		bShowNetWt		= FALSE;
		bShowGrossWt	= TRUE;
		bShowTareWt		= FALSE;
		bShowPreTareWt	= FALSE;
		bUseFootSwitch	= FALSE;
		bSaveImage		= TRUE;
	}
	STOptions(STOptions &other)
	{
		bUseCodeAsTrigger				= other.bUseCodeAsTrigger;
		bQty				= other.bQty;
		bBeepSound			= other.bBeepSound;
		bWtRqd				= other.bWtRqd; 
		DimUnits			= other.DimUnits;
		WtUnits				= other.WtUnits;
		Language			= other.Language;
		nDecimalUnits		= other.nDecimalUnits;
		nWtAccuracy			= other.nWtAccuracy;
		bSerialPortScanner	= other.bSerialPortScanner;
		bHikVisionScanner	= other.bHikVisionScanner;
		bPrintLabel			= other.bPrintLabel;
		bUseFootSwitch		= other.bUseFootSwitch;
		bSaveImage			= other.bSaveImage;

		bShowNetWt		= other.bShowNetWt;
		bShowGrossWt	= other.bShowGrossWt;
		bShowTareWt		= other.bShowTareWt;
		bShowPreTareWt	= other.bShowPreTareWt;
	}
	STOptions& operator = (STOptions& other)
	{
		bUseCodeAsTrigger				= other.bUseCodeAsTrigger;
		bQty				= other.bQty;
		bBeepSound			= other.bBeepSound;
		bWtRqd				= other.bWtRqd;
		DimUnits			= other.DimUnits;
		WtUnits				= other.WtUnits;
		nDecimalUnits		= other.nDecimalUnits;
		Language			= other.Language;
		nWtAccuracy			= other.nWtAccuracy;
		bSerialPortScanner	= other.bSerialPortScanner;
		bHikVisionScanner	= other.bHikVisionScanner;
		bPrintLabel			= other.bPrintLabel;
		bUseFootSwitch		= other.bUseFootSwitch;
		bSaveImage			= other.bSaveImage;

		bShowNetWt		= other.bShowNetWt;
		bShowGrossWt	= other.bShowGrossWt;
		bShowTareWt		= other.bShowTareWt;
		bShowPreTareWt	= other.bShowPreTareWt;

		return *this;
	}
}; // STOptions

// Represents overall result of Insp
struct STInspResult
{
	double		dLength;// Dimensions are in double as we may be measuring in inches
	double		dWidth;
	double		dHeight;
	double		dWeightNet;
	double		dWeightGr;
	double		dWeightTare;
	double		dWeightPT;
	double		dVolume;
	double		dFreight;// Freight is calculated elsewhere
	eFreightMode FreightMode;

	CString		strBarcode;
	CString		strWidth;
	CString		strHeight;
	CString		strVolume;
	CString		strWeightunit;
	CString		strDimensionUnit;
	CString		strTransportNo;
	CString		strPackageNo;
	CString		strMachineName;
	CString		strTime;

	BOOL		bDimensionReadSuccess;
	BOOL		bBarcodeReadSuccess;
	BOOL		bResultReady;
	BOOL		bResultPrinted;

	CString strImageFile2D;

	// Default constuctor
	STInspResult()
	{
		dLength = dWidth = dHeight = dWeightNet = dVolume = dWeightGr = dWeightTare = dWeightPT = dFreight = 0.0;
		bDimensionReadSuccess = bBarcodeReadSuccess = bResultReady = bResultPrinted = FALSE;
		strWeightunit = "Kg";
		strDimensionUnit = "M";
		FreightMode = FREIGHT_SEA;
	}

	STInspResult(STInspResult &other)
	{
		dLength		= other.dLength;
		dWidth		= other.dWidth;
		dHeight		= other.dHeight;
		dVolume		= other.dVolume;
		dWeightNet	= other.dWeightNet;
		dWeightGr	= other.dWeightGr;
		dWeightTare	= other.dWeightTare;
		dWeightPT	= other.dWeightPT;
		dFreight	= other.dFreight;

		strBarcode			= other.strBarcode;
		strDimensionUnit	= other.strDimensionUnit;
		strWeightunit		= other.strWeightunit;
		strTransportNo		= other.strTransportNo;
		strPackageNo		= other.strPackageNo;
		strMachineName		= other.strMachineName;
		strTime				= other.strTime;
		FreightMode			= other.FreightMode;

		bDimensionReadSuccess	= other.bDimensionReadSuccess;
		bBarcodeReadSuccess		= other.bBarcodeReadSuccess;
		bResultReady			= other.bResultReady;
		bResultPrinted			= other.bResultPrinted;

		strImageFile2D			= other.strImageFile2D;
	}

	STInspResult& operator = (STInspResult& other)
	{
		dLength		= other.dLength;
		dWidth		= other.dWidth;
		dHeight		= other.dHeight;
		dVolume		= other.dVolume;
		dWeightNet	= other.dWeightNet;
		dWeightGr	= other.dWeightGr;
		dWeightTare = other.dWeightTare;
		dWeightPT	= other.dWeightPT;
		dFreight	= other.dFreight;

		strBarcode			= other.strBarcode;
		strDimensionUnit	= other.strDimensionUnit;
		strWeightunit		= other.strWeightunit;
		strTransportNo		= other.strTransportNo;
		strPackageNo		= other.strPackageNo;
		strMachineName		= other.strMachineName;
		strTime				= other.strTime;
		FreightMode			= other.FreightMode;

		bDimensionReadSuccess	= other.bDimensionReadSuccess;
		bBarcodeReadSuccess		= other.bBarcodeReadSuccess;
		bResultReady			= other.bResultReady;
		bResultPrinted			= other.bResultPrinted;
		
		strImageFile2D			= other.strImageFile2D;

		return *this;
	}
}; // STInspResult

// Stores details of scan items
// Scan data (itemwise) is stored in folder Data//SacnID//ItemNum. 
// This data includes a txt file and pictures. The txt file contains the following fileds - Length, Width, Ht, Wt, Barcode, Date, Time
// 
struct STScanData
{
	CString strScanID;
	int nNumItems;
	CArray<STInspResult, STInspResult> arInspResults;
	CString strDate;
	CString strTime;

	// Default constuctor
	STScanData()
	{
		nNumItems = 0;
	}

	STScanData(STScanData &other)
	{
		strScanID	= other.strScanID;
		nNumItems	= other.nNumItems;
		strDate		= other.strDate;
		strTime		= other.strTime;

		arInspResults.RemoveAll();
		for (int i = 0; i < other.arInspResults.GetSize(); i++)
			arInspResults.Add(other.arInspResults[i]);
	}

	STScanData& operator = (STScanData& other)
	{
		strScanID	= other.strScanID;
		nNumItems	= other.nNumItems;
		strDate		= other.strDate;
		strTime		= other.strTime;

		arInspResults.RemoveAll();
		for (int i = 0; i < other.arInspResults.GetSize(); i++)
			arInspResults.Add(other.arInspResults[i]);
		
		return *this;
	}
	void AddItem(STInspResult stInspResult)
	{
		nNumItems++;
		arInspResults.Add(stInspResult);
	}

	void RemoveAllItems()
	{
		arInspResults.RemoveAll();
		nNumItems = 0;
	}
	void RemoveLastItem()
	{
		nNumItems--;
		if (nNumItems >= 0)
			arInspResults.RemoveAt(nNumItems);
	}

}; // STScanData

struct STComPortParams
{
	int nBaudRate;
	int nComPortNum;
	int nByteSize;
	int nParity;
	int nStopBit;

	STComPortParams()
	{
		InitVar();
	}
	void InitVar()
	{
		nBaudRate = 9600;
		nComPortNum = 1;// COM1
		nParity = 0;
		nStopBit = 1;
	}
};

struct STBuzzerTime
{
	BOOL bOnPass;
	BOOL bOnFail;
	CString strPassTime;
	CString	strFailTime;

};

struct STUserData
{
	CString strUserName;
	CString strProfession;
	int nAge;
};

struct STWeighScaleParams
{
	eWeighScaleType WeighScaleType;// STANDARD for EXCELL (Taiwan)
	eWeighScaleMode WeighScaleMode;// CONT or STABLE // 'STABLE' sends data when reading changes

	STWeighScaleParams()
	{
		WeighScaleType = EXCELL;
		WeighScaleMode = CONT;
	}
	STWeighScaleParams& operator = (STWeighScaleParams& other)
	{
		WeighScaleType = other.WeighScaleType;
		WeighScaleMode = other.WeighScaleMode;

		return *this;
	}
	STWeighScaleParams(STWeighScaleParams &other)
	{
		WeighScaleType = other.WeighScaleType;
		WeighScaleMode = other.WeighScaleMode;
	}
}; // STWeighScaleParams

// Barcode to be scanned on pkg
struct STBarcodeParts
{
	// Not used for HJ?
	CStringA yyyy;	// 4 chars
	CStringA MM;	// 2 chars
	CStringA DD;	// 2 chars
	CStringA tttt;	// 4 chars
	CStringA pppp;	// 4 chars

	CPoint pt1, pt2, pt3, pt4;// Corner points of barcode

	STBarcodeParts()
	{

	}
	STBarcodeParts& operator = (STBarcodeParts& other)
	{
		yyyy = other.yyyy;
		MM = other.MM;
		DD = other.DD;
		tttt = other.tttt;
		pppp = other.pppp;
		pt1 = other.pt1;
		pt2 = other.pt2;
		pt3 = other.pt3;
		pt4 = other.pt4;

		return *this;
	}
	STBarcodeParts(STBarcodeParts &other)
	{
		yyyy = other.yyyy;
		MM = other.MM;
		DD = other.DD;
		tttt = other.tttt;
		pppp = other.pppp;
		pt1 = other.pt1;
		pt2 = other.pt2;
		pt3 = other.pt3;
		pt4 = other.pt4;
	}
}; // STBarcodeParts

struct STUDFFileText
{
	CStringArray arStrings;// One string per line
	CString strUDFFileNameWithPath;
	STUDFFileText()
	{

	}

	void ClearAll()
	{
		arStrings.RemoveAll();
	}

	STUDFFileText& operator = (STUDFFileText& other)
	{
		arStrings.RemoveAll();
		for (int i = 0; i < other.arStrings.GetSize(); i++)
		{
			arStrings.Add(other.arStrings[i]);
		}
		strUDFFileNameWithPath = other.strUDFFileNameWithPath;

		return *this;
	}
	STUDFFileText(STUDFFileText &other)
	{
		arStrings.RemoveAll();
		for (int i = 0; i < other.arStrings.GetSize(); i++)
		{
			arStrings.Add(other.arStrings[i]);
		}
		strUDFFileNameWithPath = other.strUDFFileNameWithPath;
	}
}; // stUDFFileText

struct STDBRecordFileds
{
	CStringArray arStrings;// One string per line
	CString sta_name, data_stamp, wk_date, tran_no, pack_no, v_unit, wt_unit;
	double dpack_l, dpack_w, dpack_h, dpack_v, dpack_net, dpack_gross, dpack_tare, dpack_ptare;
	unsigned char upd_flag, del_mark;

	STDBRecordFileds()
	{
		dpack_l = dpack_w = dpack_h = dpack_v = dpack_net = dpack_gross = dpack_tare = dpack_ptare = 0.0f;
		upd_flag = del_mark = 0;
	}
	STDBRecordFileds& operator = (STDBRecordFileds& other)
	{
		sta_name = other.sta_name;
		data_stamp = other.data_stamp;
		wk_date = other.wk_date;
		tran_no = other.tran_no;
		pack_no = other.pack_no;
		v_unit = other.v_unit;
		wt_unit = other.wt_unit;

		dpack_l = other.dpack_l;
		dpack_w = other.dpack_w;
		dpack_h = other.dpack_h;
		dpack_v = other.dpack_v;

		dpack_net = other.dpack_net;
		dpack_gross = other.dpack_gross;
		dpack_tare = other.dpack_tare;
		dpack_ptare = other.dpack_ptare;

		upd_flag = other.upd_flag;
		del_mark = other.del_mark;

		return *this;
	}
	STDBRecordFileds(STDBRecordFileds &other)
	{
		sta_name = other.sta_name;
		data_stamp = other.data_stamp;
		wk_date = other.wk_date;
		tran_no = other.tran_no;
		pack_no = other.pack_no;
		v_unit = other.v_unit;
		wt_unit = other.wt_unit;

		dpack_l = other.dpack_l;
		dpack_w = other.dpack_w;
		dpack_h = other.dpack_h;
		dpack_v = other.dpack_v;

		dpack_net = other.dpack_net;
		dpack_gross = other.dpack_gross;
		dpack_tare = other.dpack_tare;
		dpack_ptare = other.dpack_ptare;

		upd_flag = other.upd_flag;
		del_mark = other.del_mark;
	}
}; // stUDFFileText

// Label printing params
struct STPrintParams
{
	int nLabelLengthMM;// Length, Width mm
	int nLabelWidthMM;// Width, Width mm
	int nGapMM;// Gap bet labels mm
	int nMarginMM;// Margin
	int nFontSize;// 0-Monotype CG Trimvurate, 1-8x12 fixed pitch dot,  2-12x20, 3-16x34, 4-24x32, 5-32x48, 6-TST24.BF2, 7-TSS24.BF2, K-Korean
	eCodeType CodeType;//
	int nCodeHtMM;// Barcode height mm
	int nNumPrints;// Number of labels to print on a 'Save'
	CString strCorporateName;
	CString strProductName;
	CString strPrinterName;
	BOOL bGetUserOKForPrinting;// Gets user confirmation before priting each label after code scan
	BOOL bPrintLogo;
	CString strLogoFile;

	STPrintParams()
	{
		nLabelLengthMM = 80;
		nLabelWidthMM = 100;
		nGapMM = 5;
		nMarginMM = 1;
		nFontSize = 2;//16x24
		CodeType = QRcode;
		nCodeHtMM = 10;
		nNumPrints = 1;
		strPrinterName = "GPS-1324D";
		bGetUserOKForPrinting = TRUE;
		bPrintLogo = FALSE;
		strLogoFile = L"D:\\LT\\PMSHJ\\LOGOCLEXP.PCX";
	}

	STPrintParams& operator = (STPrintParams& other)
	{
		nLabelLengthMM = other.nLabelLengthMM;
		nLabelWidthMM = other.nLabelWidthMM;
		nGapMM		= other.nGapMM;
		nMarginMM	= other.nMarginMM;
		nFontSize	= other.nFontSize;
		CodeType	= other.CodeType;
		nCodeHtMM	= other.nCodeHtMM;
		nNumPrints	= other.nNumPrints;
		strPrinterName = other.strPrinterName;
		bGetUserOKForPrinting = other.bGetUserOKForPrinting;
		bPrintLogo	= other.bPrintLogo;
		strLogoFile = other.strLogoFile;

		return *this;
	}

	STPrintParams(STPrintParams &other)
	{
		nLabelLengthMM = other.nLabelLengthMM;
		nLabelWidthMM = other.nLabelWidthMM;
		nGapMM		= other.nGapMM;
		nMarginMM	= other.nMarginMM;
		nFontSize	= other.nFontSize;
		CodeType	= other.CodeType;
		nCodeHtMM	= other.nCodeHtMM;
		nNumPrints	= other.nNumPrints;
		strPrinterName = other.strPrinterName;
		bGetUserOKForPrinting = other.bGetUserOKForPrinting;
		bPrintLogo	= other.bPrintLogo;
		strLogoFile = other.strLogoFile;
	}

}; // STPrintParams

// Freight Rates
struct STFreightParams
{
	double dSeaFreightRate;// USD per m3
	double dAirFreightRate;// USD per m3
	double dWeightFreightRate;// USD per Kg
	eFreightMode FreightMode;
	STFreightParams()
	{
		dSeaFreightRate = 300.0;
		dAirFreightRate = 13.0;
		dWeightFreightRate = 1.0;
		FreightMode = FREIGHT_SEA;
	}
	STFreightParams(STFreightParams &other)
	{
		dSeaFreightRate		= other.dSeaFreightRate;
		dAirFreightRate		= other.dAirFreightRate;
		dWeightFreightRate	= other.dWeightFreightRate;
		FreightMode			= other.FreightMode;
	}
	STFreightParams& operator = (STFreightParams& other)
	{
		dSeaFreightRate		= other.dSeaFreightRate;
		dAirFreightRate		= other.dAirFreightRate;
		dWeightFreightRate	= other.dWeightFreightRate;
		FreightMode			= other.FreightMode;
		
		return *this;
	}

}; // STFreightParams

struct STServerCommParams
{
	CString strURL1;// Web  addr 1
	CString strKey1;
	CString strURL2;// Web  addr 2
	CString strKey2;
	int nOperateMode;
	BOOL bSendDataToServer;

	STServerCommParams()
	{
		strURL1 = "http://47.114.126.132:9983/system/packing/data";
		strKey1 = "vbD#&!f&P&24$9x8XC*Q%Z^VJPiKNNg@";
		strURL2 = "";
		strKey2 = "";
		nOperateMode = 1;
		bSendDataToServer = TRUE;
	}
	STServerCommParams(STServerCommParams &other)
	{
		strURL1 = other.strURL1;
		strKey1 = other.strKey1;
		strURL2 = other.strURL2;
		strKey2 = other.strKey2;
		nOperateMode = other.nOperateMode;
		bSendDataToServer = other.bSendDataToServer;
	}
	STServerCommParams& operator = (STServerCommParams& other)
	{
		strURL1 = other.strURL1;
		strKey1 = other.strKey1;
		strURL2 = other.strURL2;
		strKey2 = other.strKey2;
		nOperateMode = other.nOperateMode;
		bSendDataToServer = other.bSendDataToServer;

		return *this;
	}
}; // SSTServerCommParams

#endif
