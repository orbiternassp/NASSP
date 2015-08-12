// ==============================================================
//          ORBITER AUX LIBRARY: Multiple Buttons Pages
//             http://sf.net/projects/enjomitchsorbit
//                  Part of the ORBITER SDK
//
// Copyright (C) 2012      Szymon "Enjo" Ender
//
//                         All rights reserved
//
// Multiple Buttons Pages is free software: you can redistribute it
// and/or modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation, either version
// 3 of the License, or (at your option) any later version.
//
// Multiple Buttons Pages is distributed in the hope that it will
// be useful, but WITHOUT ANY WARRANTY; without even the implied
// warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with Multiple Vessels Support. If not, see
// <http://www.gnu.org/licenses/>.
// ==============================================================

#ifndef MFDButtonPage_H
#define MFDButtonPage_H

#include <OrbiterSDK.h>
#include <map>
#include <vector>

namespace EnjoLib
{
namespace MFDGoodies
{
/// Manages MFD button pages.
/**
    Allows to register many switchable MFD button pages of the MFDClass and associates handlers
    for the buttons. The handlers must reside in the MFDClass, and they must not have any arguments.
    \tparam <MFDClass>  Your MFD class that should contain handlers
*/
template <class MFDClass>
class MFDButtonPage
{
    public:
        /// Ctor
        MFDButtonPage();
        /// Dtor
        virtual ~MFDButtonPage();

        /// Switches between button pages
        /**
            A button for this method needs to be registered through RegisterFunction(),
            which should call MFDClass' method, which then calls this method. Example:
            \code
            MFDButtonPageLaunchMFD::MFDButtonPageLaunchMFD()
            {
                // ... add menu first, then functions
                RegisterFunction("PG",  OAPI_KEY_P, &LaunchMFD::SwitchButtonsPage);
                // ...
            }
            \endcode
            \code
            void LaunchMFD::SwitchButtonsPage()
            {
                m_buttonPages.SwitchPage(this);
            }
            \endcode
            \param mfdInstance - Instance of your MFD class
        */
        void SwitchPage( MFDClass * mfdInstance ) const;

        /// Selects a specific buttons page
        /**
            Could be used instead of SwitchPage(). Example:
            \code
            void LaunchMFD::SomeFunction( int pageIndex )
            {
                m_buttonPages.SelectPage(this, pageIndex);
            }
            \endcode
            \param mfdInstance - Instance of your MFD class
            \param pageIndex - Index of page to switch to.
        */
        void SelectPage( MFDClass * mfdInstance, int pageIndex ) const;

        /// Reacts on mouse clicks on buttons
        /**
            Call from MFD::ConsumeButton(). Example:
            \code
            bool LaunchMFD::ConsumeButton (int bt, int event)
            {
                return m_buttonPages.ConsumeButton(this, bt, event);
            }
            \endcode
            \param mfdInstance - Instance of your MFD class (this)
            \param button - button number
            \param event - Windows event
        */
        bool ConsumeButton( MFDClass * mfdInstance, int button, int event ) const;

        /// Reacts on keyboard presses
        /**
            Searches for the provided key in a map for the selected page and fires associated function.
            If SearchForKeysInOtherPages() returns true, then other pages are searched for as well.
            Call from MFD::ConsumeKeyBuffered(). Example:
            \code
            bool LaunchMFD::ConsumeKeyBuffered(DWORD key)
            {
                return m_buttonPages.ConsumeKeyBuffered(this, key);
            }
            \endcode
            \param mfdInstance - Instance of your MFD class (this)
            \param key - key designation, like OAPI_KEY_T
        */
        bool ConsumeKeyBuffered( MFDClass * mfdInstance, DWORD key ) const;

        /// Reacts on continuous keyboard presses
        /**
            Searches for the provided key in a map for the selected page and fires associated function.
            If SearchForKeysInOtherPages() returns true, then other pages are searched for as well.
            The function's execution is continuous.
            Call from MFD::ConsumeKeyImmediate(). Example:
            \code
            bool LaunchMFD::ConsumeKeyImmediate(char * kstate)
            {
                return m_buttonPages.ConsumeKeyImmediate(this, kstate);
            }
            \endcode
            \param mfdInstance - Instance of your MFD class (this)
            \param kstate - key state from Orbiter
        */
        bool ConsumeKeyImmediate( MFDClass * mfdInstance, char * kstate ) const;

        /// Returns button menu of the current page
        /**
            Call from MFD::ButtonMenu(). Example:
            \code
            int LaunchMFD::ButtonMenu (const MFDBUTTONMENU **menu) const
            {
                return m_buttonPages.ButtonMenu( menu );
            }
            \endcode
            \param menu - this will be initialised by internal structure
        */
        int ButtonMenu (const MFDBUTTONMENU **menu) const;

        /// Returns button labels of the current page
        /**
            Call from MFD::ButtonLabel(). Example:
            \code
            char * LaunchMFD::ButtonLabel (int bt)
            {
                return m_buttonPages.ButtonLabel(bt);
            }
            \endcode
            \param button - button number
        */
        char * ButtonLabel (int button) const;

    protected:
        /// Should other pages be searched for when pressing a key?
        /**
            Normally the system reacts only keys on the current page.
            If this method returns true, then other pages are searched for as well.
        */
        virtual bool SearchForKeysInOtherPages() const = 0;

        /// Function pointers of the MFDClass
        typedef void (MFDClass::* MFDFunctionPtr)(void);

        /// Registeres handler in MFD scope for buttons.
        /**
            Before this method has any effect, a button page needs to be registered first with
            RegisterPage(). The keys may be repeated in different pages.
            \code
            MFDButtonPageLaunchMFD::MFDButtonPageLaunchMFD()
            {
                // ... add menu first, then functions
                RegisterFunction("TGT", OAPI_KEY_T, &LaunchMFD::OpenDialogTarget);
                RegisterFunction("ALT", OAPI_KEY_A, &LaunchMFD::OpenDialogAltitude);
                RegisterFunction("PG", OAPI_KEY_P, &LaunchMFD::SwitchButtonsPage);
                RegisterFuncCont("I +", OAPI_KEY_EQUALS, &LaunchMFD::IncreaseInclination, &LaunchMFD::DecreaseInclination);
                RegisterFuncCont("I -", OAPI_KEY_MINUS,  &LaunchMFD::DecreaseInclination, &LaunchMFD::IncreaseInclination);
            }
            \endcode
            \param label - max 3 char long label of button. Example: "TGT"
            \param key - associated key on keyboard, one of OAPI_KEY_*. Example: OAPI_KEY_T
            \param funLClick - on left click MFD function pointer (handler). Example: & MyMFD::OpenDialogTarget. The handler can"t take any arguments.
            \param funRClick - on right click MFD function pointer (handler). Optional
        */
        void RegisterFunction( const std::string & label, DWORD key,
                              MFDFunctionPtr funLClick, MFDFunctionPtr funRClick = NULL );

        /// Registeres handler in MFD scope for buttons that should have a continous reaction
        /**
            Same as RegisterFunction(), but allows for continuous reaction on the keys,
            like for example, when you want to continuously modify a variable.
            \param label - max 3 char long label of button. Example: "TGT"
            \param key - associated key on keyboard, one of OAPI_KEY_*. Example: OAPI_KEY_T
            \param funLClick - on left click MFD function pointer (handler). Example: & MyMFD::OpenDialogTarget. The handler can"t take any arguments.
            \param funRClick - on right click MFD function pointer (handler). Optional
        */
        void RegisterFunctionCont( const std::string & label, DWORD key,
                              MFDFunctionPtr funLClick, MFDFunctionPtr funRClick = NULL );

        /// Registeres button page, and buttons menu
        /**
            Must be called before RegisterFunction() is called.
            The menu must be \b STATICALLY declared. For example:
            \code
            static const MFDBUTTONMENU mnu1[] =
            {
                {"Select target", 0, 'T'},
                {"Enter altitude", "km", 'A'},
                {"Switch button page", 0, 'P'},
            };
            RegisterPage(mnu1, sizeof(mnu1) / sizeof(MFDBUTTONMENU));
            \endcode
            \param menu - \b STATICALLY declared button menu
            \param size - menu size. Use sizeof(menu) / sizeof(MFDBUTTONMENU)
        */
        void RegisterPage( const MFDBUTTONMENU * menu, int size );

    private:

        /// Represents button page
        struct Page
        {
            const MFDBUTTONMENU * m_menu;
            int m_menuSize;
            std::vector<std::string> m_labels;
            std::vector<bool> m_continuousClick;
            std::vector<MFDFunctionPtr> m_buttonsLeftClick;
            std::vector<MFDFunctionPtr> m_buttonsRightClick;
            std::map<DWORD, MFDFunctionPtr> m_keys;
            std::map<DWORD, bool> m_continuousKey;
        };

        bool PressKey( MFDClass * mfdInstance, DWORD key ) const;
        void RegisterFuncPriv( bool continuous, const std::string & label, DWORD key,
                              MFDFunctionPtr funLClick, MFDFunctionPtr funRClick);

        std::vector<Page> m_pages; ///< Current page index
        mutable size_t m_i; ///< Current page index

        typedef std::map<DWORD, bool>::const_iterator MapBoolIterator;
};

// Public:
template <class MFDClass>
MFDButtonPage<MFDClass>::MFDButtonPage()
: m_i(0)
{
}

template <class MFDClass>
MFDButtonPage<MFDClass>::~MFDButtonPage() {}

template <class MFDClass>
void MFDButtonPage<MFDClass>::SwitchPage( MFDClass * mfdInstance )  const
{
    m_i = (++m_i) % m_pages.size(); // increment index, but not beyond the size
    mfdInstance->InvalidateButtons(); // redraw buttons
}

template <class MFDClass>
void MFDButtonPage<MFDClass>::SelectPage( MFDClass * mfdInstance, int pageIndex ) const
{
    if (pageIndex >= 0 && pageIndex < (int)m_pages.size())
    {
        m_i = pageIndex;
        mfdInstance->InvalidateButtons(); // redraw buttons
    }
    else
        sprintf_s(oapiDebugString(), 512, "MFDButtonPage::SwitchPage():"
                  " Page index %d is beyond pages size %d!", pageIndex, m_pages.size());
}

template <class MFDClass>
bool MFDButtonPage<MFDClass>::ConsumeButton( MFDClass * mfdInstance, int button, int event ) const
{
    if ( button >= (int)m_pages.at(m_i).m_buttonsLeftClick.size())
        return false;

    if (event & PANEL_MOUSE_LBDOWN ||
        (event & PANEL_MOUSE_LBPRESSED && m_pages.at(m_i).m_continuousClick[button]) )
    {   // Left mouse button just clicked
        (mfdInstance->*(m_pages.at(m_i).m_buttonsLeftClick[button]))(); // Call the function
        return true;
    }
    else  if ( event & PANEL_MOUSE_RBDOWN ||
            ( event & PANEL_MOUSE_RBPRESSED && m_pages.at(m_i).m_continuousClick[button] ) )
    {
        MFDFunctionPtr fun = m_pages.at(m_i).m_buttonsRightClick[button];
        if ( fun ) // If function was registered at all
        {
            (mfdInstance->*(fun))(); // Call the function
            return true;
        }
    }
    return false;
}

template <class MFDClass>
bool MFDButtonPage<MFDClass>::ConsumeKeyBuffered( MFDClass * mfdInstance, DWORD key ) const
{
    // First search for the key on this page
    MapBoolIterator it = m_pages.at(m_i).m_continuousKey.find(key);
    if (it != m_pages.at(m_i).m_continuousKey.end() && ! it->second )
        return PressKey(mfdInstance, key);

    if ( SearchForKeysInOtherPages() )
    {
        // Then, if required, search in other pages
        for ( size_t j = 0; j < m_pages.size(); ++j )
        {
            if ( m_i == j )
                continue; // The current page was already queried
            MapBoolIterator it = m_pages.at(j).m_continuousKey.find(key);
            if (it != m_pages.at(j).m_continuousKey.end() && ! it->second )
                return PressKey(mfdInstance, key);
        }
    }
    return false;
}

template <class MFDClass>
bool MFDButtonPage<MFDClass>::ConsumeKeyImmediate( MFDClass * mfdInstance, char * kstate ) const
{
    for (MapBoolIterator it = m_pages.at(m_i).m_continuousKey.begin();
    it != m_pages.at(m_i).m_continuousKey.end(); ++it)
    {
        if ( KEYDOWN(kstate, it->first ) && it->second )
            return PressKey(mfdInstance, it->first);
    }

    if ( SearchForKeysInOtherPages() )
    {
        for ( size_t j = 0; j < m_pages.size(); ++j )
        {
             if ( m_i == j )
                continue; // The current page was already queried

            for (MapBoolIterator it = m_pages.at(j).m_continuousKey.begin();
            it != m_pages.at(j).m_continuousKey.end(); ++it)
            {
                if ( KEYDOWN(kstate, it->first ) && it->second )
                    return PressKey(mfdInstance, it->first);
            }
        }
    }
    return false;
}

template <class MFDClass>
int MFDButtonPage<MFDClass>::ButtonMenu (const MFDBUTTONMENU **menu) const
{
    if ( menu ) *menu = m_pages.at(m_i).m_menu; // pass the static menu to the parameric pointer
    return m_pages.at(m_i).m_menuSize;
}

template <class MFDClass>
char * MFDButtonPage<MFDClass>::ButtonLabel (int bt) const
{
    return (bt < (int)m_pages.at(m_i).m_labels.size() ?
             (char*)m_pages.at(m_i).m_labels[bt].c_str() : NULL);
}

// Protected:
template <class MFDClass>
void MFDButtonPage<MFDClass>::RegisterFunction( const std::string & label, DWORD key,
                                               MFDFunctionPtr funLClick, MFDFunctionPtr funRClick )
{
    RegisterFuncPriv( false, label, key, funLClick, funRClick);
}

template <class MFDClass>
void MFDButtonPage<MFDClass>::RegisterFunctionCont( const std::string & label, DWORD key,
                                               MFDFunctionPtr funLClick, MFDFunctionPtr funRClick )
{
    RegisterFuncPriv( true, label, key, funLClick, funRClick);
}

template <class MFDClass>
void MFDButtonPage<MFDClass>::RegisterPage( const MFDBUTTONMENU * menu, int size )
{
    Page p;
    p.m_menu = menu;
    p.m_menuSize = size;
    m_pages.push_back(p);
}

// Private:
template <class MFDClass>
bool MFDButtonPage<MFDClass>::PressKey( MFDClass * mfdInstance, DWORD key ) const
{
    typedef std::map<DWORD, MFDFunctionPtr>::const_iterator MapFuncIterator;
    // First search for the key on this page
    MapFuncIterator it = m_pages.at(m_i).m_keys.find(key);
    if (it != m_pages.at(m_i).m_keys.end() )
    {
        (mfdInstance->*(it->second))(); // Call the function
        return true;
    }

    if ( SearchForKeysInOtherPages() )
    {
        // Then, if required, search in other pages
        for ( size_t j = 0; j < m_pages.size(); ++j )
        {
            if ( m_i == j )
                continue; // The current page was already queried
            MapFuncIterator it = m_pages.at(j).m_keys.find(key);
            if (it != m_pages.at(j).m_keys.end() )
            {
                (mfdInstance->*(it->second))();  // Call the function
                return true;
            }
        }
    }
    return false;
}

template <class MFDClass>
void MFDButtonPage<MFDClass>::RegisterFuncPriv( bool continuous, const std::string & label, DWORD key,
                              MFDFunctionPtr funLClick, MFDFunctionPtr funRClick)
{
    if ( m_pages.empty() )
    {
        sprintf_s(oapiDebugString(), 512, "MFDButtonPage::RegisterFuncPriv(): No pages registered yet!");
        return;
    }
    Page & p = m_pages.back();
    p.m_labels.push_back(label);
    p.m_continuousClick.push_back(continuous);
    p.m_buttonsLeftClick.push_back(funLClick);
    p.m_buttonsRightClick.push_back(funRClick);
    p.m_keys[key] = funLClick;
    p.m_continuousKey[key] = continuous;
}
}
}

#endif // MFDButtonPage_H
