/**
 * @licence app begin@
 * Copyright (C) 2011-2012  BMW AG
 *
 * This file is part of GENIVI Project Dlt Viewer.
 *
 * Contributions are licensed to the GENIVI Alliance under one or more
 * Contribution License Agreements.
 *
 * \copyright
 * This Source Code Form is subject to the terms of the
 * Mozilla Public License, v. 2.0. If a  copy of the MPL was not distributed with
 * this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * \file searchdialog.h
 * For further information see http://www.genivi.org/.
 * @licence end@
 */
#include "searchtablemodel.h"

#include "fieldnames.h"
#include "dltsettingsmanager.h"
#include "dltuiutils.h"
#include "optmanager.h"


SearchTableModel::SearchTableModel(const QString &,QObject *parent) :
    QAbstractTableModel(parent)
{
    qfile = NULL;
    project = NULL;
    pluginManager = NULL;
}

SearchTableModel::~SearchTableModel()
{

}



QVariant SearchTableModel::data(const QModelIndex &index, int role) const
{
    QDltMsg msg;
    QByteArray buf;

    if (!index.isValid())
        return QVariant();

    if (index.row() >= m_searchResultList.size() && index.row()<0)
        return QVariant();

    if (role == Qt::DisplayRole)
    {
        /* get the message with the selected item id */
        if(!qfile->getMsg(m_searchResultList.at(index.row()), msg))
        {
            if(index.column() == FieldNames::Index)
            {
                return QString("%1").arg((m_searchResultList.at(index.row())));
            }
            else if(index.column() == FieldNames::Payload)
            {
                return QString("!!CORRUPTED MESSAGE!!");
            }
            return QVariant();
        }

        if(DltSettingsManager::getInstance()->value("startup/pluginsEnabled", true).toBool())
            pluginManager->decodeMsg(msg,!OptManager::getInstance()->issilentMode());

        switch(index.column())
        {
        case FieldNames::Index:
            /* display index */            
            return QString("%1").arg((m_searchResultList.at(index.row())));
        case FieldNames::Time:
            if( project->settings->automaticTimeSettings == 0 )
               return QString("%1.%2").arg(msg.getGmTimeWithOffsetString(project->settings->utcOffset,project->settings->dst)).arg(msg.getMicroseconds(),6,10,QLatin1Char('0'));
            else
               return QString("%1.%2").arg(msg.getTimeString()).arg(msg.getMicroseconds(),6,10,QLatin1Char('0'));
        case FieldNames::TimeStamp:
            return QString("%1.%2").arg(msg.getTimestamp()/10000).arg(msg.getTimestamp()%10000,4,10,QLatin1Char('0'));
        case FieldNames::Counter:
            return QString("%1").arg(msg.getMessageCounter());
        case FieldNames::EcuId:
            return msg.getEcuid();
        case FieldNames::AppId:
            switch(project->settings->showApIdDesc){
            case 0:
                return msg.getApid();
                break;
            case 1:
                  for(int num = 0; num < project->ecu->topLevelItemCount (); num++)
                   {
                    EcuItem *ecuitem = (EcuItem*)project->ecu->topLevelItem(num);
                    for(int numapp = 0; numapp < ecuitem->childCount(); numapp++)
                    {
                        ApplicationItem * appitem = (ApplicationItem *) ecuitem->child(numapp);
                        if(appitem->id == msg.getApid() && !appitem->description.isEmpty())
                        {
                           return appitem->description;
                        }
                    }
                   }
                  return QString("Apid: %1 (No description)").arg(msg.getApid());
                break;
             default:
                return msg.getApid();
            }
        case FieldNames::ContextId:
            switch(project->settings->showCtIdDesc){
            case 0:
                return msg.getCtid();
                break;
            case 1:

                  for(int num = 0; num < project->ecu->topLevelItemCount (); num++)
                   {
                    EcuItem *ecuitem = (EcuItem*)project->ecu->topLevelItem(num);
                    for(int numapp = 0; numapp < ecuitem->childCount(); numapp++)
                    {
                        ApplicationItem * appitem = (ApplicationItem *) ecuitem->child(numapp);
                        for(int numcontext = 0; numcontext < appitem->childCount(); numcontext++)
                        {
                            ContextItem * conitem = (ContextItem *) appitem->child(numcontext);

                            if(appitem->id == msg.getApid() && conitem->id == msg.getCtid()
                                    && !conitem->description.isEmpty())
                            {
                               return conitem->description;
                            }
                        }
                    }
                   }
                  return  QString("Ctid: %1 (No description)").arg(msg.getCtid());
                break;
             default:
                return msg.getCtid();
            }
        case FieldNames::SessionId:
            switch(project->settings->showSessionName){
            case 0:
                return QString("%1").arg(msg.getSessionid());
                break;
            case 1:
                if(!msg.getSessionName().isEmpty())
                   return msg.getSessionName();
               else
                   return QString("%1").arg(msg.getSessionid());
                break;
             default:
                return QString("%1").arg(msg.getSessionid());
            }
        case FieldNames::Type:
            return msg.getTypeString();
        case FieldNames::Subtype:
            return msg.getSubtypeString();
        case FieldNames::Mode:
            return msg.getModeString();
        case FieldNames::ArgCount:
            return QString("%1").arg(msg.getNumberOfArguments());
        case FieldNames::Payload:
            /* display payload */
            return msg.toStringPayload();
        default:
            if (index.column()>=FieldNames::Arg0)
            {
                int col=index.column()-FieldNames::Arg0; //arguments a zero based
                QDltArgument arg;
                if (msg.getArgument(col,arg))
                {
                    return arg.toString();
                }
                else
                 return QString(" - ");

            }

        }
    }

    if ( role == Qt::ForegroundRole ) {        
        //always white
        return QVariant(QBrush(QColor(0,0,0)));
    }

    if ( role == Qt::BackgroundRole ) {
        //always black
        return QVariant(QBrush(QColor(255,255,255)));
    }


    if ( role == Qt::TextAlignmentRole ) {
       switch(index.column())
       {
           case FieldNames::Index:
               return QVariant(Qt::AlignRight  | Qt::AlignVCenter);
           case FieldNames::Time:
               return QVariant(Qt::AlignCenter | Qt::AlignVCenter);
           case FieldNames::TimeStamp:
               return QVariant(Qt::AlignRight  | Qt::AlignVCenter);
           case FieldNames::Counter:               
               return QVariant(Qt::AlignCenter | Qt::AlignVCenter);//don't show that in the search field
           case FieldNames::EcuId:
               return QVariant(Qt::AlignCenter | Qt::AlignVCenter);
           case FieldNames::AppId:
               return QVariant(Qt::AlignCenter | Qt::AlignVCenter);
               switch(project->settings->showApIdDesc){
               case 0:
                   return QVariant(Qt::AlignCenter | Qt::AlignVCenter);
                   break;
               case 1:
                   return QVariant(Qt::AlignLeft | Qt::AlignVCenter);
                   break;
               default:
                   return QVariant(Qt::AlignLeft | Qt::AlignVCenter);
                   break;
               }
           case FieldNames::ContextId:
               switch(project->settings->showCtIdDesc){
               case 0:
                   return QVariant(Qt::AlignCenter | Qt::AlignVCenter);
                   break;
               case 1:
                   return QVariant(Qt::AlignLeft | Qt::AlignVCenter);
                   break;
               default:
                   return QVariant(Qt::AlignLeft | Qt::AlignVCenter);
                   break;
               }
           case FieldNames::Type:
               return QVariant(Qt::AlignCenter | Qt::AlignVCenter);
           case FieldNames::Subtype:
               return QVariant(Qt::AlignCenter | Qt::AlignVCenter);
           case FieldNames::Mode:
               return QVariant(Qt::AlignCenter | Qt::AlignVCenter);
           case FieldNames::ArgCount:
               return QVariant(Qt::AlignRight  | Qt::AlignVCenter);
           case FieldNames::Payload:
               return QVariant(Qt::AlignLeft   | Qt::AlignVCenter);
       }
   }

    return QVariant();
}

QVariant SearchTableModel::headerData(int section, Qt::Orientation orientation,
                               int role) const
{
   if (role != Qt::DisplayRole)
       return QVariant();

   if (orientation == Qt::Horizontal)
   {
       return FieldNames::getName((FieldNames::Fields)section, project->settings);
   }

   return QVariant();
}

int SearchTableModel::rowCount(const QModelIndex & /*parent*/) const
{
    return get_SearchResultListSize();
}

void SearchTableModel::modelChanged()
{    
    index(0, 1);
    index(m_searchResultList.size()-1, 0);
    index(m_searchResultList.size()-1, columnCount() - 1);
    emit(layoutChanged());
}

int SearchTableModel::columnCount(const QModelIndex & /*parent*/) const
{
    return DLT_VIEWER_SEARCHCOLUMN_COUNT+project->settings->showArguments;
}

void SearchTableModel::clear_SearchResults()
{
    m_searchResultList.clear();
}

void SearchTableModel::add_SearchResultEntry(unsigned long entry)
{
    m_searchResultList.append(entry);
}


bool SearchTableModel::get_SearchResultEntry(int position, unsigned long &entry)
{
    if (position > m_searchResultList.size() || 0 > position )
    {
        return false;
    }

    entry = m_searchResultList.at(position);
    return true;
}


int SearchTableModel::get_SearchResultListSize() const
{
    return m_searchResultList.size();
}