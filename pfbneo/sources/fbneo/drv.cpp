// Driver Init module

#include "c2dui.h"
#include "burner.h"
#include "retro_input.h"

using namespace c2dui;

extern UiMain *ui;

extern UINT8 NeoSystem;
int bDrvOkay = 0;
int kNetGame = 0;
int nIpsMaxFileLen = 0;

INT32 GetIpsesMaxLen(char *) { return 0; }

bool GetIpsDrvProtection() { return false; }

// replaces ips_manager.cpp
bool bDoIpsPatch = false;

void IpsApplyPatches(UINT8 *base, char *rom_name) {}

bool bRunPause;

// burner/state.cpp
bool bReplayReadOnly;
INT32 nReplayStatus = 0;
INT32 nReplayUndoCount = 0;
UINT32 nReplayCurrentFrame = 0;
UINT32 nStartFrame = 0;

INT32 FreezeInput(UINT8 **buf, INT32 *size) { return 0; }

INT32 UnfreezeInput(const UINT8 *buf, INT32 size) { return 0; }
// burner/state.cpp

bool is_netgame_or_recording() {
    return false;
}

static int ProgressCreate();

static UINT8 NeoSystemList[] = {
        0x13, // "Universe BIOS ver. 4.0"
        0x14, // "Universe BIOS ver. 3.3"
        0x15, // "Universe BIOS ver. 3.2"
        0x16, // "Universe BIOS ver. 3.1"
        0x00, // "MVS Asia/Europe ver. 6 (1 slot)"
        0x01, // "MVS Asia/Europe ver. 5 (1 slot)"
        0x02, // "MVS Asia/Europe ver. 3 (4 slot)"
        0x03, // "MVS USA ver. 5 (2 slot)"
        0x04, // "MVS USA ver. 5 (4 slot)"
        0x05, // "MVS USA ver. 5 (6 slot)"
        0x08, // "MVS Japan ver. 6 (? slot)"
        0x09, // "MVS Japan ver. 5 (? slot)"
        0x0a, // "MVS Japan ver. 6 (4 slot)"
        0x10, // "AES Asia"
        0x0f, // "AES Japan"
        0x0b, // "NEO-MVH MV1C (Asia)"
        0x0c, // "NEO-MVH MV1C (Japan)"
        0x12, // "Deck ver. 6 (Git Ver 1.3)"
        0x11, // "Development Kit"
};

static int DoLibInit() {
    int nRet;

    ProgressCreate();

    nRet = BzipOpen(false);
    printf("DoLibInit: BzipOpen = %i\n", nRet);
    if (nRet) {
        BzipClose();
        return 1;
    }

    NeoSystem = NeoSystemList[ui->getConfig()->get(Option::Id::ROM_NEOBIOS, true)->getIndex()];

    nRet = BurnDrvInit();
    printf("DoLibInit: BurnDrvInit = %i\n", nRet);

    BzipClose();

    if (nRet) {
        BurnDrvExit();
        return 1;
    } else {
        return 0;
    }
}

// Catch calls to BurnLoadRom() once the emulation has started;
// Intialise the zip module before forwarding the call, and exit cleanly.
static int DrvLoadRom(unsigned char *Dest, int *pnWrote, int i) {
    int nRet;

    BzipOpen(false);

    if ((nRet = BurnExtLoadRom(Dest, pnWrote, i)) != 0) {
        char szText[256];
        char *pszFilename;
        BurnDrvGetRomName(&pszFilename, i, 0);
        sprintf(szText, "Error loading %s for %s.\nEmulation will likely have problems.",
                pszFilename, BurnDrvGetTextA(DRV_NAME));
        printf("DrvLoadRom: %s\n", szText);
        ui->getUiMessageBox()->show("ERROR", szText, "OK");
    }

    BzipClose();

    BurnExtLoadRom = DrvLoadRom;

    return nRet;
}

int DrvInit(int nDrvNum, bool bRestore) {
    printf("DrvInit(%i, %i)\n", nDrvNum, bRestore);
    DrvExit();

    // set selected driver
    nBurnDrvSelect[0] = (UINT32) nDrvNum;
    // for retro_input
    bIsNeogeoCartGame = ((BurnDrvGetHardwareCode() & HARDWARE_PUBLIC_MASK) == HARDWARE_SNK_NEOGEO);
    // default input values
    nMaxPlayers = BurnDrvGetMaxPlayers();
    SetDefaultDeviceTypes();
    // init inputs
    InputInit();
    SetControllerInfo();

    printf("DrvInit: DoLibInit()\n");
    if (DoLibInit()) {                // Init the Burn library's driver
        //char szTemp[512];
        //_stprintf(szTemp, _T("Error starting '%s'.\n"), BurnDrvGetText(DRV_FULLNAME));
        //AppError(szTemp, 1);
        return 1;
    }

    printf("DrvInit: BurnExtLoadRom = DrvLoadRom\n");
    BurnExtLoadRom = DrvLoadRom;

    char path[1024];
    snprintf(path, 1023, "%s%s.fs", szAppEEPROMPath, BurnDrvGetTextA(DRV_NAME));
    BurnStateLoad(path, 0, nullptr);

    bDrvOkay = 1;
    nBurnLayer = 0xff;
    nSpriteEnable = 0xff;

    return 0;
}

// for uiStateMenu.cpp (BurnStateLoad)
int DrvInitCallback() {
    return DrvInit((int) nBurnDrvSelect[0], false);
}

int DrvExit() {
    if (bDrvOkay) {
        if (nBurnDrvSelect[0] < nBurnDrvCount) {
            char path[1024];
            snprintf(path, 1023, "%s%s.fs", szAppEEPROMPath, BurnDrvGetTextA(DRV_NAME));
            BurnStateSave(path, 0);
            InputExit();
            BurnDrvExit();
        }
    }

    BurnExtLoadRom = nullptr;
    bDrvOkay = 0;
    nBurnDrvSelect[0] = ~0U;

    return 0;
}

static double nProgressPosBurn = 0;

static int ProgressCreate() {
    nProgressPosBurn = 0;
    ui->getUiProgressBox()->setVisibility(c2d::Visibility::Visible);
    ui->getUiProgressBox()->setLayer(1000);
    return 0;
}

int ProgressUpdateBurner(double dProgress, const TCHAR *pszText, bool bAbs) {
    ui->getUiProgressBox()->setTitle(BurnDrvGetTextA(DRV_FULLNAME));

    if (pszText) {
        nProgressPosBurn += dProgress;
        ui->getUiProgressBox()->setMessage(pszText);
        ui->getUiProgressBox()->setProgress((float) nProgressPosBurn);
    } else {
        ui->getUiProgressBox()->setMessage("Please wait...");
    }

    ui->flip();

    return 0;
}

int AppError(TCHAR *szText, int bWarning) {
    //ui->getUiMessageBox()->show("ERROR", szText ? szText : "UNKNOW ERROR", "OK");
    return 1;
}

#ifdef __PFBN_NO_CONSOLES__

void nes_add_cheat(char *code) {};

void nes_remove_cheat(char *code) {};
#endif