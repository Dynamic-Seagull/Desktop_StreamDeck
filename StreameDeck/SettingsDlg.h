#pragma once

#include "StreameDeckDlg.h"   // ButtonConfig, GRID_BTN_COUNT 등


// CSettingsDlg 대화 상자
class CSettingsDlg : public CDialogEx
{
public:
    CSettingsDlg(ButtonConfig* pConfigs, CWnd* pParent = nullptr);

#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_SETTINGS_DIALOG };
#endif

    // 확인(IDOK) 반환 후 메인 다이얼로그가 읽어가는 결과값
    ButtonConfig    m_configs[GRID_BTN_COUNT];

protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    virtual BOOL OnInitDialog();
    virtual void OnOK();

    afx_msg void OnListSelChange(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void   OnBtnBrowse();
    afx_msg void   OnBtnApply();

    DECLARE_MESSAGE_MAP()

private:
    // 컨트롤 멤버
    CFont       m_font;
    CListCtrl   m_listBtns;
    CEdit       m_editCaption;
    CEdit       m_editPath;
    CButton     m_btnBrowse;
    CButton     m_btnApply;
    CButton     m_btnOK;
    CButton     m_btnCancel;
    CButton     m_groupConfig;
    CStatic     m_lblCaption;
    CStatic     m_lblPath;

    int         m_nSelected;   // 현재 선택된 목록 인덱스 (-1 = 없음)

    void CreateControls();
    void PopulateList();
    void UpdateEditFields(int nIdx);
    void RefreshListItem(int nIdx);
};
