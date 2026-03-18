
// SettingsDlg.cpp: 버튼 설정 다이얼로그 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "StreameDeck.h"
#include "SettingsDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#pragma comment(lib, "comctl32.lib")


CSettingsDlg::CSettingsDlg(ButtonConfig* pConfigs, CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_SETTINGS_DIALOG, pParent)
    , m_nSelected(-1)
{
    for (int i = 0; i < GRID_BTN_COUNT; i++)
        m_configs[i] = pConfigs[i];
}

void CSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CSettingsDlg, CDialogEx)
    ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_BUTTONS, OnListSelChange)
    ON_BN_CLICKED(IDC_BTN_BROWSE, OnBtnBrowse)
    ON_BN_CLICKED(IDC_BTN_APPLY,  OnBtnApply)
END_MESSAGE_MAP()


BOOL CSettingsDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    SetWindowText(_T("버튼 설정"));

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

    // 클라이언트 영역 560×450 픽셀로 조정
    CRect rcWin(0, 0, 560, 450);
    AdjustWindowRectEx(&rcWin, GetStyle(), FALSE, GetExStyle());
    SetWindowPos(nullptr, 0, 0, rcWin.Width(), rcWin.Height(),
                 SWP_NOMOVE | SWP_NOZORDER);
    CenterWindow();

    CreateControls();
    PopulateList();

    // 적용 버튼은 목록 선택 전까지 비활성
    m_btnApply.EnableWindow(FALSE);

    return TRUE;
}

void CSettingsDlg::CreateControls()
{
    CFont* pFont = &m_font;

    // ── 레이아웃 상수 (클라이언트 560×450) ───────────────────────
    //  상단: ListView  (10,10) ~ (550,225)   →  540 × 215
    //  하단: 편집영역  (10,235) ~ (550,400)  →  540 × 165
    //  최하: 확인/취소 (10,412) ~ (550,440)  →  540 × 28
    // ─────────────────────────────────────────────────────────────

    // ── 상단: 버튼 목록 (ListView, 전폭) ─────────────────────────
    m_listBtns.Create(
        WS_CHILD | WS_VISIBLE | WS_BORDER |
        LVS_REPORT | LVS_SINGLESEL | LVS_SHOWSELALWAYS,
        CRect(10, 10, 550, 225), this, IDC_LIST_BUTTONS);
    m_listBtns.SetFont(pFont);
    m_listBtns.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

    // Windows는 0번 열을 항상 좌정렬로 고정하므로,
    // 더미 열을 0번에 먼저 삽입 후 삭제하여 "번호" 열을 가운데 정렬로 적용
    m_listBtns.InsertColumn(0, _T(""),     LVCFMT_LEFT,     0);   // 더미
    m_listBtns.InsertColumn(1, _T("번호"), LVCFMT_CENTER,  38);
    m_listBtns.InsertColumn(2, _T("캡션"), LVCFMT_LEFT,   110);
    m_listBtns.InsertColumn(3, _T("경로"), LVCFMT_LEFT,   370);
    m_listBtns.DeleteColumn(0);   // 더미 제거 → "번호"가 0번이 되지만 CENTER 유지


    // ── 하단: 그룹박스 + 편집 영역 ───────────────────────────────
    m_groupConfig.Create(_T("선택 버튼 설정"),
                         WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
                         CRect(10, 233, 550, 400), this, -1);
    m_groupConfig.SetFont(pFont);

    // 캡션 행 (y=262)
    m_lblCaption.Create(_T("캡션:"),
                        WS_CHILD | WS_VISIBLE,
                        CRect(20, 262, 60, 280), this, -1);
    m_lblCaption.SetFont(pFont);

    m_editCaption.Create(WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
                         CRect(65, 260, 540, 282), this, IDC_EDIT_CAPTION);
    m_editCaption.SetFont(pFont);

    // 경로 행 (y=296)
    m_lblPath.Create(_T("경로:"),
                     WS_CHILD | WS_VISIBLE,
                     CRect(20, 298, 60, 316), this, -1);
    m_lblPath.SetFont(pFont);

    m_editPath.Create(WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
                      CRect(65, 296, 455, 318), this, IDC_EDIT_PATH);
    m_editPath.SetFont(pFont);

    m_btnBrowse.Create(_T("찾아보기..."),
                       WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                       CRect(460, 295, 540, 319), this, IDC_BTN_BROWSE);
    m_btnBrowse.SetFont(pFont);

    // 적용 버튼
    m_btnApply.Create(_T("적 용"),
                      WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                      CRect(420, 358, 540, 386), this, IDC_BTN_APPLY);
    m_btnApply.SetFont(pFont);

    // 확인 / 취소 버튼
    m_btnOK.Create(_T("확 인"),
                   WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
                   CRect(355, 412, 445, 440), this, IDOK);
    m_btnOK.SetFont(pFont);

    m_btnCancel.Create(_T("취 소"),
                       WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                       CRect(450, 412, 540, 440), this, IDCANCEL);
    m_btnCancel.SetFont(pFont);
}

void CSettingsDlg::PopulateList()
{
    m_listBtns.DeleteAllItems();
    for (int i = 0; i < GRID_BTN_COUNT; i++)
    {
        CString no;
        no.Format(_T("%d"), i + 1);
        int idx = m_listBtns.InsertItem(i, no);
        m_listBtns.SetItemText(idx, 1, m_configs[i].strCaption);
        m_listBtns.SetItemText(idx, 2, m_configs[i].strPath);
    }
}

void CSettingsDlg::UpdateEditFields(int nIdx)
{
    if (nIdx < 0 || nIdx >= GRID_BTN_COUNT) return;
    m_editCaption.SetWindowText(m_configs[nIdx].strCaption);
    m_editPath.SetWindowText(m_configs[nIdx].strPath);
}

void CSettingsDlg::RefreshListItem(int nIdx)
{
    if (nIdx < 0 || nIdx >= GRID_BTN_COUNT) return;
    m_listBtns.SetItemText(nIdx, 1, m_configs[nIdx].strCaption);
    m_listBtns.SetItemText(nIdx, 2, m_configs[nIdx].strPath);
}

// ── 목록 선택 변경 ────────────────────────────────────────────────
void CSettingsDlg::OnListSelChange(NMHDR* pNMHDR, LRESULT* pResult)
{
    LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
    // 새로 선택된 항목에만 반응 (해제 이벤트 무시)
    if ((pNMLV->uNewState & LVIS_SELECTED) && !(pNMLV->uOldState & LVIS_SELECTED))
    {
        m_nSelected = pNMLV->iItem;
        UpdateEditFields(m_nSelected);
        m_btnApply.EnableWindow(TRUE);
    }
    *pResult = 0;
}

// ── 찾아보기: 파일 / 폴더 / 웹사이트 선택 ───────────────────────
void CSettingsDlg::OnBtnBrowse()
{
    const int ID_FILE    = 100;
    const int ID_FOLDER  = 101;
    const int ID_WEBSITE = 102;

    TASKDIALOG_BUTTON aButtons[] = {
        { ID_FILE,    L"파일"    },
        { ID_FOLDER,  L"폴더"    },
        { ID_WEBSITE, L"웹사이트" }
    };

    TASKDIALOGCONFIG tdc   = {};
    tdc.cbSize             = sizeof(tdc);
    tdc.hwndParent         = GetSafeHwnd();
    tdc.dwFlags            = TDF_ALLOW_DIALOG_CANCELLATION;
    tdc.pszMainInstruction = L"원하는 형태를 고르세요.";
    tdc.pButtons           = aButtons;
    tdc.cButtons           = 3;
    tdc.nDefaultButton     = ID_FILE;

    int nButton = IDCANCEL;
    TaskDialogIndirect(&tdc, &nButton, nullptr, nullptr);

    if (nButton == ID_FILE)
    {
        // 파일 열기 대화 상자
        CFileDialog dlg(TRUE, nullptr, nullptr,
                        OFN_HIDEREADONLY | OFN_FILEMUSTEXIST,
                        _T("모든 파일 (*.*)|*.*||"), this);
        if (dlg.DoModal() == IDOK)
            m_editPath.SetWindowText(dlg.GetPathName());
    }
    else if (nButton == ID_FOLDER)
    {
        // 폴더 선택 대화 상자
        BROWSEINFO bi = {};
        bi.hwndOwner  = GetSafeHwnd();
        bi.lpszTitle  = _T("폴더를 선택하세요.");
        bi.ulFlags    = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;

        LPITEMIDLIST pidl = SHBrowseForFolder(&bi);
        if (pidl)
        {
            TCHAR folderPath[MAX_PATH];
            if (SHGetPathFromIDList(pidl, folderPath))
                m_editPath.SetWindowText(folderPath);
            CoTaskMemFree(pidl);
        }
    }
    else if (nButton == ID_WEBSITE)
    {
        // 경로 편집창에 https:// 접두사가 없으면 채워 주고 포커스 이동
        CString strCurrent;
        m_editPath.GetWindowText(strCurrent);
        if (strCurrent.IsEmpty())
            m_editPath.SetWindowText(_T("https://"));
        m_editPath.SetFocus();
        m_editPath.SetSel(0, -1);   // 전체 선택
    }
}

// ── 적용: 편집 내용을 현재 선택 버튼에 저장 ──────────────────────
void CSettingsDlg::OnBtnApply()
{
    if (m_nSelected < 0 || m_nSelected >= GRID_BTN_COUNT)
    {
        AfxMessageBox(_T("목록에서 버튼을 먼저 선택하세요."), MB_OK | MB_ICONWARNING);
        return;
    }

    m_editCaption.GetWindowText(m_configs[m_nSelected].strCaption);
    m_editPath.GetWindowText(m_configs[m_nSelected].strPath);
    RefreshListItem(m_nSelected);
}

// ── 확인: 현재 편집 중인 내용도 함께 반영 ────────────────────────
void CSettingsDlg::OnOK()
{
    if (m_nSelected >= 0 && m_nSelected < GRID_BTN_COUNT)
    {
        m_editCaption.GetWindowText(m_configs[m_nSelected].strCaption);
        m_editPath.GetWindowText(m_configs[m_nSelected].strPath);
    }
    CDialogEx::OnOK();
}
