
// StreameDeckDlg.h: 헤더 파일
//

#pragma once

#define GRID_BTN_COUNT  15
#define GRID_COLS       5
#define GRID_ROWS       3

struct ButtonConfig
{
    CString strCaption;
    CString strPath;
};


// CStreameDeckDlg 대화 상자
class CStreameDeckDlg : public CDialogEx
{
// 생성
public:
    CStreameDeckDlg(CWnd* pParent = nullptr);

// 대화 상자 데이터
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_STREAMEDECK_DIALOG };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX);

public:
    ButtonConfig    m_configs[GRID_BTN_COUNT];

// 구현
private:
    HICON           m_hIcon;
    CFont           m_font;
    CBrush          m_brBg;
    CButton         m_gridBtns[GRID_BTN_COUNT];
    CButton         m_btnSettings;
    CButton         m_btnClose;

    // BMP 이미지 핸들
    HBITMAP         m_hBmpBg;
    HBITMAP         m_hBmpBtn;
    HBITMAP         m_hBmpBtnClicked;
    HBITMAP         m_hBmpSetting;
    HBITMAP         m_hBmpSettingClicked;
    HBITMAP         m_hBmpClose;
    HBITMAP         m_hBmpCloseClicked;

    CString         GetImgPath(LPCTSTR fileName);
    void            LoadImages();
    void            CreateControls();
    void            UpdateButtonCaptions();
    void            LoadConfig();
    void            SaveConfig();
    CString         GetConfigPath();

protected:
    virtual BOOL    OnInitDialog();
    afx_msg void    OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void    OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    afx_msg BOOL    OnEraseBkgnd(CDC* pDC);
    afx_msg void    OnGridButton(UINT nID);
    afx_msg void    OnBtnSettings();
    afx_msg void    OnBtnClose();
    afx_msg HBRUSH  OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
    afx_msg void    OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDIS);
    afx_msg LRESULT OnNcHitTest(CPoint point);
    afx_msg void    OnDestroy();
    DECLARE_MESSAGE_MAP()
};
