#ifndef __SQL_INSERT__
#define __SQL_INSERT__
#include "sqlcmdType.hpp"
namespace SQLCMD
{

    struct Insert
    {
        std::string& res_;
        std::string q_f;   
        std::string q_v;       
        short count;
        size_t end_,orig_end;   
          Insert(std::string& res,std::string tablename , size_t end):res_(res),end_(end - 1 ),count(0),q_f("insert into " + tablename + "("),q_v("VALUES(")
            {}

            bool operator()(pair mp)
            {
                q_f+=mp.first;
                if (mp.second.type == valdesc::TEXT)
                    q_v+="'"+mp.second.value+"'";
                else if(mp.second.type == valdesc::NUMERIC)
                    q_v+=mp.second.value;
                count = (( 0 == count ) && ( 1 == end_ ) && (orig_end == end_))? end_ : count;
                if (count < end_ )
                {
                    q_f+=",";
                    q_v+=",";

                }
                else
                {
                    q_f+=")";
                    q_v+=")";
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
        short count;
        size_t end_, orig_end;
        //insert into layers(layerGuid,hashId) 
        //'testX0', 1010221 WHERE NOT EXISTS(select 1 from layers where layerId = 1)
        InsertIfNotExist(std::string& res, std::string tablename, size_t end) :res_(res), end_(end - 1), count(0), q_where(), q_f("insert into " + tablename + "("), q_v("SELECT "), tableName(tablename)
        {}
        InsertIfNotExist(const InsertIfNotExist& inoe) :count(inoe.count),res_(inoe.res_), end_(inoe.end_), q_f(inoe.q_f), q_v(inoe.q_v), tableName(inoe.tableName) {}

        bool operator()(pair mp)
        {
            if (mp.second.m_selectable)
                q_f += mp.first;

            if (mp.second.m_selectable)
            {
                if (mp.second.type == valdesc::TEXT)
                    q_v += "'" + mp.second.value + "'";
                else if (mp.second.type == valdesc::NUMERIC)
                    q_v += mp.second.value;
            }
            count = ((0 == count) && (1 == end_) && (orig_end == end_)) ? end_ : count;
            
            if ((mp.second.logic == valdesc::WHERE_ON) ||
                (mp.second.logic == valdesc::WHERE_AND) ||
                (mp.second.logic == valdesc::WHERE_OR) ||
                (mp.second.logic == valdesc::valdesc::WHERE_IN))
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
                    + tableName + ((mp.second.type == valdesc::TEXT) ? " like " : " = ")
                    + ((mp.second.type == valdesc::TEXT) ? "'" : "")
                    + mp.second.value + ((mp.second.type == valdesc::TEXT) ? "'" : "");
            }
            if (count < end_)
            {
                if (mp.second.m_selectable) {
                    q_f += ",";
                    q_v += ",";
                }

            }
            else
            {
                if (!q_where.empty())
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
       // std::enable_if<>
       // void setEnd()

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


}
#endif