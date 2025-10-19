#ifndef __SQL_INSERT__
#define __SQL_INSERT__
#include "sqlcmdType.hpp"
namespace SQLCMD
{
   // enum selector_cmd_type { NONE = 10003, TEXT = 1001, NUMERIC = 1002, TIME_NOW = 1003 };
    template
        <
            bool valuesOn = true
        >
    struct Insert_
    {
        enum {SELECTABLE_SUPPORT = false};
        std::string& res_;
        std::string q_f;   
        std::string q_v;       
        size_t count;
        size_t end_,orig_end;   
            Insert_(std::string& res,std::string tablename , size_t end):res_(res),end_(end - 1 ),count(0),q_f("insert into " + tablename + "("),
                q_v( (valuesOn)?"VALUES(":"")
            {}

            bool operator()(pair mp)
            {
 
                if (mp.second.m_selectable)
                    q_f += mp.first;

                if (mp.second.m_selectable)
                    if ((mp.second.type == SQLCMD::selector_cmd_type::TEXT)&& (!q_v.empty()))
                            q_v += (!q_v.empty()) ? "'" + mp.second.value + "'" : "";
                        else if ((mp.second.type == SQLCMD::selector_cmd_type::NUMERIC) ||
                        (mp.second.type == SQLCMD::selector_cmd_type::TIME_NOW))
                            q_v += (!q_v.empty()) ? mp.second.value : "";

                count = (( 0 == count ) && ( 1 == end_ ) && (orig_end == end_))? end_ : count;
                if (count < end_ )
                {
                    if (mp.second.m_selectable)
                    {
                        q_f += ",";
                        q_v += (!q_v.empty())?",":"";
                    }
                }
                else
                {
                    q_f+=")";
                    q_v+= (!q_v.empty()) ? ")":"";
                }
                if (count >= end_)
                    res_ = q_f + q_v;
                ++count;
                return true;
            }
            
            void setEnd(size_t szend)
            {
                orig_end = szend;    
                end_ =  (szend > 1)? -- szend : szend;     
            }
            
            std::string get(void)
            {   
                return res_;
            }
            
            std::string get(size_t limit)
            {     
                return res_;
            }
    }; 

    struct InsertIfNotExist
    {
        std::string q_where;
        std::string& res_;
        std::string q_f;
        std::string q_v;
        std::string tableName;
        size_t count;
        size_t end_, orig_end;
        enum { SELECTABLE_SUPPORT = true };
 
        InsertIfNotExist(std::string& res, std::string tablename, size_t end) :res_(res), end_(end - 1), count(0), q_where(), q_f("insert into " + tablename + "("), q_v(" SELECT "), tableName(tablename)
        {}      
        InsertIfNotExist(const InsertIfNotExist& inoe) :count(inoe.count),res_(inoe.res_), end_(inoe.end_), q_f(inoe.q_f), q_v(inoe.q_v), tableName(inoe.tableName) 
        {}

        bool operator()(pair mp)
        {
            if (mp.second.m_selectable)
                q_f += mp.first;

            if (mp.second.m_selectable)
            {
                if (mp.second.type == SQLCMD::selector_cmd_type::TEXT)
                    q_v += "'" + mp.second.value + "'";
                else if ((mp.second.type == SQLCMD::selector_cmd_type::NUMERIC) ||
                    (mp.second.type == SQLCMD::selector_cmd_type::TIME_NOW))
                    q_v += mp.second.value;
            }
            count = ((0 == count) && (1 == end_) && (orig_end == end_)) ? end_ : count;
            
            if ((mp.second.logic == SQLCMD::valdesc::WHERE_ON) ||
                (mp.second.logic == SQLCMD::valdesc::WHERE_AND) ||
                (mp.second.logic == SQLCMD::valdesc::WHERE_OR) ||
                (mp.second.logic == SQLCMD::valdesc::valdesc::WHERE_IN))
            {
                if (q_where.empty())
                {
                    q_where = " WHERE NOT EXISTS (SELECT 1 FROM "+ tableName +" WHERE ";
                }
                std::string fieldName = mp.first;
                fieldName.erase(remove(fieldName.begin(), fieldName.end(), '#'), fieldName.end());
                //std::replace(fieldName.begin(), fieldName.end(), '#', ' ');
                tableName = tableName + "." + fieldName;

                q_where += ((mp.second.logic == valdesc::WHERE_AND) ? " AND " : (mp.second.logic == valdesc::WHERE_OR) ? " OR " : " ")
                    + tableName + ((mp.second.type == SQLCMD::selector_cmd_type::TEXT) ? " like " : " = ")
                    + ((mp.second.type == SQLCMD::selector_cmd_type::TEXT) ? "'" : "")
                    + mp.second.value + ((mp.second.type == SQLCMD::selector_cmd_type::TEXT) ? "'" : "");
            }
            if (count < end_)
            {
                if (mp.second.m_selectable) {
                    q_f += ",";
                    q_v += ",";
                }

            }
            else if(count >= end_ )
            {
                if (!q_where.empty())
                    if (std::count(q_where.begin(), q_where.end(),'(') > std::count(q_where.begin(), q_where.end(), ')'))
                        q_where += ")";

                if (std::count(q_f.begin(), q_f.end(), '(') != std::count(q_f.begin(), q_f.end(), ')'))
                    q_f += ")";
            }

            count += mp.second.m_selectable;
            res_ = q_f + q_v + q_where;
            return true;
        }

        void setEnd(size_t szend)
        {
            orig_end = szend;
            end_ = (szend > 1) ? --szend : szend;
        }
 
        std::string get(void)
        {
          
            return res_;
        }

        std::string get(size_t limit)
        {
            res_ = q_f + q_v + q_where;
            return res_;
        }
    };
    typedef Insert_<> Insert;
    typedef Insert_<false> halfInsert;

}
#endif