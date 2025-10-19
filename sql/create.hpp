#ifndef __UPDATE_SQL__
#define __UPDATE_SQL__
#include <string>
#include <iostream>
#include <map>
#include <vector> 
#include <algorithm>
#include "sqlcmdType.hpp"
namespace SQLCMD
{
    template
        <
         bool isTempTable = false,
         bool ifNotExists = false
        >
    struct Create
    {
        enum { SELECTABLE_SUPPORT = false };
        std::string AsTable;
        std::string& res_;
        std::string q_f;   
        std::string q_v;    
        short count;
        size_t end_;     
        Create(std::string& res,std::string tablename , size_t end)
        :res_(res),end_(end - 1 ),count(0),q_f("CREATE "+ std::string((isTempTable)?"TEMP ":"") + "TABLE " +std::string((ifNotExists)?"if not exists":"") + " " + tablename), q_v("(")
        {
    
        }
    
        bool operator()(pair mp)
        {

            q_v+= (
                (mp.second.type == SQLCMD::selector_cmd_type::TEXT) && (mp.second.logic != valdesc::WHERE_ON) &&
                (mp.second.logic != valdesc::WHERE_AND) && (mp.second.logic != valdesc::WHERE_OR) && (mp.second.logic != valdesc::AS_TABLE)
                )?( mp.first + "'" + mp.second.value + "'" ):(mp.first+" "+mp.second.value + " ");

            if (count < end_ - 1)
            {
               q_v+=",";
            }
            
            if ((mp.second.logic == valdesc::AS_TABLE)&&(AsTable.empty()))
                AsTable = " AS " + mp.second.value;
  
            
            if (count >= end_)
            {
                res_ = q_f + q_v + ")";    
            }       
            if ( !AsTable.empty() )
            {
                res_ = q_f + AsTable;
            }
            ++count;
            return true;
        }    

  
        std::string get(void)
        {
            return res_;
        }
        
        std::string get(size_t limit)
        {
            
            return res_;
        }
        
        void setEnd(size_t szend)
        {
            end_ = (szend > 1)? -- szend : szend ;    
        }    
    };
    
  
}

#endif