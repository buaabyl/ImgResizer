
// ImgResizerDlg.h : header file
//

#pragma once
#include "afxwin.h"


// CImgResizerDlg dialog
class CImgResizerDlg : public CDialogEx
{
// Construction
public:
	CImgResizerDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_IMGRESIZER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
    int idx_resolution_;
    bool brunning_;
    CString title_;
    int percent_;

public:
    CStatic m_picbox_show_;
    CProgressCtrl m_progressctrl_percent_;
    CButton m_radio_resolution_[6];
    CvSize size_resolution_[6];
    CStatic m_static_group_;
        
public:
    afx_msg void OnDropFiles(HDROP hDropInfo);
    afx_msg LRESULT OnUpdateProgress(WPARAM nr_converted, LPARAM nr_total);

private:
    void CustomPaint();
};
