//#ifndef __UPDATE_SQL__
//#define __UPDATE_SQL__
#ifndef _UPDATE_SQL_
#define _UPDATE_SQL_
#include <string>
#include <iostream>
#include <map>
#include <vector> 
#include <algorithm>
#include "sqlcmdType.hpp"
namespace SQLCMD
{
    struct Update
    {
        enum {SELECTABLE_SUPPORT = true};
        std::string q_where;
        std::string& res_;
        std::string q_f;   
        std::string q_v;    
        short q_v_count;
        short count;
        size_t end_;     
        Update(std::string& res,std::string tablename , size_t end)
        :res_(res),end_(end - 1 ), q_v_count(0), count(0),q_f("UPDATE "+tablename),q_v(" SET ")
        ,q_where("")
        {
    
        }
    
        bool operator()(pair mp)
        {

            if (
                (mp.second.logic != valdesc::WHERE_ON) &&
                (mp.second.logic != valdesc::WHERE_AND) && 
                (mp.second.logic != valdesc::WHERE_OR)&&
                (mp.second.logic != valdesc::WHERE_EXISTS)

              )
            {
           
                q_v += mp.first + std::string("="); 
                if (mp.second.type == SQLCMD::selector_cmd_type::TEXT)
                {
                  q_v += (mp.second.logic != valdesc::__FILLER__) ? "'" + mp.second.value + "'" : "(" + mp.second.value + ")";
                }
                else
                {
                  q_v += mp.second.value;
                }
                q_v += ((q_v_count < end_)&&(end_ > 1))?",":" ";
                ++q_v_count;
              
            }   
          

            if ((mp.second.logic == valdesc::WHERE_ON) || (mp.second.logic == valdesc::WHERE_AND) || (mp.second.logic == valdesc::WHERE_OR)) /*&& (mp.second.value.compare("NONE")!=0))*/
            {
                if (q_where.empty())
                    q_where = " where ";

                q_where += ((count > 0)? ((mp.second.logic == valdesc::WHERE_AND)?" AND ":((mp.second.logic == valdesc::WHERE_OR)?" OR ":" ")):"")
                + mp.first+((mp.second.type == SQLCMD::selector_cmd_type::TEXT)?" like ":" = ") + ((mp.second.type == SQLCMD::selector_cmd_type::TEXT)?"'":"") 
                + mp.second.value + ((mp.second.type == SQLCMD::selector_cmd_type::TEXT)?"'":"");
               
            }
          
            if (mp.second.logic == valdesc::WHERE_EXISTS)
            {
               q_where = "WHERE EXISTS(" + mp.second.value +")";
            }

            if (count >= end_)
            {
                res_ = q_f + q_v + q_where;
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