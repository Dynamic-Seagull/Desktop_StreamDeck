
// StreameDeckDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "StreameDeck.h"
#include "StreameDeckDlg.h"
#include "SettingsDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
    CAboutDlg();

#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_ABOUTBOX };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX);

protected:
    DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CStreameDeckDlg 대화 상자

CStreameDeckDlg::CStreameDeckDlg(CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_STREAMEDECK_DIALOG, pParent)
    , m_hBmpBg(nullptr)
    , m_hBmpBtn(nullptr)
    , m_hBmpBtnClicked(nullptr)
    , m_hBmpSetting(nullptr)
    , m_hBmpSettingClicked(nullptr)
    , m_hBmpClose(nullptr)
    , m_hBmpCloseClicked(nullptr)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CStreameDeckDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CStreameDeckDlg, CDialogEx)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_WM_ERASEBKGND()
    ON_WM_CTLCOLOR()
    ON_WM_DRAWITEM()
    ON_WM_NCHITTEST()
    ON_WM_DESTROY()
    ON_CONTROL_RANGE(BN_CLICKED, IDC_BTN_GRID_BASE,
                     IDC_BTN_GRID_BASE + GRID_BTN_COUNT - 1, OnGridButton)
    ON_BN_CLICKED(IDC_BTN_SETTINGS, OnBtnSettings)
    ON_BN_CLICKED(IDC_BTN_CLOSE, OnBtnClose)
END_MESSAGE_MAP()


// CStreameDeckDlg 메시지 처리기

BOOL CStreameDeckDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // 캡션바 및 시스템 메뉴 제거
    ModifyStyle(WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, 0);

    // 배경 브러시 생성 (폴백용 다크 컬러)
    m_brBg.CreateSolidBrush(RGB(40, 40, 40));

    // 맑은 고딕 9pt 폰트 생성
    CDC* pDC = GetDC();
    int nFontH = -MulDiv(9, GetDeviceCaps(pDC->GetSafeHdc(), LOGPIXELSY), 72);
    ReleaseDC(pDC);
    LOGFONT lf = {};
    lf.lfHeight  = nFontH;
    lf.lfCharSet = HANGEUL_CHARSET;
    _tcscpy_s(lf.lfFaceName, _T("맑은 고딕"));
    m_font.CreateFontIndirect(&lf);
    SetFont(&m_font);

    // 창 크기를 클라이언트 영역 기준 500×300 픽셀로 고정 (캡션 제거 후 재계산)
    CRect rcWin(0, 0, 500, 300);
    AdjustWindowRectEx(&rcWin, GetStyle(), FALSE, GetExStyle());
    SetWindowPos(nullptr, 0, 0, rcWin.Width(), rcWin.Height(),
                 SWP_NOMOVE | SWP_NOZORDER | SWP_FRAMECHANGED);
    CenterWindow();

    // BMP 이미지 로드
    LoadImages();

    // 설정 불러오기 → 컨트롤 생성
    LoadConfig();
    CreateControls();

    return TRUE;
}

// ── BMP 이미지 경로 헬퍼 ─────────────────────────────────────────
CString CStreameDeckDlg::GetImgPath(LPCTSTR fileName)
{
    TCHAR exePath[MAX_PATH];
    GetModuleFileName(nullptr, exePath, MAX_PATH);
    CString path(exePath);
    int pos = path.ReverseFind(_T('\\'));
    if (pos >= 0)
        path = path.Left(pos + 1);
    path += _T("img\\");
    path += fileName;
    return path;
}

void CStreameDeckDlg::LoadImages()
{
    auto load = [&](LPCTSTR name) -> HBITMAP {
        return (HBITMAP)LoadImage(nullptr, GetImgPath(name),
                                  IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    };

    m_hBmpBg             = load(_T("img_bg.bmp"));
    m_hBmpBtn            = load(_T("img_btn.bmp"));
    m_hBmpBtnClicked     = load(_T("img_btn_clicked.bmp"));
    m_hBmpSetting        = load(_T("img_setting.bmp"));
    m_hBmpSettingClicked = load(_T("img_setting_clicked.bmp"));
    m_hBmpClose          = load(_T("img_close.bmp"));
    m_hBmpCloseClicked   = load(_T("img_close_clicked.bmp"));
}

void CStreameDeckDlg::CreateControls()
{
    // ── 레이아웃 상수 (픽셀 기준, 클라이언트 500×300) ────────────
    // 그리드 버튼: 86×65px, 간격 5px, 시작점 (25,30)
    const int BTN_W = 86, BTN_H = 65, BTN_GAP = 5;
    const int BTN_START_X = 25, BTN_START_Y = 30;

    // 설정 버튼: 우측 하단 (오른쪽 10px, 아래 8px 여백)
    const int SET_W = 80, SET_H = 28;
    const int SET_X = 500 - 10 - SET_W;   // = 410
    const int SET_Y = 300 - 8  - SET_H;   // = 264

    // 닫기 버튼: 좌측 하단 (왼쪽 10px, 아래 8px 여백)
    const int CLO_W = 28, CLO_H = 28;
    const int CLO_X = 10;
    const int CLO_Y = SET_Y;
    // ─────────────────────────────────────────────────────────────

    // 15개 버튼 (5열 × 3행)
    for (int i = 0; i < GRID_BTN_COUNT; i++)
    {
        int col = i % GRID_COLS;
        int row = i / GRID_COLS;
        int x   = BTN_START_X + col * (BTN_W + BTN_GAP);
        int y   = BTN_START_Y + row * (BTN_H + BTN_GAP);

        m_gridBtns[i].Create(
            m_configs[i].strCaption,
            WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
            CRect(x, y, x + BTN_W, y + BTN_H),
            this, IDC_BTN_GRID_BASE + i);
        m_gridBtns[i].SetFont(&m_font);
    }

    // 설정 버튼
    m_btnSettings.Create(_T(""),
                         WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
                         CRect(SET_X, SET_Y, SET_X + SET_W, SET_Y + SET_H),
                         this, IDC_BTN_SETTINGS);
    m_btnSettings.SetFont(&m_font);

    // 닫기 버튼
    m_btnClose.Create(_T(""),
                      WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
                      CRect(CLO_X, CLO_Y, CLO_X + CLO_W, CLO_Y + CLO_H),
                      this, IDC_BTN_CLOSE);
    m_btnClose.SetFont(&m_font);
}

void CStreameDeckDlg::UpdateButtonCaptions()
{
    for (int i = 0; i < GRID_BTN_COUNT; i++)
    {
        if (m_gridBtns[i].GetSafeHwnd())
            m_gridBtns[i].SetWindowText(m_configs[i].strCaption);
    }
}

// ── 그리드 버튼 클릭: 연결된 경로 열기 ──────────────────────────
void CStreameDeckDlg::OnGridButton(UINT nID)
{
    int idx = static_cast<int>(nID - IDC_BTN_GRID_BASE);
    if (idx < 0 || idx >= GRID_BTN_COUNT) return;

    const CString& path = m_configs[idx].strPath;
    if (path.IsEmpty())
    {
        return;
    }

    HINSTANCE hResult = ShellExecute(
        GetSafeHwnd(), _T("open"), path, nullptr, nullptr, SW_SHOWNORMAL);

    if (reinterpret_cast<INT_PTR>(hResult) <= 32)
    {
        CString msg;
        msg.Format(_T("파일 또는 폴더를 열 수 없습니다.\n경로: %s"), static_cast<LPCTSTR>(path));
        AfxMessageBox(msg, MB_OK | MB_ICONERROR);
    }
}

// ── 닫기 버튼 클릭: 창 종료 ──────────────────────────────────────
void CStreameDeckDlg::OnBtnClose()
{
    OnOK();
}

// ── 설정 버튼 클릭: 설정 다이얼로그 표시 ─────────────────────────
void CStreameDeckDlg::OnBtnSettings()
{
    CSettingsDlg dlg(m_configs, this);
    if (dlg.DoModal() == IDOK)
    {
        for (int i = 0; i < GRID_BTN_COUNT; i++)
            m_configs[i] = dlg.m_configs[i];

        UpdateButtonCaptions();
        SaveConfig();
    }
}

// ── 설정 저장/불러오기 (button_config.ini) ─────────────────────────
CString CStreameDeckDlg::GetConfigPath()
{
    TCHAR exePath[MAX_PATH];
    GetModuleFileName(nullptr, exePath, MAX_PATH);
    CString path(exePath);
    int pos = path.ReverseFind(_T('\\'));
    if (pos >= 0)
        path = path.Left(pos + 1);
    path += _T("button_config.ini");
    return path;
}

void CStreameDeckDlg::LoadConfig()
{
    CString iniPath = GetConfigPath();
    for (int i = 0; i < GRID_BTN_COUNT; i++)
    {
        CString section;
        section.Format(_T("Button%d"), i + 1);

        TCHAR buf[1024] = {};
        GetPrivateProfileString(section, _T("Caption"), _T(""), buf, 1024, iniPath);
        m_configs[i].strCaption = buf;

        GetPrivateProfileString(section, _T("Path"), _T(""), buf, 1024, iniPath);
        m_configs[i].strPath = buf;
    }
}

void CStreameDeckDlg::SaveConfig()
{
    CString iniPath = GetConfigPath();
    for (int i = 0; i < GRID_BTN_COUNT; i++)
    {
        CString section;
        section.Format(_T("Button%d"), i + 1);
        WritePrivateProfileString(section, _T("Caption"), m_configs[i].strCaption, iniPath);
        WritePrivateProfileString(section, _T("Path"),    m_configs[i].strPath,    iniPath);
    }
}

// ── 기존 MFC 핸들러 ───────────────────────────────────────────────
void CStreameDeckDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
    if ((nID & 0xFFF0) == IDM_ABOUTBOX)
    {
        CAboutDlg dlgAbout;
        dlgAbout.DoModal();
    }
    else
    {
        CDialogEx::OnSysCommand(nID, lParam);
    }
}

void CStreameDeckDlg::OnPaint()
{
    if (IsIconic())
    {
        CPaintDC dc(this);
        SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width()  - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;
        dc.DrawIcon(x, y, m_hIcon);
    }
    else
    {
        CDialogEx::OnPaint();
    }
}

HCURSOR CStreameDeckDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}

// ── 배경 BMP 그리기 ──────────────────────────────────────────────
BOOL CStreameDeckDlg::OnEraseBkgnd(CDC* pDC)
{
    CRect rc;
    GetClientRect(&rc);

    if (m_hBmpBg)
    {
        HDC hMemDC = ::CreateCompatibleDC(pDC->GetSafeHdc());
        HBITMAP hOld = (HBITMAP)::SelectObject(hMemDC, m_hBmpBg);
        BITMAP bm = {};
        ::GetObject(m_hBmpBg, sizeof(bm), &bm);
        ::StretchBlt(pDC->GetSafeHdc(),
                     0, 0, rc.Width(), rc.Height(),
                     hMemDC, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);
        ::SelectObject(hMemDC, hOld);
        ::DeleteDC(hMemDC);
    }
    else
    {
        // 이미지 로드 실패 시 단색 폴백
        pDC->FillSolidRect(&rc, RGB(40, 40, 40));
    }
    return TRUE;
}

// ── 클라이언트 영역 전체 드래그 이동 ────────────────────────────
LRESULT CStreameDeckDlg::OnNcHitTest(CPoint point)
{
    LRESULT hit = CDialogEx::OnNcHitTest(point);
    if (hit == HTCLIENT)
        return HTCAPTION;
    return hit;
}

// ── 배경/텍스트 색상 처리 ────────────────────────────────────────
HBRUSH CStreameDeckDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    if (nCtlColor == CTLCOLOR_DLG)
        return m_brBg;

    if (nCtlColor == CTLCOLOR_STATIC)
    {
        pDC->SetTextColor(RGB(255, 255, 255));
        pDC->SetBkMode(TRANSPARENT);
        return m_brBg;
    }

    return CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);
}

// ── 오너 드로우 버튼 렌더링 ─────────────────────────────────────
void CStreameDeckDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDIS)
{
    if (lpDIS->CtlType != ODT_BUTTON)
    {
        CDialogEx::OnDrawItem(nIDCtl, lpDIS);
        return;
    }

    bool bPressed = (lpDIS->itemState & ODS_SELECTED) != 0;

    // 버튼 종류에 따라 BMP 선택
    HBITMAP hBmp = nullptr;
    if (nIDCtl == IDC_BTN_SETTINGS)
        hBmp = bPressed ? m_hBmpSettingClicked : m_hBmpSetting;
    else if (nIDCtl == IDC_BTN_CLOSE)
        hBmp = bPressed ? m_hBmpCloseClicked : m_hBmpClose;
    else
        hBmp = bPressed ? m_hBmpBtnClicked : m_hBmpBtn;

    CDC dc;
    dc.Attach(lpDIS->hDC);
    CRect rc(lpDIS->rcItem);

    if (hBmp)
    {
        HDC hMemDC = ::CreateCompatibleDC(dc.GetSafeHdc());
        HBITMAP hOld = (HBITMAP)::SelectObject(hMemDC, hBmp);
        BITMAP bm = {};
        ::GetObject(hBmp, sizeof(bm), &bm);
        ::StretchBlt(dc.GetSafeHdc(),
                     rc.left, rc.top, rc.Width(), rc.Height(),
                     hMemDC, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);
        ::SelectObject(hMemDC, hOld);
        ::DeleteDC(hMemDC);
    }
    else
    {
        // BMP 로드 실패 시 폴백
        COLORREF clrBg = bPressed ? RGB(35, 35, 35) : RGB(50, 50, 50);
        CBrush brBg(clrBg);
        dc.FillRect(&rc, &brBg);
        CBrush brBorder(RGB(0, 0, 0));
        dc.FrameRect(&rc, &brBorder);
    }

    // 그리드 버튼에만 캡션 텍스트 출력
    if (nIDCtl >= IDC_BTN_GRID_BASE && nIDCtl < IDC_BTN_GRID_BASE + GRID_BTN_COUNT)
    {
        CString text;
        CWnd* pWnd = GetDlgItem(nIDCtl);
        if (pWnd) pWnd->GetWindowText(text);

        if (!text.IsEmpty())
        {
            dc.SetTextColor(RGB(255, 255, 255));
            dc.SetBkMode(TRANSPARENT);
            CFont* pOldFont = dc.SelectObject(&m_font);

            CRect rcText = rc;
            rcText.DeflateRect(2, 2);
            if (bPressed) rcText.OffsetRect(1, 1);

            dc.DrawText(text, &rcText, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_WORD_ELLIPSIS);

            if (pOldFont) dc.SelectObject(pOldFont);
        }
    }

    dc.Detach();
}

// ── 종료 시 BMP 핸들 해제 ────────────────────────────────────────
void CStreameDeckDlg::OnDestroy()
{
    CDialogEx::OnDestroy();

    if (m_hBmpBg)             { DeleteObject(m_hBmpBg);             m_hBmpBg = nullptr; }
    if (m_hBmpBtn)            { DeleteObject(m_hBmpBtn);            m_hBmpBtn = nullptr; }
    if (m_hBmpBtnClicked)     { DeleteObject(m_hBmpBtnClicked);     m_hBmpBtnClicked = nullptr; }
    if (m_hBmpSetting)        { DeleteObject(m_hBmpSetting);        m_hBmpSetting = nullptr; }
    if (m_hBmpSettingClicked) { DeleteObject(m_hBmpSettingClicked); m_hBmpSettingClicked = nullptr; }
    if (m_hBmpClose)          { DeleteObject(m_hBmpClose);          m_hBmpClose = nullptr; }
    if (m_hBmpCloseClicked)   { DeleteObject(m_hBmpCloseClicked);   m_hBmpCloseClicked = nullptr; }
}
