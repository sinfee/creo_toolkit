#include "./includes/main.h"
#include "./includes/ShowDirectory.h"
#include "./includes/OpenSamenameDrw.h"
#include "./includes/CommonFuns.h"
#include "./includes/CleanWorkDirectory.h"
#include "./includes/TimeSave.h"
#include "./includes/PaintColor.h"
#include "./includes/AlignSymDim.h"
#include "./includes/RenameMdl.h"
#include "./includes/ChangeWorkDirectory.h"
#include "./includes/PrtinAsmRename.h"
#include "./includes/BatOpenFiles.h"
#include "./includes/BatSnapshot.h"
#include "./includes/BattoPdf.h"
#include "./includes/BattoDwg.h"
#include "./includes/AutoDirSetting.h"
#include "./includes/cfg.h"
#include "./includes/FamInstExport.h"

char *LastRibbonTab = NULL;
ProPath *CurrentWorkDirectoryList;
HINT hint;

UserCheckBut check_but[1];

void ShowAboutDialog()
{

    if (hint == paintColor)
    {
        PaintColor_AfterMacro();
    }
    else
    {
        ShowMessageDialog(1, L"本项目遵循BSD 3-clause许可证发布。\n请不要放到CSDN等处收费下载。\n访问项目主页获得更多信息：\nhttps://github.com/slacker-HD/creo_toolkit/tree/master/CreoTool");
    }
    hint = About;
}

static uiCmdAccessState AccessDefault(uiCmdAccessMode access_mode)
{
    return ACCESS_AVAILABLE;
}

static uiCmdAccessState AccessPRTorASM(uiCmdAccessMode access_mode)
{
    if (CurrentMdlType() == PRO_ASSEMBLY || CurrentMdlType() == PRO_PART)
        return ACCESS_AVAILABLE;
    else
        return ACCESS_INVISIBLE;
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

static uiCmdAccessState AccessPRT(uiCmdAccessMode access_mode)
{
    if (CurrentMdlType() == PRO_PART)
        return ACCESS_AVAILABLE;
    else
        return ACCESS_INVISIBLE;
}

int user_initialize()
{
    ProError status;
    uiCmdCmdId IMI_ShowDirectoryID;
    uiCmdCmdId IMI_ShowWorkDirmenuID;
    uiCmdCmdId IMI_ChangeWorkDirectorymenuID;
    uiCmdCmdId IMI_OpenSamenameDrwmenuID;
    uiCmdCmdId IMI_VerticalAlignmenuID;
    uiCmdCmdId IMI_HorizonAlignmenuID;
    uiCmdCmdId IMI_PurgeWorkDirmenuID;
    uiCmdCmdId IMI_TimeSavemenuID;
    uiCmdCmdId IMI_AboutID;
    uiCmdCmdId IMI_PaintColormenuID;
    uiCmdCmdId IMI_ClearColormenuID;
    uiCmdCmdId IMI_MdlRenamemenuID;

    uiCmdCmdId IMI_BatOPenPrtmenuID;
    uiCmdCmdId IMI_BatOPenDrwmenuID;

    uiCmdCmdId IMI_BatSnapShotmenuID;
    uiCmdCmdId IMI_BatExportPdfmenuID;
    uiCmdCmdId IMI_BatExportDwgmenuID;

    uiCmdCmdId IMI_ExportFamInstsID;

    ProPath currentPath;
    ProPath exePath;
    ProPath cfgPath;
    int n_size;

    wchar_t ifMemPath[256] = L"True";
    wchar_t lastPath[256] = L"True";
    int valueLength;
    int compResult;

    status = ProMenubarMenuAdd("IMI_Mainmenu", "IMI_Mainmenu", "About", PRO_B_TRUE, MSGFILE);
    status = ProMenubarmenuMenuAdd("IMI_Mainmenu", "IMI_Mainmenu", "IMI_Mainmenu", NULL, PRO_B_TRUE, MSGFILE);

    status = ProMenubarmenuMenuAdd("IMI_Mainmenu", "IMI_DirToolsubmenu", "IMI_DirToolsubmenu", NULL, PRO_B_TRUE, MSGFILE);

    status = ProCmdActionAdd("IMI_ShowDirectory_Act", (uiCmdCmdActFn)ShowDirectory, uiProeImmediate, AccessDefault, PRO_B_TRUE, PRO_B_TRUE, &IMI_ShowDirectoryID);
    status = ProMenubarmenuPushbuttonAdd("IMI_DirToolsubmenu", "IMI_ShowDirmenu", "IMI_ShowDirmenu", "IMI_ShowDirmenutips", NULL, PRO_B_TRUE, IMI_ShowDirectoryID, MSGFILE);

    status = ProCmdActionAdd("IMI_ShowWorkDirectory_Act", (uiCmdCmdActFn)ShowWorkDirectory, uiProeImmediate, AccessDefault, PRO_B_TRUE, PRO_B_TRUE, &IMI_ShowWorkDirmenuID);
    status = ProMenubarmenuPushbuttonAdd("IMI_DirToolsubmenu", "IMI_ShowWorkDirmenu", "IMI_ShowWorkDirmenu", "IMI_ShowWorkDirmenutips", NULL, PRO_B_TRUE, IMI_ShowWorkDirmenuID, MSGFILE);

    status = ProCmdActionAdd("IMI_ChangeWorkDirectory_Act", (uiCmdCmdActFn)ShowChangeWorkDirectoryDialog, uiProeImmediate, AccessDefault, PRO_B_TRUE, PRO_B_TRUE, &IMI_MdlRenamemenuID);
    status = ProMenubarmenuPushbuttonAdd("IMI_DirToolsubmenu", "IMI_ChangeWorkDirectorymenu", "IMI_ChangeWorkDirectorymenu", "IMI_ChangeWorkDirectorymenutips", NULL, PRO_B_TRUE, IMI_MdlRenamemenuID, MSGFILE);

    status = ProMenubarmenuMenuAdd("IMI_Mainmenu", "IMI_DirDRWsubmenu", "IMI_DirDRWsubmenu", NULL, PRO_B_TRUE, MSGFILE);

    status = ProCmdActionAdd("IMI_OpenSamenameDrw_Act", (uiCmdCmdActFn)OpenSamenameDrw, uiProeImmediate, AccessPRTorASM, PRO_B_TRUE, PRO_B_TRUE, &IMI_OpenSamenameDrwmenuID);
    status = ProMenubarmenuPushbuttonAdd("IMI_DirDRWsubmenu", "IMI_OpenSamenameDrwmenu", "IMI_OpenSamenameDrwmenu", "IMI_OpenSamenameDrwmenutips", NULL, PRO_B_TRUE, IMI_OpenSamenameDrwmenuID, MSGFILE);

    status = ProCmdActionAdd("IMI_VerticalAlign_Act", (uiCmdCmdActFn)VerticalAlign, uiProeImmediate, AccessDRW, PRO_B_TRUE, PRO_B_TRUE, &IMI_VerticalAlignmenuID);
    status = ProMenubarmenuPushbuttonAdd("IMI_DirDRWsubmenu", "IMI_VerticalAlignmenu", "IMI_VerticalAlignmenu", "IMI_VerticalAlignmenutips", NULL, PRO_B_TRUE, IMI_VerticalAlignmenuID, MSGFILE);

    status = ProCmdActionAdd("IMI_HorizonAlign_Act", (uiCmdCmdActFn)HorizonAlign, uiProeImmediate, AccessDRW, PRO_B_TRUE, PRO_B_TRUE, &IMI_HorizonAlignmenuID);
    status = ProMenubarmenuPushbuttonAdd("IMI_DirDRWsubmenu", "IMI_HorizonAlignmenu", "IMI_HorizonAlignmenu", "IMI_HorizonAlignmenutips", NULL, PRO_B_TRUE, IMI_HorizonAlignmenuID, MSGFILE);

    status = ProMenubarmenuMenuAdd("IMI_Mainmenu", "IMI_Filesubmenu", "IMI_Filesubmenu", NULL, PRO_B_TRUE, MSGFILE);

    status = ProCmdActionAdd("IMI_PurgeWorkDir_Act", (uiCmdCmdActFn)ProMdlPurgeAll, uiProeImmediate, AccessDefault, PRO_B_TRUE, PRO_B_TRUE, &IMI_PurgeWorkDirmenuID);
    status = ProMenubarmenuPushbuttonAdd("IMI_Filesubmenu", "IMI_PurgeWorkDirmenu", "IMI_PurgeWorkDirmenu", "IMI_PurgeWorkDirmenutips", NULL, PRO_B_TRUE, IMI_PurgeWorkDirmenuID, MSGFILE);

    status = ProCmdActionAdd("IMI_MdlRenamemenu_Act", (uiCmdCmdActFn)RenamePrtandDrw, uiProeImmediate, AccessPRTorASM, PRO_B_TRUE, PRO_B_TRUE, &IMI_MdlRenamemenuID);
    status = ProMenubarmenuPushbuttonAdd("IMI_Filesubmenu", "IMI_MdlRenamemenu", "IMI_MdlRenamemenu", "IMI_MdlRenamemenutips", NULL, PRO_B_TRUE, IMI_MdlRenamemenuID, MSGFILE);

    status = ProCmdActionAdd("IMI_TimeSave_Act", (uiCmdCmdActFn)ShowTimeSaveDialog, uiProeImmediate, AccessDefault, PRO_B_TRUE, PRO_B_TRUE, &IMI_TimeSavemenuID);
    status = ProMenubarmenuPushbuttonAdd("IMI_Filesubmenu", "IMI_TimeSavemenu", "IMI_TimeSavemenu", "IMI_TimeSavemenutips", NULL, PRO_B_TRUE, IMI_TimeSavemenuID, MSGFILE);

    status = ProCmdOptionAdd("IMI_AutoDirSettingChkMenu_Act", (uiCmdCmdActFn)AutoDirSettingFn, PRO_B_TRUE, (uiCmdCmdValFn)AutoDirSettingValueFn, AccessDefault, PRO_B_TRUE, PRO_B_TRUE, &(check_but[0].command));
    status = ProMenubarmenuChkbuttonAdd("IMI_Filesubmenu", "IMI_AutoDirSettingChkMenu", "IMI_AutoDirSettingChkMenu", "IMI_AutoDirSettingChkMenu", "IMI_TimeSave_Act", PRO_B_TRUE, check_but[0].command, MSGFILE);

    status = ProMenubarmenuMenuAdd("IMI_Mainmenu", "IMI_PaintColorsubmenu", "IMI_PaintColorsubmenu", NULL, PRO_B_TRUE, MSGFILE);

    status = ProCmdActionAdd("IMI_PaintColor_Act", (uiCmdCmdActFn)PaintColor, uiProeImmediate, AccessASM, PRO_B_TRUE, PRO_B_TRUE, &IMI_AboutID);
    status = ProMenubarmenuPushbuttonAdd("IMI_PaintColorsubmenu", "IMI_PaintColormenu", "IMI_PaintColormenu", "IMI_PaintColormenutips", NULL, PRO_B_TRUE, IMI_AboutID, MSGFILE);

    status = ProCmdActionAdd("IMI_ClearColor_Act", (uiCmdCmdActFn)ClearColor, uiProeImmediate, AccessASM, PRO_B_TRUE, PRO_B_TRUE, &IMI_PaintColormenuID);
    status = ProMenubarmenuPushbuttonAdd("IMI_PaintColorsubmenu", "IMI_ClearColormenu", "IMI_ClearColormenu", "IMI_ClearColormenutips", NULL, PRO_B_TRUE, IMI_PaintColormenuID, MSGFILE);

    status = ProCmdActionAdd("IMI_About_Act", (uiCmdCmdActFn)ShowAboutDialog, uiProeImmediate, AccessDefault, PRO_B_TRUE, PRO_B_TRUE, &IMI_PaintColormenuID);
    status = ProMenubarmenuPushbuttonAdd("IMI_Mainmenu", "IMI_Aboutmenu", "IMI_Aboutmenu", "IMI_Aboutmenutips", NULL, PRO_B_TRUE, IMI_PaintColormenuID, MSGFILE);

    status = ProMenubarmenuMenuAdd("IMI_Mainmenu", "IMI_BatOPensubmenu", "IMI_BatOPensubmenu", NULL, PRO_B_TRUE, MSGFILE);

    status = ProCmdActionAdd("IMI_BatOPenPrt_Act", (uiCmdCmdActFn)OpenPrts, uiProeImmediate, AccessDefault, PRO_B_TRUE, PRO_B_TRUE, &IMI_BatOPenPrtmenuID);
    status = ProMenubarmenuPushbuttonAdd("IMI_BatOPensubmenu", "IMI_BatOPenPrtmenu", "IMI_BatOPenPrtmenu", "IMI_BatOPenPrtmenutips", NULL, PRO_B_TRUE, IMI_BatOPenPrtmenuID, MSGFILE);

    status = ProCmdActionAdd("IMI_BatOPenDrw_Act", (uiCmdCmdActFn)OpenDrws, uiProeImmediate, AccessDefault, PRO_B_TRUE, PRO_B_TRUE, &IMI_BatOPenDrwmenuID);
    status = ProMenubarmenuPushbuttonAdd("IMI_BatOPensubmenu", "IMI_BatOPenDrwmenu", "IMI_BatOPenDrwmenu", "IMI_BatOPenDrwmenutips", NULL, PRO_B_TRUE, IMI_BatOPenDrwmenuID, MSGFILE);

    status = ProMenubarmenuMenuAdd("IMI_Mainmenu", "IMI_BatExportmenu", "IMI_BatExportmenu", NULL, PRO_B_TRUE, MSGFILE);

    status = ProCmdActionAdd("IMI_BatSnapShot_Act", (uiCmdCmdActFn)BatSnapShot, uiProeImmediate, AccessDefault, PRO_B_TRUE, PRO_B_TRUE, &IMI_BatSnapShotmenuID);
    status = ProMenubarmenuPushbuttonAdd("IMI_BatExportmenu", "IMI_BatSnapShotmenu", "IMI_BatSnapShotmenu", "IMI_BatSnapShotmenutips", NULL, PRO_B_TRUE, IMI_BatSnapShotmenuID, MSGFILE);

    status = ProCmdActionAdd("IMI_BatExportPdf_Act", (uiCmdCmdActFn)BatToPdf, uiProeImmediate, AccessDefault, PRO_B_TRUE, PRO_B_TRUE, &IMI_BatExportPdfmenuID);
    status = ProMenubarmenuPushbuttonAdd("IMI_BatExportmenu", "IMI_BatExportPdfmenu", "IMI_BatExportPdfmenu", "IMI_BatExportPdfmenutips", NULL, PRO_B_TRUE, IMI_BatExportPdfmenuID, MSGFILE);

    status = ProCmdActionAdd("IMI_BatExportDwg_Act", (uiCmdCmdActFn)BatToDwg, uiProeImmediate, AccessDefault, PRO_B_TRUE, PRO_B_TRUE, &IMI_BatExportDwgmenuID);
    status = ProMenubarmenuPushbuttonAdd("IMI_BatExportmenu", "IMI_BatExportDwgmenu", "IMI_BatExportDwgmenu", "IMI_BatExportDwgmenutips", NULL, PRO_B_TRUE, IMI_BatExportDwgmenuID, MSGFILE);

    status = ProCmdActionAdd("IMI_ExportFamInsts_Act", (uiCmdCmdActFn)ExportFamInsts, uiProeImmediate, AccessPRT, PRO_B_TRUE, PRO_B_TRUE, &IMI_ExportFamInstsID);
    status = ProMenubarmenuPushbuttonAdd("IMI_BatExportmenu", "IMI_ExportFamInstsmenu", "IMI_ExportFamInstsmenu", "IMI_ExportFamInstsmenutips", NULL, PRO_B_TRUE, IMI_ExportFamInstsID, MSGFILE);

    status = AsmTreePrtinAsmRenamePopupmenusSetup();

    status = ProNotificationSet(PRO_RIBBON_TAB_SWITCH, (ProFunction)ProRibbonTabSwitchNotification);
    status = ProNotificationSet(PRO_DIRECTORY_CHANGE_POST, (ProFunction)ProDirectoryChangeNotification);

    status = ProToolkitApplTextPathGet(exePath);
    status = ProWstringCopy(exePath, cfgPath, PRO_VALUE_UNUSED);
    status = ProWstringConcatenate(L"\\CreoTool.ini", cfgPath, PRO_VALUE_UNUSED);

    status = ProDirectoryCurrentGet(currentPath);

    if (ReadConfig(cfgPath, L"LastPath", currentPath, &valueLength) != 0)
    {
        WriteOrUpdateConfig(cfgPath, L"LastPath", currentPath);
    }

    if (ReadConfig(cfgPath, L"AutoChangePath", ifMemPath, &valueLength) != 0)
    {
        WriteOrUpdateConfig(cfgPath, L"AutoChangePath", L"False");
    }

    status = ProWstringCompare(L"True", ifMemPath, PRO_VALUE_UNUSED, &compResult);
    if (compResult == 0)
    {
        check_but[0].state = PRO_B_TRUE;
        status = ProDirectoryChange(currentPath);
    }
    else
    {
        check_but[0].state = PRO_B_FALSE;
    }

    status = ProDirectoryCurrentGet(currentPath);
    status = ProWstringLengthGet(currentPath, &n_size);
    currentPath[n_size - 1] = '\0';
    status = ProArrayAlloc(1, sizeof(ProPath), 1, (ProArray *)&CurrentWorkDirectoryList);
    status = ProWstringCopy(currentPath, CurrentWorkDirectoryList[0], PRO_VALUE_UNUSED);
    hint = About;

    return PRO_TK_NO_ERROR;
}

void user_terminate()
{
    ProError status;
    ProPath currentPath;
    ProPath exePath;
    ProPath cfgPath;
    if (LastRibbonTab != NULL)
        status = ProStringFree(LastRibbonTab);
    status = ProNotificationUnset(PRO_RIBBON_TAB_SWITCH);
    status = ProNotificationUnset(PRO_DIRECTORY_CHANGE_POST);
    status = ProArrayFree((ProArray *)&CurrentWorkDirectoryList);

    status = ProToolkitApplTextPathGet(exePath);
    status = ProWstringCopy(exePath, cfgPath, PRO_VALUE_UNUSED);
    status = ProWstringConcatenate(L"\\CreoTool.ini", cfgPath, PRO_VALUE_UNUSED);
    status = ProDirectoryCurrentGet(currentPath);
    if (status == PRO_TK_NO_ERROR)
    {
        WriteOrUpdateConfig(cfgPath, L"LastPath", currentPath);
    }
}
