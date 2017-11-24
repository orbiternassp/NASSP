#ifndef _ProjectApolloMFDButtonsClass
#define _ProjectApolloMFDButtonsClass

#include "MFDButtonPage.hpp"

class ProjectApolloMFD;

class ProjectApolloMFDButtons : public EnjoLib::MFDGoodies::MFDButtonPage<ProjectApolloMFD>
{
public:
	ProjectApolloMFDButtons();
protected:
	bool SearchForKeysInOtherPages() const;
private:
};
#endif