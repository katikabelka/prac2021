/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * This file incorporates work covered by the following license notice:
 *
 *   Licensed to the Apache Software Foundation (ASF) under one or more
 *   contributor license agreements. See the NOTICE file distributed
 *   with this work for additional information regarding copyright
 *   ownership. The ASF licenses this file to you under the Apache
 *   License, Version 2.0 (the "License"); you may not use this file
 *   except in compliance with the License. You may obtain a copy of
 *   the License at http://www.apache.org/licenses/LICENSE-2.0 .
 */


#include "ListenerHelper.h"
#include "MyProtocolHandler.h"

#include <com/sun/star/awt/MessageBoxButtons.hpp>
#include <com/sun/star/awt/Toolkit.hpp>
#include <com/sun/star/awt/XMessageBoxFactory.hpp>
#include <com/sun/star/frame/ControlCommand.hpp>
#include <com/sun/star/text/XTextViewCursorSupplier.hpp>
#include <com/sun/star/sheet/XSpreadsheetView.hpp>
#include <com/sun/star/system/SystemShellExecute.hpp>
#include <com/sun/star/system/SystemShellExecuteFlags.hpp>
#include <com/sun/star/system/XSystemShellExecute.hpp>
#include <cppuhelper/supportsservice.hxx>
#include <com/sun/star/frame/XComponentLoader.hpp>
#include <com/sun/star/text/XTextDocument.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/frame/XLoaderFactory.hpp>
#include <com/sun/star/text/XTextTable.hpp>
#include <com/sun/star/table/XCell.hpp>
#include <iostream>

using namespace com::sun::star::awt;
using namespace com::sun::star::frame;
using namespace com::sun::star::system;
using namespace com::sun::star::uno;

using com::sun::star::beans::NamedValue;
using com::sun::star::beans::PropertyValue;
using com::sun::star::sheet::XSpreadsheetView;
using com::sun::star::text::XTextViewCursorSupplier;
using com::sun::star::util::URL;

ListenerHelper aListenerHelper;

void BaseDispatch::ShowMessageBox( const Reference< XFrame >& rFrame, const ::rtl::OUString& aTitle, const ::rtl::OUString& aMsgText )
{
    if ( !mxToolkit.is() )
        mxToolkit = Toolkit::create(mxContext);
    Reference< XMessageBoxFactory > xMsgBoxFactory( mxToolkit, UNO_QUERY );
    if ( rFrame.is() && xMsgBoxFactory.is() )
    {
        Reference< XMessageBox > xMsgBox = xMsgBoxFactory->createMessageBox(
            Reference< XWindowPeer >( rFrame->getContainerWindow(), UNO_QUERY ),
            com::sun::star::awt::MessageBoxType_INFOBOX,
            MessageBoxButtons::BUTTONS_OK,
            aTitle,
            aMsgText );

        if ( xMsgBox.is() )
            xMsgBox->execute();
    }
}

void BaseDispatch::SendCommand( const com::sun::star::util::URL& aURL, const ::rtl::OUString& rCommand, const Sequence< NamedValue >& rArgs, sal_Bool bEnabled )
{
    Reference < XDispatch > xDispatch =
            aListenerHelper.GetDispatch( mxFrame, aURL.Path );

    FeatureStateEvent aEvent;

    aEvent.FeatureURL = aURL;
    aEvent.Source     = xDispatch;
    aEvent.IsEnabled  = bEnabled;
    aEvent.Requery    = sal_False;

    ControlCommand aCtrlCmd;
    aCtrlCmd.Command   = rCommand;
    aCtrlCmd.Arguments = rArgs;

    aEvent.State <<= aCtrlCmd;
    aListenerHelper.Notify( mxFrame, aEvent.FeatureURL.Path, aEvent );
}

void BaseDispatch::SendCommandTo( const Reference< XStatusListener >& xControl, const URL& aURL, const ::rtl::OUString& rCommand, const Sequence< NamedValue >& rArgs, sal_Bool bEnabled )
{
    FeatureStateEvent aEvent;

    aEvent.FeatureURL = aURL;
    aEvent.Source     = (::com::sun::star::frame::XDispatch*) this;
    aEvent.IsEnabled  = bEnabled;
    aEvent.Requery    = sal_False;

    ControlCommand aCtrlCmd;
    aCtrlCmd.Command   = rCommand;
    aCtrlCmd.Arguments = rArgs;

    aEvent.State <<= aCtrlCmd;
    xControl->statusChanged( aEvent );
}

void SAL_CALL MyProtocolHandler::initialize( const Sequence< Any >& aArguments ) throw ( Exception, RuntimeException)
{
    Reference < XFrame > xFrame;
    if ( aArguments.getLength() )
    {
        // the first Argument is always the Frame, as a ProtocolHandler needs to have access
        // to the context in which it is invoked.
        aArguments[0] >>= xFrame;
        mxFrame = xFrame;
    }
}

Reference< XDispatch > SAL_CALL MyProtocolHandler::queryDispatch(   const URL& aURL, const ::rtl::OUString& sTargetFrameName, sal_Int32 nSearchFlags )
                throw( RuntimeException )
{
    Reference < XDispatch > xRet;
    if ( !mxFrame.is() )
        return 0;

    Reference < XController > xCtrl = mxFrame->getController();
    if ( xCtrl.is() && aURL.Protocol == "vnd.demo.complextoolbarcontrols.demoaddon:" )
    {
        Reference < XTextViewCursorSupplier > xCursor( xCtrl, UNO_QUERY );
        Reference < XSpreadsheetView > xView( xCtrl, UNO_QUERY );
        if ( !xCursor.is() && !xView.is() )
            // without an appropriate corresponding document the handler doesn't function
            return xRet;

        if ( aURL.Path == "GenButtonCmd" ||
             aURL.Path == "StatButtonCmd" ||
             aURL.Path == "CheckButtonCmd" ||
             aURL.Path == "ComboboxCmd" ||
             aURL.Path == "SpinfieldCmd" ||
             aURL.Path == "DropdownboxCmd" )
        {
            xRet = aListenerHelper.GetDispatch( mxFrame, aURL.Path );
            if ( !xRet.is() )
            {
                xRet = xCursor.is() ? (BaseDispatch*) new WriterDispatch( mxContext, mxFrame ) :
                    (BaseDispatch*) new CalcDispatch( mxContext, mxFrame );
                aListenerHelper.AddDispatch( xRet, mxFrame, aURL.Path );
            }
        }
    }

    return xRet;
}

Sequence < Reference< XDispatch > > SAL_CALL MyProtocolHandler::queryDispatches( const Sequence < DispatchDescriptor >& seqDescripts )
            throw( RuntimeException )
{
    sal_Int32 nCount = seqDescripts.getLength();
    Sequence < Reference < XDispatch > > lDispatcher( nCount );

    for( sal_Int32 i=0; i<nCount; ++i )
        lDispatcher[i] = queryDispatch( seqDescripts[i].FeatureURL, seqDescripts[i].FrameName, seqDescripts[i].SearchFlags );

    return lDispatcher;
}

::rtl::OUString MyProtocolHandler_getImplementationName ()
    throw (RuntimeException)
{
    return ::rtl::OUString( MYPROTOCOLHANDLER_IMPLEMENTATIONNAME );
}

Sequence< ::rtl::OUString > SAL_CALL MyProtocolHandler_getSupportedServiceNames(  )
    throw (RuntimeException)
{
    Sequence < ::rtl::OUString > aRet(1);
    aRet[0] = ::rtl::OUString( MYPROTOCOLHANDLER_SERVICENAME );
    return aRet;
}

#undef SERVICE_NAME

Reference< XInterface > SAL_CALL MyProtocolHandler_createInstance( const Reference< XComponentContext > & rSMgr)
    throw( Exception )
{
    return (cppu::OWeakObject*) new MyProtocolHandler( rSMgr );
}

// XServiceInfo
::rtl::OUString SAL_CALL MyProtocolHandler::getImplementationName(  )
    throw (RuntimeException)
{
    return MyProtocolHandler_getImplementationName();
}

sal_Bool SAL_CALL MyProtocolHandler::supportsService( const ::rtl::OUString& rServiceName )
    throw (RuntimeException)
{
    return cppu::supportsService(this, rServiceName);
}

Sequence< ::rtl::OUString > SAL_CALL MyProtocolHandler::getSupportedServiceNames(  )
    throw (RuntimeException)
{
    return MyProtocolHandler_getSupportedServiceNames();
}

void SAL_CALL BaseDispatch::dispatch( const URL& aURL, const Sequence < PropertyValue >& lArgs ) throw (RuntimeException)
{
    /* It's necessary to hold this object alive, till this method finishes.
       May the outside dispatch cache (implemented by the menu/toolbar!)
       forget this instance during de-/activation of frames (focus!).

        E.g. An open db beamer in combination with the My-Dialog
        can force such strange situation :-(
     */
    Reference< XInterface > xSelfHold(static_cast< XDispatch* >(this), UNO_QUERY);

    if ( aURL.Protocol == "vnd.demo.complextoolbarcontrols.demoaddon:" )
    {
        if ( aURL.Path == "GenButtonCmd" )
        {
            Reference< ::com::sun::star::frame::XComponentLoader > xComponentLoader(mxFrame, UNO_QUERY);
            const com::sun::star::uno::Sequence<com::sun::star::beans::PropertyValue> empty_args(0);
            
            auto newFrame = xComponentLoader->loadComponentFromURL("private:factory/swriter", "_blank", 0, empty_args);

            Reference< ::com::sun::star::text::XTextDocument > xTextDocument(newFrame, UNO_QUERY);
            sal_uInt32 arr[100000];
            sal_uInt32 c;
            std::stringstream ss;
            ss << maComboBoxText;
            ss >> maxlength;
            if (!maxlength) {
                maxlength = 1;
            }
            int cnt = 0;
            rtl::OUString ans;
            bool start = false;
            int chance;
            rtl::OUString cur_lay = "Ru";
            for (int i = 0; i < numwords; i++) {
                cnt = std::rand()% maxlength + 1;
                if (layout.compareTo(rtl::OUString("Mix")) == 0) {
                    chance = std::rand() % 2;
                    if (chance) {
                        cur_lay = "Ru";
                    } else {
                        cur_lay = "En";
                    }
                } else {
                    cur_lay = layout;
                }

                if (cur_lay.compareTo(rtl::OUString("En")) == 0) {
                    for (int i = 0; i < cnt; i++) {
                        c = std::rand()% 26;
                        sal_Unicode t = 'a' + c;
                       ans += rtl::OUString(t);
                    }
                } else if (cur_lay.compareTo(rtl::OUString("Ru")) == 0) {
                    for (int i = 0; i < cnt; i++) {
                        c = std::rand()% 32;
                        if (c == 31) {
                            sal_Unicode t = L'ё';
                            ans += rtl::OUString(L'ё');
                        }
                        sal_Unicode t = L'а' + c;
                        ans += rtl::OUString(t);
                    }
                }
                ans += " ";
            } 
            xTextDocument->getText()->setString(ans);
        }
        else if ( aURL.Path == "StatButtonCmd" )
        {
            auto doc = mxFrame->getController()->getModel();
            Reference< ::com::sun::star::text::XTextDocument > xTextDocument(doc, UNO_QUERY);
            auto xText = xTextDocument->getText();
            auto txt = xText->getString();
            int32_t len = txt.getLength();
            bool enword = false, ruword = false;
            int32_t now = 0;
            std::map <int32_t, int32_t> stat;
            for (int32_t i = 0; i < len; i++) {
                if (txt[i] >='A' && txt[i] <= 'Z' || txt[i] >= 'a' && txt[i] <= 'z') {
                    if (!enword) {
                        enword = true;
                        now = 1;
                    }
                    else {
                        now++;
                    }
                } else {
                    if (enword) {
                        enword = false;
                        if (stat.find(now) == stat.end()) {
                            stat.insert(std::pair<int32_t, int32_t>(now, 1));
                        } else {
                            stat[now] += 1;
                        }
                    }
                }
                if (txt[i] >= L'А' && txt[i] <= L'Я' || txt[i] >= L'а' && txt[i] <= L'я' || txt[i] == L'ё' || txt[i] == L'Ё') {
                    if (!ruword) {
                        ruword = true;
                        now = 1;
                    }
                    else {
                        now++;
                    }
                } else {
                    if (ruword) {
                        ruword = false;
                        if (stat.find(now) == stat.end()) {
                            stat.insert(std::pair<int32_t, int32_t>(now, 1));
                        } else {
                            stat[now] += 1;
                        }
                    }
                }
            }
            if (ruword || enword) {
                if (stat.find(now) == stat.end()) {
                    stat.insert(std::pair<int32_t, int32_t>(now, 1));
                } else {
                    stat[now] += 1;
                }
            }
            
            Reference<com::sun::star::lang::XMultiServiceFactory> xMultiServiceFactory (xTextDocument, UNO_QUERY);
            Reference <com::sun::star::text::XTextTable> xTextTable (xMultiServiceFactory->createInstance("com.sun.star.text.TextTable"),UNO_QUERY);
            xTextTable -> initialize(stat.size() + 1, 2);
            Reference <com::sun::star::text::XTextContent> xTextContent (xTextTable, UNO_QUERY);
            xText -> insertTextContent(xText->getEnd(), xTextContent, 0);
            auto fields = xTextTable->getCellNames();

            auto fld = xTextTable->getCellByName(fields[0]);
            Reference<com::sun::star::text::XText> xTextFld1(fld, UNO_QUERY);
            auto cursor = xTextFld1->createTextCursor();
            cursor->setString("Length of the word");

            fld = xTextTable->getCellByName(fields[1]);
            Reference<com::sun::star::text::XText> xTextFld2(fld, UNO_QUERY);
            cursor = xTextFld2->createTextCursor();
            cursor->setString("Number of words");

            sal_Int64 i = 2;
        
            for (auto it = stat.begin(); it != stat.end(); ++it) {
                auto fld = xTextTable->getCellByName(fields[i]);
                fld->setValue((*it).first);
                i++;
                fld = xTextTable->getCellByName(fields[i]);
                fld->setValue((*it).second);
                i++;
            }


        }
        else if ( aURL.Path == "CheckButtonCmd" )
        {
            auto doc = mxFrame->getController()->getModel();
            Reference< ::com::sun::star::text::XTextDocument > xTextDocument(doc, UNO_QUERY);
            auto xText = xTextDocument->getText();
            auto cursor = xText -> createTextCursor();
            Reference <com::sun::star::beans::XPropertySet> xPropertySet(cursor, UNO_QUERY);
            ::rtl::OUString str;
            sal_Unicode c;
            bool en = false;
            std::size_t cnt = 0;
            std::size_t total_length = (xText->getString()).getLength();
            Reference <::com::sun::star::beans::XPropertySet> xCursorProps(cursor, UNO_QUERY);
            while(cursor->goRight(1, true)) {
                std::size_t curlen = 0;
                auto tmp = (cursor->getString())[curlen];
                while (curlen < (total_length - cnt) && (tmp >='A' && tmp <= 'Z' || tmp >= 'a' && tmp <= 'z' ) ) {
                    cursor->goRight(1, true);
                    curlen++;
                    tmp = (cursor->getString())[curlen];
                }
                if (curlen != (total_length - cnt)) {
                    cursor->goLeft(1, true);
                }
                auto txt = cursor->getString();
                size_t size = txt.getLength();
                for (int i = 0; i < size; i++) {
                    if (txt[i] >='A' && txt[i] <= 'Z' || txt[i] >= 'a' && txt[i] <= 'z') {
                        xCursorProps->setPropertyValue("CharBackColor", Any(0xFF0000));
                        break;
                    }
                }
                if (cursor->goRight(1, true)) {
                    cursor->collapseToEnd();
                    cnt += curlen + 1;
                } else {
                    break;
                }
            }
        }
        else if ( aURL.Path == "ComboboxCmd" )
        {
            // remove the text if it's in our list
            Sequence< NamedValue > aRemoveArgs( 1 );
            aRemoveArgs[0].Name  = rtl::OUString( "Text" );
            aRemoveArgs[0].Value <<= maComboBoxText;
            SendCommand( aURL, ::rtl::OUString( "RemoveEntryText" ), aRemoveArgs, sal_True );

            // add the new text to the start of the list
            Sequence< NamedValue > aInsertArgs( 2 );
            aInsertArgs[0].Name = rtl::OUString( "Pos" );
            aInsertArgs[0].Value <<= sal_Int32( 0 );
            aInsertArgs[1].Name = rtl::OUString( "Text" );
            aInsertArgs[1].Value <<= maComboBoxText;
            SendCommand( aURL, ::rtl::OUString("InsertEntry"), aInsertArgs, sal_True );

            for (auto i = 0; i < lArgs.getLength(); i++)
            {
                if (lArgs[i].Name == "Text") {
                    rtl::OUString sub_num;
                    lArgs[i].Value >>= sub_num;
                    maxlength = sub_num.toInt32();
                }
            }
        }
        else if ( aURL.Path == "InsertEntry" )
        {
            // Retrieve the text argument from the sequence property value
            rtl::OUString aText;
            for ( sal_Int32 i = 0; i < lArgs.getLength(); i++ )
            {
                if ( lArgs[i].Name == "Text" )
                {
                    lArgs[i].Value >>= aText;
                    break;
                }
            }

            // create new URL to address the combox box
            URL aCmdURL;
            aCmdURL.Path = "ComboboxCmd";
            aCmdURL.Protocol = "vnd.demo.complextoolbarcontrols.demoaddon:";
            aCmdURL.Complete = aCmdURL.Path + aCmdURL.Protocol;

            // set the selected item as text into the combobox
            Sequence< NamedValue > aArgs( 1 );
            aArgs[0].Name = "Text";
            aArgs[0].Value <<= aText;
            SendCommand( aCmdURL, ::rtl::OUString( "SetText" ), aArgs, sal_True );
        }
        else if ( aURL.Path == "SpinfieldCmd" )
        {
            lArgs[1].Value >>= numwords;
        }
        else if ( aURL.Path == "DropdownboxCmd" )
        {
            // Retrieve the text argument from the sequence property value
            rtl::OUString aText;
            for ( sal_Int32 i = 0; i < lArgs.getLength(); i++ )
            {
                if ( lArgs[i].Name == "Text" )
                {
                    lArgs[i].Value >>= aText;
                    lArgs[i].Value >>= layout;
                    break;
                }
            }
        }
    }
}

void SAL_CALL BaseDispatch::addStatusListener( const Reference< XStatusListener >& xControl, const URL& aURL ) throw (RuntimeException)
{
    if ( aURL.Protocol == "vnd.demo.complextoolbarcontrols.demoaddon:" )
    {
        if ( aURL.Path == "GenButtonCmd" )
        {
            // just enable this command
            ::com::sun::star::frame::FeatureStateEvent aEvent;
            aEvent.FeatureURL = aURL;
            aEvent.Source = (::com::sun::star::frame::XDispatch*) this;
            aEvent.IsEnabled = mbButtonEnabled;
            aEvent.Requery = sal_False;
            aEvent.State = Any();
            xControl->statusChanged( aEvent );
        }
        else if ( aURL.Path == "StatButtonCmd" )
        {
            // just enable this command
            ::com::sun::star::frame::FeatureStateEvent aEvent;
            aEvent.FeatureURL = aURL;
            aEvent.Source = (::com::sun::star::frame::XDispatch*) this;
            aEvent.IsEnabled = mbButtonEnabled;
            aEvent.Requery = sal_False;
            aEvent.State = Any();
            xControl->statusChanged( aEvent );
        }
        else if ( aURL.Path == "CheckButtonCmd" )
        {
            // just enable this command
            ::com::sun::star::frame::FeatureStateEvent aEvent;
            aEvent.FeatureURL = aURL;
            aEvent.Source = (::com::sun::star::frame::XDispatch*) this;
            aEvent.IsEnabled = mbButtonEnabled;
            aEvent.Requery = sal_False;
            aEvent.State = Any();
            xControl->statusChanged( aEvent );
        }
        else if ( aURL.Path == "ComboboxCmd" )
        {
            // just enable this command
            ::com::sun::star::frame::FeatureStateEvent aEvent;
            aEvent.FeatureURL = aURL;
            aEvent.Source = (::com::sun::star::frame::XDispatch*) this;
            aEvent.IsEnabled = sal_True;
            aEvent.Requery = sal_False;
            aEvent.State = Any();
            xControl->statusChanged( aEvent );
        }
        else if ( aURL.Path == "ToggleDropdownButtonCmd" )
        {
            // A toggle dropdown box is normally used for a group of commands
            // where the user can select the last issued command easily.
            // E.g. a typical command group would be "Insert shape"
            Sequence< NamedValue > aArgs( 1 );

            // send command to set context menu content
            Sequence< rtl::OUString > aContextMenu( 3 );
            aContextMenu[0] = "Ru";
            aContextMenu[1] = "En";
            aContextMenu[2] = "Mix";

            aArgs[0].Name = "List";
            aArgs[0].Value <<= aContextMenu;
            SendCommandTo( xControl, aURL, rtl::OUString( "SetList" ), aArgs, sal_True );

            // send command to check item on pos=0
            aArgs[0].Name = rtl::OUString( "Pos" );
            aArgs[0].Value <<= sal_Int32( 0 );
            SendCommandTo( xControl, aURL, ::rtl::OUString( "CheckItemPos" ), aArgs, sal_True );
        }
        else if ( aURL.Path == "DropdownButtonCmd" )
        {
            // A dropdown box is normally used for a group of dependent modes, where
            // the user can only select one. The modes cannot be combined.
            // E.g. a typical group would be left,right,center,block.
            Sequence< NamedValue > aArgs( 1 );

            // send command to set context menu content
            Sequence< rtl::OUString > aContextMenu( 2 );
            aContextMenu[0] = "Button Enabled";
            aContextMenu[1] = "Button Disabled";

            aArgs[0].Name = "List";
            aArgs[0].Value <<= aContextMenu;
            SendCommandTo( xControl, aURL, rtl::OUString( "SetList" ), aArgs, sal_True );

            // set position according to enable/disable state of button
            sal_Int32 nPos( mbButtonEnabled ? 0 : 1 );

            // send command to check item on pos=0
            aArgs[0].Name = "Pos";
            aArgs[0].Value <<= nPos;
            SendCommandTo( xControl, aURL, ::rtl::OUString( "CheckItemPos" ), aArgs, sal_True );
        }
        else if ( aURL.Path == "SpinfieldCmd" )
        {
            // A spin button
            Sequence< NamedValue > aArgs( 5 );

            // send command to initialize spin button
            aArgs[0].Name = "Value";
            aArgs[0].Value <<= int( 0 );
            aArgs[1].Name = "UpperLimit";
            aArgs[1].Value <<= int( 100000);
            aArgs[2].Name = "LowerLimit";
            aArgs[2].Value <<= int( 0 );
            aArgs[3].Name = "Step";
            aArgs[3].Value <<= int( 1 );
            aArgs[4].Name = "OutputFormat";
            aArgs[4].Value <<= rtl::OUString("%d");

            SendCommandTo( xControl, aURL, rtl::OUString( "SetValues" ), aArgs, sal_True );
        }
        else if ( aURL.Path == "DropdownboxCmd" )
        {
            // A dropdown box is normally used for a group of commands
            // where the user can select one of a defined set.
            Sequence< NamedValue > aArgs( 1 );

            // send command to set context menu content
            Sequence< rtl::OUString > aList( 3 );
            aList[0] = "Ru";
            aList[1] = "En";
            aList[2] = "Mix";

            aArgs[0].Name = "List";
            aArgs[0].Value <<= aList;
            SendCommandTo( xControl, aURL, rtl::OUString( "SetList" ), aArgs, sal_True );
        }

        aListenerHelper.AddListener( mxFrame, xControl, aURL.Path );
    }
}

void SAL_CALL BaseDispatch::removeStatusListener( const Reference< XStatusListener >& xControl, const URL& aURL ) throw (RuntimeException)
{
    aListenerHelper.RemoveListener( mxFrame, xControl, aURL.Path );
}

void SAL_CALL BaseDispatch::controlEvent( const ControlEvent& Event ) throw (RuntimeException)
{
    if ( Event.aURL.Protocol == "vnd.demo.complextoolbarcontrols.demoaddon:" )
    {
        if ( Event.aURL.Path == "ComboboxCmd" )
        {
            // We get notifications whenever the text inside the combobox has been changed.
            // We store the new text into a member.
            if ( Event.Event == "TextChanged" )
            {
                rtl::OUString aNewText;
                sal_Bool      bHasText( sal_False );
                for ( sal_Int32 i = 0; i < Event.aInformation.getLength(); i++ )
                {
                    if ( Event.aInformation[i].Name == "Text" )
                    {
                        bHasText = Event.aInformation[i].Value >>= aNewText;
                        break;
                    }
                }
                if ( bHasText )
                    maComboBoxText = aNewText;
            }
        }
    }
}

BaseDispatch::BaseDispatch( const Reference< XComponentContext > &rxContext,
                            const Reference< XFrame >& xFrame,
                            const ::rtl::OUString& rServiceName )
        : mxContext( rxContext )
        , mxFrame( xFrame )
        , msDocService( rServiceName )
        , mbButtonEnabled( sal_True )
        , layout("Ru")
{
}

BaseDispatch::~BaseDispatch()
{
    mxFrame.clear();
    mxContext.clear();
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
