#ifndef _ApolloGuidanceMFDButtonsClass
#define _ApolloGuidanceMFDButtonsClass

class ApolloRTCCMFD;

class ApolloRTCCMFDButtons : public EnjoLib::MFDGoodies::MFDButtonPage<ApolloRTCCMFD>
{
public:
	ApolloRTCCMFDButtons();
protected:
	bool SearchForKeysInOtherPages() const;
private:
};
#endif