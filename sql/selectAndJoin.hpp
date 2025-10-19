
#ifndef _SELECT_AND_JOIN_
#define _SELECT_AND_JOIN_
#include "sqlcmdType.hpp"
#include <optional>


namespace SQLCMD
{


    template
    <
    typename T,
    size_t count
    >
    struct EmptySpace 
    {
        enum 
        {
            IsEmpty = true,
            TID = 0,
            COUNT = count
        };
    
        EmptySpace(){}
    
    
        EmptySpace(std::string tablename_){}
    
        std::string
        getTableName()
        {
            return ("");
        }
    
        void
        linkJoin(std::string fieldFrom,std::string fieldTo)
        {}
    
    
    
        void linkTableTo(std::string _tableLinked)
        {}
    
    
        void linkTableTo(std::string _tablename,std::string _tableLinked)
        {}
        
        std::string
        get()
        {
            return ("");
        }   
    };


    enum JOIN_TYPE_E
    {
        INNER_JOIN = 0,
        LEFT_JOIN  = 1,
        RIGHT_JOIN = 2
    };
    
    template
    <
    typename T,
    size_t JOIN_TYPE,
    size_t TYPE_ID
    >
    struct Join 
    {
    enum
    {
        IsEmpty = false,
        TID = TYPE_ID
    };

    protected:
    std::string basePart;
    const std::string types_[3]; 
    typename T::type t_out;
    std::string tablename;
    std::string tableLinked;
    std::string _tokenField;
    std::string _baseField;
    Join():t_out(T::get()),basePart("inner join")
    {}
    Join(std::string tablename_):t_out(T::get()),types_{"inner","left","right"},tablename(tablename_)
    {
        basePart = types_[JOIN_TYPE] + std::string(" join");
    }

    std::string
    getTableName()
    {
        return (tablename);
    }

    Join(const Join& j):basePart(j.basePart),tablename(j.tablename),tableLinked(j.tableLinked),t_out(j.t_out)
    {}

    void
    linkJoin(std::string fieldFrom,std::string fieldTo)
    {
        _baseField  = fieldFrom;  
        _tokenField = fieldTo;
    }



    void linkTableTo(std::string _tableLinked)
    {
        tableLinked = _tableLinked;
    }


    void linkTableTo(std::string _tablename,std::string _tableLinked)
    {
        tableLinked = _tableLinked;
        tablename = _tablename;
    }


    std::string
    get()
    {
        std::string str("");
        for (pair const p : t_out)
        {
            if (_tokenField[0] == '#')
            {
                _tokenField.erase(0, 1);
                if (p.first.compare(_tokenField) == 0)
                {
                    str += " " + tableLinked + " on "  + tableLinked + "." + _baseField + " = " + tablename + "." + p.first;
                }
                continue;
            }
            if (p.first.compare(_tokenField)==0)//(p.first.compare(_tokenField) == 0)
            {
                str += " " + tableLinked + " on " + tablename + "." + _baseField + " = " + tableLinked + "." + _baseField ;
            }

        }
        basePart += str;
        return (basePart);
    }

    ~Join()
    {
        T::clear();
    }
    };
    template
        <
        typename typeInnerJoin_0 = EmptySpace<wrapper<SQLCMD::selector_type>, 0>,
        typename typeInnerJoin_1 = EmptySpace<wrapper<SQLCMD::selector_type>, 1>,
        typename typeInnerJoin_2 = EmptySpace<wrapper<SQLCMD::selector_type>, 2>
        >
        struct Select :public typeInnerJoin_0, typeInnerJoin_1, typeInnerJoin_2
    {
        enum { SELECTABLE_SUPPORT = true };
        std::string _INTERNAL_JOIN_DEBUG_;
        std::string& res_;
        std::string q_f;
        std::string q_v;
        std::string q_where;
        short count;
        //    bool whereOn;
        enum { JOINCOUNT = int(!typeInnerJoin_0::IsEmpty) + int(!typeInnerJoin_1::IsEmpty) + int(!typeInnerJoin_2::IsEmpty) };
        std::string tablename;
        std::string joinedTable[(JOINCOUNT == 0) ? 1 : JOINCOUNT];
        size_t end_;
        short int unselectable_detected;
        std::string Join[(JOINCOUNT ==0)?1: JOINCOUNT];
        std::vector<std::string> _joins;
        std::string fieldFrom[(JOINCOUNT == 0) ? 1 : JOINCOUNT];
        std::string fieldTo[(JOINCOUNT == 0) ? 1 : JOINCOUNT];
        std::string InPredicat;
        Select(std::string& res, std::string tablename_, std::vector<std::string> joinedTable_, size_t end) :typeInnerJoin_0(tablename_), typeInnerJoin_1(tablename_), typeInnerJoin_2(tablename_), res_(res), end_(end), count(0),
          q_f("SELECT "), q_v("FROM " + tablename_), tablename(tablename_), q_where(""), unselectable_detected(0), InPredicat("") , _INTERNAL_JOIN_DEBUG_("OFF")
        {

            if (!typeInnerJoin_0::IsEmpty)
                joinedTable[typeInnerJoin_0::TID] = joinedTable_[typeInnerJoin_0::TID];
            if (!typeInnerJoin_1::IsEmpty)
                joinedTable[typeInnerJoin_1::TID] = joinedTable_[typeInnerJoin_1::TID];
            if (!typeInnerJoin_2::IsEmpty)
                joinedTable[typeInnerJoin_2::TID] = joinedTable_[typeInnerJoin_2::TID];
        }

        Select(std::string& res, std::string tablename_,  size_t end) :typeInnerJoin_0(tablename_), typeInnerJoin_1(tablename_), typeInnerJoin_2(tablename_), res_(res), end_(end), count(0),
            q_f("SELECT "), q_v("FROM " + tablename_), tablename(tablename_), q_where(""), unselectable_detected(0)
        { }
        /*
        
        */


        void linkJoin(std::vector<std::string> fieldFrom, std::vector<std::string> fieldTo)
        {
            for (size_t i = 0; i < JOINCOUNT; ++i)
            {
                this->fieldFrom[i] = fieldFrom[i];
                this->fieldTo[i] = fieldTo[i];
            }
        }


        void addInParam(const std::vector <valdesc> InSeq)
        {
            this->InPredicat = "IN(";
            for (size_t i = 0; i < InSeq.size(); ++i)
            {
                if (InSeq[i].type == SQLCMD::selector_cmd_type::TEXT)
                {
                    this->InPredicat += "'" + InSeq[i].value + "'"+((i < (InSeq.size() - 1)) ? "," : "");/*"'" + ","*/;
                }
                else
                    this->InPredicat += InSeq[i].value + ((i < (InSeq.size() - 1)) ? "," : "");
            }
            this->InPredicat += ")";
        }
        void linkToTable()
        {

        }

        Select(const Select& s) :res_(s.res_), end_(s.end_),
            count(s.count), q_f(s.q_f), q_v(s.q_v), tablename(s.tablename),InPredicat(s.InPredicat),_joins(s._joins), _INTERNAL_JOIN_DEBUG_(s._INTERNAL_JOIN_DEBUG_)
        {
            for (size_t i = 0; i < JOINCOUNT; ++i)
            {
                this->fieldFrom[i] = s.fieldFrom[i];
                this->fieldTo[i] = s.fieldTo[i];
                this->Join[i] = s.Join[i];
                this->joinedTable[i] = s.joinedTable[i];
            }
        }


        void setEnd(size_t szend)
        {
            end_ = (szend > 1) ? --szend : szend;
        }

        /*
        
        допилить совместное использхоание IN OR и AND.
        
        */

        std::vector<std::string> prepareJoins(const pair& mp)
        {
         
          if (!typeInnerJoin_0::IsEmpty)
            typeInnerJoin_0::linkTableTo(tablename, joinedTable[typeInnerJoin_0::TID]);

          if (!typeInnerJoin_1::IsEmpty)
            typeInnerJoin_1::linkTableTo(tablename, joinedTable[typeInnerJoin_1::TID]);

          if (!typeInnerJoin_2::IsEmpty)
            typeInnerJoin_2::linkTableTo(tablename, joinedTable[typeInnerJoin_2::TID]);

          for (size_t i = 0; i <= JOINCOUNT; ++i)
          {
            if (fieldFrom[i].compare(mp.first) != 0)
              continue;

            if ((!typeInnerJoin_0::IsEmpty) && (typeInnerJoin_0::TID == i))
            {
              typeInnerJoin_0::linkJoin(fieldFrom[i], this->fieldTo[i]);
              /*
              Вот тут формируется и крепится join
              */
              Join[i] = typeInnerJoin_0::get();
              _joins.push_back(Join[i]);
            }
            if ((!typeInnerJoin_1::IsEmpty) && (typeInnerJoin_1::TID == i))
            {
              typeInnerJoin_1::linkJoin(fieldFrom[i], this->fieldTo[i]);
              Join[i] = typeInnerJoin_1::get();
              _joins.push_back(Join[i]);
            }

            if ((!typeInnerJoin_2::IsEmpty) && (typeInnerJoin_2::TID == i))
            {
              typeInnerJoin_2::linkJoin(fieldFrom[i], this->fieldTo[i]);
              Join[i] = typeInnerJoin_2::get();
              _joins.push_back(Join[i]);
            }
          }
          return _joins;

        }
         



        bool operator()(pair mp)
        {
 
            
            std::vector<std::string> joins = prepareJoins(mp);
            
            
            std::string tmp("");

            if (!mp.second.belongedToTables.empty())
            {
                for (std::string bt : mp.second.belongedToTables)
                {
                    if (mp.second.m_selectable)                
                        tmp += ((!tmp.empty()) ? "," : "") + 
                            (
                            (mp.second.takeAsPredicate == SQLCMD::valdesc::MAX) ? std::string("MAX(") + bt + "." + mp.first + ") AS " + mp.first + " " :
                            (mp.second.takeAsPredicate == SQLCMD::valdesc::MIN) ? std::string("MIN(") + bt + "." + mp.first + ") AS " + mp.first + " " :
                            (mp.second.takeAsPredicate == SQLCMD::valdesc::SUM) ? std::string("SUM(") + bt + "." + mp.first + ") AS " + mp.first + " " :
                            (mp.second.takeAsPredicate == SQLCMD::valdesc::DISTINCT) ? std::string("DISTINCT ") + bt + "." + mp.first + " AS " + mp.first + " " :
                                bt + "." + mp.first);
                    
                }
            }
            else
            {
                if (mp.second.m_selectable)  
                    tmp += (
                            (mp.second.takeAsPredicate == SQLCMD::valdesc::MAX)     ? std::string("MAX(")      + tablename + "." + mp.first + ") AS " + mp.first + " " :
                            (mp.second.takeAsPredicate == SQLCMD::valdesc::MIN)     ? std::string("MIN(")      + tablename + "." + mp.first + ") AS " + mp.first + " " :
                            (mp.second.takeAsPredicate == SQLCMD::valdesc::SUM)     ? std::string("SUM(")      + tablename + "." + mp.first + ") AS " + mp.first + " " :
                            (mp.second.takeAsPredicate == SQLCMD::valdesc::DISTINCT) ? std::string("DISTINCT ") + tablename + "." + mp.first + " AS " + mp.first  + " " :
                        tablename + "." + mp.first);
                
            }

            switch (mp.second.logic)
            {
              case valdesc::__FILLER__:
              {
                tmp = (mp.second.m_selectable) ? mp.second.belongedToTables[0] + "." + mp.first : "";
              }
              break;
              case valdesc::__AS__:
              {
                tmp = (mp.second.m_selectable) ? mp.second.value + " AS " + mp.first : "";
              }
              break;
              default:
                break;
            }

      //      tmp = ((mp.second.logic == valdesc::__FILLER__) && (mp.second.m_selectable)) ? mp.second.value : "";
      //      tmp = ((mp.second.logic == valdesc::__AS__) && (mp.second.m_selectable)) ? mp.second.value + " AS " + mp.first : "";
            q_f += ((count > 0)&&(mp.second.m_selectable))?("," + tmp) : tmp;

            const int MINIMAL_SELECTED_FIELDS_COUNT = 1;
           // q_f += ((count < end_) && (mp.second.m_selectable) && (end_ > MINIMAL_SELECTED_FIELDS_COUNT)) ? "," : "";
            
            if ((mp.second.logic == valdesc::WHERE_ON)  ||
                (mp.second.logic == valdesc::WHERE_AND) ||
                (mp.second.logic == valdesc::WHERE_OR)  ||
                (mp.second.logic == valdesc::valdesc::WHERE_IN)||
                (mp.second.logic == valdesc::__FILLER__)
              )
            {
 
                if (q_where.empty())
                {
                    q_where = " where ";
                }
                std::string tableName = "";
                std::string _tableName = "";
                if (!mp.second.belongedToTables.empty())
                {
                    for (std::string bt : mp.second.belongedToTables)
                    {
                        if (mp.second.m_selectable)
                        {
                            tableName += " ," + bt + "." + mp.first;
                        }
                        _tableName += bt + "." + mp.first;

                    }
                }
                else
                {
                    tableName = tablename + "." + mp.first;
                    _tableName = tablename + "." + mp.first;
                }

                q_where += ((count >= 0) && ((mp.second.logic == valdesc::WHERE_IN)) ? tableName + " " + InPredicat : "");
                if (mp.second.logic != valdesc::WHERE_IN)
                {
                  q_where += ((mp.second.logic == valdesc::WHERE_AND) ? " AND " : (mp.second.logic == valdesc::WHERE_OR) ? " OR " : " ")
                    + _tableName + ((mp.second.type == SQLCMD::selector_cmd_type::TEXT) && (mp.second.logic != valdesc::__FILLER__) ? " like " : " = ")
                    + ((mp.second.type == SQLCMD::selector_cmd_type::TEXT) && (mp.second.logic != valdesc::__FILLER__) ? "'" : "")
                      + ((mp.second.logic == valdesc::__FILLER__)? tablename +"." + mp.second.value : mp.second.value)
                      + ((mp.second.type == SQLCMD::selector_cmd_type::TEXT) && (mp.second.logic != valdesc::__FILLER__) ? "'" : "");
                }
            }
        
//
//Разобраться блять с джойнами!
//
           
            count += mp.second.m_selectable;
          //  if (end_ == count)
          //    std::cout << "test!" << std::endl;
           
            if (joins.size() < JOINCOUNT)
              return false;

            std::string _join;
            for (auto v:joins)
              _join += " " + v;

            if (!_INTERNAL_JOIN_DEBUG_.compare("ON"))
            {
              std::cout << "join " << _join << std::endl;
              getc(stdin);
            }
            res_ = q_f + " " + q_v + " " + _join  + q_where;

            
            return true;
        }

        void setDebug(void)
        {
          _INTERNAL_JOIN_DEBUG_ = "ON";
        }

        std::string get(void)
        {
            return res_;
        }


        std::string get(size_t limit_)
        {
            return res_ + " LIMIT " + std::to_string(limit_);
        }

    };
           
    typedef Join< wrapper<SQLCMD::selector_type>,JOIN_TYPE_E::INNER_JOIN,0> InnerJoinId0; 
    typedef Join< wrapper<SQLCMD::selector_type>,JOIN_TYPE_E::INNER_JOIN,1> InnerJoinId1; 
    typedef Join< wrapper<SQLCMD::selector_type>,JOIN_TYPE_E::INNER_JOIN,2> InnerJoinId2; 
    typedef Join< wrapper<SQLCMD::selector_type>, JOIN_TYPE_E::LEFT_JOIN, 0> leftJoin0;
};
#endif