#include "Connector.h"
#include "MFDConnector.h"

class ProjectApolloChecklistMFD: public MFD {
public:
	ProjectApolloChecklistMFD (DWORD w, DWORD h, VESSEL *vessel);
	~ProjectApolloChecklistMFD ();
	char *ButtonLabel (int bt);
	int ButtonMenu (const MFDBUTTONMENU **menu) const;
	bool ConsumeButton (int bt, int event);
	bool ConsumeKeyBuffered (DWORD key);
	void Update (HDC hDC);
	void WriteStatus (FILEHANDLE scn) const;
	void ReadStatus (FILEHANDLE scn);
	void StoreStatus (void) const;
	void RecallStatus (void);

	MFDConnector conn;

	
	static int MsgProc (UINT msg, UINT mfd, WPARAM wparam, LPARAM lparam);

	int SelectedGroup;
	char* SelectedGroupName;

	ChecklistItem item;

	vector<ChecklistGroup> groups;

	DWORD width;
	DWORD height;

protected:

	int screen;

	bool MFDInit; // MFD has been initialized, including retrieving Available Checklists
	int NumChkLsts;
	bool ChkLstAutoOn;

	std::string line;

	int cnt;
	
	int TopStep;
	int CurrentStep;
	int HiLghtdLine;
};