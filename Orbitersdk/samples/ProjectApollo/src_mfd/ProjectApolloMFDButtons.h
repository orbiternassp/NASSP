#ifndef _ProjectApolloMFDButtonsClass
#define _ProjectApolloMFDButtonsClass

#include "MFDButtonPage.hpp"

class ProjectApolloMFD;

class ProjectApolloMFDButtons : public EnjoLib::MFDGoodies::MFDButtonPage<ProjectApolloMFD>
{
public:
	struct Pages {
		int None;
		int GNC;
		int ECS;
		int IU;
		int TELE;
		int Debug;
		int LGC;
		int Failures;
	} page;

	ProjectApolloMFDButtons();
protected:
	bool SearchForKeysInOtherPages() const;
private:
};
#endif