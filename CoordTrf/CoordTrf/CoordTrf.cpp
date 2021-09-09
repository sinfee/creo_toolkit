// CoordTrf.cpp : 定义 DLL 的初始化例程。
//

#include "stdafx.h"
#include "CoordTrf.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//TODO: 如果此 DLL 相对于 MFC DLL 是动态链接的，
//		则从此 DLL 导出的任何调入
//		MFC 的函数必须将 AFX_MANAGE_STATE 宏添加到
//		该函数的最前面。
//
//		例如:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// 此处为普通函数体
//		}
//
//		此宏先于任何 MFC 调用
//		出现在每个函数中十分重要。这意味着
//		它必须作为函数中的第一个语句
//		出现，甚至先于所有对象变量声明，
//		这是因为它们的构造函数可能生成 MFC
//		DLL 调用。
//
//		有关其他详细信息，
//		请参阅 MFC 技术说明 33 和 58。
//

// CCoordTrfApp

BEGIN_MESSAGE_MAP(CCoordTrfApp, CWinApp)
END_MESSAGE_MAP()


// CCoordTrfApp 构造

CCoordTrfApp::CCoordTrfApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CCoordTrfApp 对象

CCoordTrfApp theApp;


// CCoordTrfApp 初始化

BOOL CCoordTrfApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}



int CurrentMdlType()
{
	ProMdl mdl;
	ProMdlType mdltype;
	ProError status;
	status = ProMdlCurrentGet(&mdl);
	if (status != PRO_TK_NO_ERROR)
		return -1;
	status = ProMdlTypeGet(mdl, &mdltype);
	if (status != PRO_TK_NO_ERROR)
		return -1;
	else
		return mdltype;
}

static uiCmdAccessState AccessASM(uiCmdAccessMode access_mode)
{
	if (CurrentMdlType() == PRO_ASSEMBLY)
		return ACCESS_AVAILABLE;
	else
		return ACCESS_INVISIBLE;
}

static uiCmdAccessState AccessDRW(uiCmdAccessMode access_mode)
{
	if (CurrentMdlType() == PRO_DRAWING)
		return ACCESS_AVAILABLE;
	else
		return ACCESS_INVISIBLE;
}

void CoordComponentToAsm()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	ProError status;
	ProPoint3d pointCompCoord,pointAsmCoord;
	ProSelection *sel;
	int n_sel;
	ProMatrix transformation;
	ProAsmcomppath comppath;
	CString message;
	AfxMessageBox(_T("选择一个点查看其在组件及装配体中的坐标，注意选取时请在视图中而不是元素树中选择点特征，否则数据会不正确。"));
	status = ProSelect("point", 1, NULL, NULL, NULL, NULL, &sel, &n_sel);
	if (status == PRO_TK_NO_ERROR)
	{
		status = ProSelectionPoint3dGet(sel[0], pointCompCoord);
		status = ProSelectionAsmcomppathGet(sel[0], &comppath);
		status = ProAsmcomppathTrfGet(&comppath,PRO_B_TRUE,transformation);
		status = ProPntTrfEval(pointCompCoord,transformation,pointAsmCoord);

		message.Format("点在组件默认坐标系下的坐标为(%f,%f,%f)，\n在装配体默认坐标系下的坐标为(%f,%f,%f)",pointCompCoord[0],pointCompCoord[1],pointCompCoord[2],pointAsmCoord[0],pointAsmCoord[1],pointAsmCoord[2]);
		AfxMessageBox(message);
	}
}

void CoordViewtoDrawing()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	ProError status;
	ProPoint3d pointDrawingCoord,pointViewCoord;
	ProView view;
	ProSelection *sel;
	int n_sel;
	ProMatrix transViewtoDrawing;
	ProMdl mdl;
	CString message;
	pointViewCoord[0] = 0;
	pointViewCoord[1] = 0;
	pointViewCoord[2] = 0;
	AfxMessageBox(_T("选择一个视图查看视图原点坐标转换为绘图的坐标。"));
	status = ProSelect((char *)"dwg_view", 1, NULL, NULL, NULL, NULL, &sel, &n_sel);
	if (status == PRO_TK_NO_ERROR)
	{
		status = ProSelectionViewGet(sel[0],&view);
		status = ProMdlCurrentGet(&mdl);
		status = ProDrawingViewTransformGet(ProDrawing(mdl),view,PRO_B_TRUE,transViewtoDrawing);
		status = ProPntTrfEval(pointViewCoord,transViewtoDrawing,pointDrawingCoord);
		message.Format("视图原点坐标在绘图的坐标系下坐标为(%f,%f,%f)",pointDrawingCoord[0],pointDrawingCoord[1],pointDrawingCoord[2]);
		AfxMessageBox(message);
	}
}

void CoordsolidtoDrawing()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	ProError status;
	ProPoint3d pointCompCoord,pointAsmCoord,pointDrawingCoord,pointViewCoord;
	ProAsmcomppath comppath;
	ProView view;
	ProSelection *sel;
	int n_sel;
	ProMdl mdl;
	ProSolid solid;
	CString message;
	ProMatrix transComptoAsm,transAsmtoDrawing,transDrawingtoView;
	AfxMessageBox(_T("选择一个点查看其在组件、装配体及所选视图下的中的坐标"));
	status = ProSelect((char *)"point", 1, NULL, NULL, NULL, NULL, &sel, &n_sel);
	if (status == PRO_TK_NO_ERROR)
	{
		status = ProSelectionPoint3dGet(sel[0], pointCompCoord);
		status = ProSelectionAsmcomppathGet(sel[0], &comppath);
		status = ProAsmcomppathTrfGet(&comppath,PRO_B_TRUE,transComptoAsm);
		status = ProPntTrfEval(pointCompCoord,transComptoAsm,pointAsmCoord);

		status = ProMdlCurrentGet(&mdl);
		status = ProDrawingCurrentsolidGet(ProDrawing(mdl),&solid);
		status = ProSelectionViewGet(sel[0],&view);
		status = ProViewMatrixGet(ProMdl(solid),view,transAsmtoDrawing);
		status = ProPntTrfEval(pointAsmCoord,transAsmtoDrawing,pointDrawingCoord);


		status = ProDrawingViewTransformGet(ProDrawing(mdl),view,PRO_B_FALSE,transDrawingtoView);
		status = ProPntTrfEval(pointDrawingCoord,transDrawingtoView,pointViewCoord);

		message.Format("点在组件默认坐标系下的坐标为(%f,%f,%f)，\n在装配体默认坐标系下的坐标为(%f,%f,%f)\n，在所选点所在视图下的坐标为(%f,%f)",pointCompCoord[0],pointCompCoord[1],pointCompCoord[2],pointAsmCoord[0],pointAsmCoord[1],pointAsmCoord[2],pointViewCoord[0],pointViewCoord[1]);

		AfxMessageBox(message);

	}
}
extern "C" int user_initialize()
{
	ProError status;
	uiCmdCmdId ComponentToAsmID,ViewtoDrawingID, solidtoDrawingID;

	status = ProMenubarMenuAdd("CoordTrf", "CoordTrf", "About", PRO_B_TRUE, MSGFILE);
	status = ProMenubarmenuMenuAdd("CoordTrf", "CoordTrf", "CoordTrf", NULL, PRO_B_TRUE, MSGFILE);

	status = ProCmdActionAdd("ComponentToAsm_Act", (uiCmdCmdActFn)CoordComponentToAsm, uiProeImmediate, AccessASM, PRO_B_TRUE, PRO_B_TRUE, &ComponentToAsmID);
	status = ProMenubarmenuPushbuttonAdd("CoordTrf", "ComponentToAsm", "ComponentToAsm", "ComponentToAsmtips", NULL, PRO_B_TRUE, ComponentToAsmID, MSGFILE);

	status = ProCmdActionAdd("ViewtoDrawing_Act", (uiCmdCmdActFn)CoordViewtoDrawing, uiProeImmediate, AccessDRW, PRO_B_TRUE, PRO_B_TRUE, &ViewtoDrawingID);
	status = ProMenubarmenuPushbuttonAdd("CoordTrf", "ViewtoDrawing", "ViewtoDrawing", "ViewtoDrawingtips", NULL, PRO_B_TRUE, ViewtoDrawingID, MSGFILE);

	status = ProCmdActionAdd("solidtoDrawing_Act", (uiCmdCmdActFn)CoordsolidtoDrawing, uiProeImmediate, AccessDRW, PRO_B_TRUE, PRO_B_TRUE, &solidtoDrawingID);
	status = ProMenubarmenuPushbuttonAdd("CoordTrf", "solidtoDrawing", "solidtoDrawing", "solidtoDrawingtips", NULL, PRO_B_TRUE, solidtoDrawingID, MSGFILE);
	
	return PRO_TK_NO_ERROR;
}

extern "C" void user_terminate()
{
}
