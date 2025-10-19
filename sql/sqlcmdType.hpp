#ifndef __SQLCMDTYPE__
#define __SQLCMDTYPE__
#include <iostream>
#include <map>
#include <string>
#include <sstream>
#include <iomanip>
#include <vector> 
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
namespace SQLCMD
{
    namespace types
    {
        
        typedef struct _nulltype {}  nullType;
        typedef unsigned long long DWORD64;
        typedef unsigned long long ul_long;
        typedef unsigned long ulong;
        typedef unsigned int  ui_int;
        struct  time_tq{};
        typedef time_tq timeDef;
        // time_tq timeDef() { return time_tq(); };
    
    }

    namespace utils
    {
        std::string
            Newuid()
        {
            boost::uuids::uuid uid_ = boost::uuids::random_generator()();

            auto t = boost::uuids::to_string(uid_);
            return (t);
        }

        types::ul_long
            str_hash(std::string str)
        {
            types::ul_long ret = 0;
            types::ul_long l = str.length();
            for (types::ul_long i = 0, j = l; i < j; ++i, j--)
                ret += str[i] * i + str[j] * j;
            return (ret);
        }
        
        types::ul_long
            getHashId(void)
        {
            return (str_hash(Newuid()));
        }


    }


  

    template
    <
        typename T
    >
    struct
    wrapper    
    {
           typedef T type;
           static
           void
           add(const T& t)
           {
                value = t;
           }
           static
           void
           clear()
           {
                value.clear();
           }
           
           static
           T
           get()
           {
               return (value);
           }
           private:
            static T value;
    };
    template<typename T> T wrapper<T>::value;

//    enum selector_cmd_type { NONE = 10003, TEXT = 1001, NUMERIC = 1002, TIME_NOW = 1003 };
    enum selector_cmd_type { NONE = 10003, TEXT = 1001, NUMERIC = 1002, TIME_NOW = 1003,NOTEXT = 1004 };
    struct valdesc
    {
        enum { MIN=2022,MAX = 2023,SUM = 2024,DISTINCT = 2025};
        enum { WHERE_ON = 11011,WHERE_IN=11012, WHERE_AND = 11013 , WHERE_OR = 11014 , AS_TABLE = 21015,WHERE_EXISTS = 21016,__AS__ = 21017,__FILLER__ = 2018 };

        std::string value;
        bool m_selectable;
        std::vector<std::string> belongedToTables;
        //std::vector<std::string> inPredicatArray;
        types::ui_int takeAsPredicate;
        types::ui_int logic;
        types::ui_int type;
        types::ui_int current_instance;
        types::ul_long id;
        types::ul_long linkedId;
        static types::ui_int instance_counter;
        std::string joinedTable;
        std::string tmpGuid;

    /*    template
          <
          typename T
          >
        typename std::enable_if<std::is_floating_point<typename T>::value,std::string>::type precision(T val, size_t _precision)
        {
          std::ostringstream stream;
          stream << std::fixed << std::setprecision(_precision) << val;
          return (stream.str());
        }
        */
        template
          <
          typename T
          >
          typename std::enable_if<std::is_floating_point<T>::value, std::string>::type precision(T val, size_t _precision)
        {
          std::ostringstream stream;
          stream.precision(_precision);
          stream << std::fixed << val;
          return (stream.str());
        }
    //    valdesc(std::string value_,  short logic_,  bool selectable) :value(value_), type(TEXT),
      //    logic(logic_), current_instance(instance_counter++), belongedToTables({""}), takeAsPredicate(takeAsPredicate_), m_selectable(selectable), id(utils::getHashId()), linkedId(0)
      //  {
      //  }

        valdesc(std::string value_, short logic_) :value(value_), type(selector_cmd_type::TEXT),
          logic(logic_), current_instance(instance_counter++), belongedToTables(), m_selectable(true), takeAsPredicate(selector_cmd_type::NONE), id(utils::getHashId()), linkedId(0)
        {
        }
        valdesc(std::string value_, short logic_, bool selectable) :value(value_), type(selector_cmd_type::TEXT),
          logic(logic_), current_instance(instance_counter++), belongedToTables(), m_selectable(selectable), takeAsPredicate(selector_cmd_type::NONE), id(utils::getHashId()), linkedId(0)
        {
        }
        valdesc(std::string value_)
          :value(value_), m_selectable(true),
          type(TEXT), logic(NONE), current_instance(++instance_counter), takeAsPredicate(NONE), id(utils::getHashId()), linkedId(0)
        {
        }

        valdesc(std::string value_, std::vector<std::string> belongedToTables_, short logic_, types::ui_int takeAsPredicate_, bool selectable) :value(value_),  type(TEXT),
            logic(logic_), current_instance(instance_counter++), belongedToTables(belongedToTables_),takeAsPredicate(takeAsPredicate_), m_selectable(selectable), id(utils::getHashId()), linkedId(0)
        {
        }

        valdesc(types::ul_long value_, std::vector<std::string> belongedToTables_, short logic_, types::ui_int takeAsPredicate_, bool selectable) 
            :value((value_ == selector_cmd_type::NONE) ? "NONE" : std::to_string(value_)), takeAsPredicate(takeAsPredicate_), m_selectable(selectable),
            type((value_ == selector_cmd_type::NONE) ? selector_cmd_type::NONE : selector_cmd_type::NUMERIC), logic(logic_), current_instance(++instance_counter), belongedToTables(belongedToTables_), id(utils::getHashId()), linkedId(0)
        {}

        valdesc(std::string value_, std::vector<std::string> belongedToTables_, short logic_) :value(value_), type(TEXT),
            logic(logic_), current_instance(instance_counter++), belongedToTables(belongedToTables_), m_selectable(true), takeAsPredicate(NONE), id(utils::getHashId()), linkedId(0)
        {}


        valdesc(std::string value_, std::vector<std::string> belongedToTables_, short logic_, bool selectable) :value(value_), type(TEXT),
            logic(logic_), current_instance(instance_counter++), belongedToTables(belongedToTables_), m_selectable(selectable), takeAsPredicate(NONE), id(utils::getHashId()), linkedId(0)
        {}




        valdesc(types::ul_long value_, std::vector<std::string> belongedToTables_, short logic_)
            :value((value_ == NONE) ? "NONE" : std::to_string(value_)), m_selectable(true),
            type((value_ == selector_cmd_type::NONE) ? selector_cmd_type::NONE : selector_cmd_type::NUMERIC), logic(logic_), current_instance(++instance_counter), belongedToTables(belongedToTables_), takeAsPredicate(NONE), id(utils::getHashId()), linkedId(0)
        {}


        valdesc(types::ul_long value_, std::vector<std::string> belongedToTables_, short logic_, bool selectable)
            :value((value_ == NONE) ? "NONE" : std::to_string(value_)), m_selectable(selectable),
            type((value_ == selector_cmd_type::NONE) ? selector_cmd_type::NONE : selector_cmd_type::NUMERIC), logic(logic_), current_instance(++instance_counter), belongedToTables(belongedToTables_), takeAsPredicate(NONE), id(utils::getHashId()), linkedId(0)
        {}
        
        valdesc(types::ul_long value_, short logic_)
            :value((value_ == NONE) ? "NONE" : std::to_string(value_)), m_selectable(true),
            type((value_ == selector_cmd_type::NONE) ?selector_cmd_type::NONE : selector_cmd_type::NUMERIC), logic(logic_), current_instance(++instance_counter), belongedToTables(), takeAsPredicate(selector_cmd_type::NONE), id(utils::getHashId()), linkedId(0)
        {}
        valdesc(types::ul_long value_, short logic_, bool selectable)
            :value((value_ == NONE) ? "NONE" : std::to_string(value_)), m_selectable(selectable),
            type((value_ == selector_cmd_type::NONE) ? selector_cmd_type::NONE : selector_cmd_type::NUMERIC), logic(logic_), current_instance(++instance_counter), belongedToTables(), takeAsPredicate(selector_cmd_type::NONE), id(utils::getHashId()), linkedId(0)
        {}
  
 
        valdesc(types::ul_long value_)
            :value((value_ == NONE) ? "NONE" : std::to_string(value_)), m_selectable(true),
            type((value_ == selector_cmd_type::NONE) ? selector_cmd_type::NONE : selector_cmd_type::NUMERIC), logic(selector_cmd_type::NONE), 
            current_instance(++instance_counter), takeAsPredicate(selector_cmd_type::NONE), id(utils::getHashId()), linkedId(0)
        {}
        valdesc(types::time_tq value_)
            :value("datetime('now','localtime')"), m_selectable(true),
            type(selector_cmd_type::TIME_NOW), logic(NONE), current_instance(++instance_counter), takeAsPredicate(NONE), id(utils::getHashId()), linkedId(0)
        {}

       valdesc(int value_)
            :value((value_ == NONE)?std::string("NONE"):std::to_string(value_)), m_selectable(true),
            type((value_ == selector_cmd_type::NONE) ? selector_cmd_type::NONE : selector_cmd_type::NUMERIC), logic(selector_cmd_type::NONE),
           /* current_instance(++instance_counter),*/ takeAsPredicate(selector_cmd_type::NONE), id(utils::getHashId()), linkedId(0)
        {}
        valdesc(int value_, short logic_)
            :value((value_ == NONE) ? std::string("NONE") : std::to_string(value_)), m_selectable(true),
            type((value_ == selector_cmd_type::NONE) ? selector_cmd_type::NONE : selector_cmd_type::NUMERIC), logic(logic_), /*current_instance(++instance_counter),*/ belongedToTables(), takeAsPredicate(selector_cmd_type::NONE), id(utils::getHashId()), linkedId(0)
        {}
        valdesc(int value_, short logic_, bool selectable)
            :value((value_ == NONE) ? std::string("NONE") : std::to_string(value_)), m_selectable(selectable),
            type((value_ == selector_cmd_type::NONE) ? selector_cmd_type::NONE : selector_cmd_type::NUMERIC), logic(logic_), /*current_instance(++instance_counter),*/ belongedToTables(), takeAsPredicate(selector_cmd_type::NONE), id(utils::getHashId()), linkedId(0)
        {}
        valdesc(float value_)
            :value((value_ == NONE) ? std::string("NONE") : std::string(precision(value_, 3))), m_selectable(true),
            type((value_ == selector_cmd_type::NONE) ? selector_cmd_type::NONE : selector_cmd_type::NUMERIC), logic(selector_cmd_type::NONE),
            /*current_instance(++instance_counter),*/ takeAsPredicate(selector_cmd_type::NONE), id(utils::getHashId()), linkedId(0)
        {}
        valdesc(float value_, short logic_)
            :value((value_ == NONE) ? std::string("NONE") : std::string(precision(value_, 3))), m_selectable(true),
            type((value_ == selector_cmd_type::NONE) ? selector_cmd_type::NONE : selector_cmd_type::NUMERIC), logic(logic_), /*current_instance(++instance_counter),*/ belongedToTables(), takeAsPredicate(selector_cmd_type::NONE), id(utils::getHashId()), linkedId(0)
        {}
        valdesc(float value_, short logic_, bool selectable)
            :value((value_ == NONE) ? std::string("NONE") : std::string(precision(value_, 3))), m_selectable(selectable),
            type((value_ == selector_cmd_type::NONE) ? selector_cmd_type::NONE : selector_cmd_type::NUMERIC), logic(logic_), /*current_instance(++instance_counter),*/ belongedToTables(), takeAsPredicate(selector_cmd_type::NONE), id(utils::getHashId()), linkedId(0)
        {}

        /**/
        valdesc(double value_,size_t _precision)
            :value((value_ == NONE) ? std::string("NONE") : std::string(precision(value_, _precision))), m_selectable(true),
            type((value_ == selector_cmd_type::NONE) ? selector_cmd_type::NONE : selector_cmd_type::NUMERIC), logic(selector_cmd_type::NONE),
            takeAsPredicate(selector_cmd_type::NONE), id(utils::getHashId()), linkedId(0)
        {}
        valdesc(double value_, short logic_,size_t _precision)
            :value((value_ == NONE) ? std::string("NONE") : std::string(precision(value_, _precision))), m_selectable(true),
            type((value_ == selector_cmd_type::NONE) ? selector_cmd_type::NONE : selector_cmd_type::NUMERIC), logic(logic_), belongedToTables(), takeAsPredicate(selector_cmd_type::NONE), id(utils::getHashId()), linkedId(0)
        {}
        valdesc(double value_, short logic_, size_t _precision, bool selectable)
            :value((value_ == NONE) ? std::string("NONE") : std::string(precision(value_, _precision))), m_selectable(selectable),
            type((value_ == selector_cmd_type::NONE) ? selector_cmd_type::NONE : selector_cmd_type::NUMERIC), logic(logic_),  belongedToTables(), takeAsPredicate(selector_cmd_type::NONE), id(utils::getHashId()), linkedId(0)
        {}

        valdesc(double value_, size_t _precision, std::vector<std::string> belongedToTables_, short logic_, types::ui_int takeAsPredicate_, bool selectable) :value((value_ == NONE) ? std::string("NONE") : precision(value_, _precision)), 
          type((value_ == selector_cmd_type::NONE) ? selector_cmd_type::NONE : selector_cmd_type::NUMERIC),
          logic(logic_), current_instance(instance_counter++), belongedToTables(belongedToTables_), takeAsPredicate(takeAsPredicate_), m_selectable(selectable), id(utils::getHashId()), linkedId(0)
        {
        }


        valdesc() 
            :type(selector_cmd_type::NONE), value("NONE"), current_instance(++instance_counter), logic(selector_cmd_type::NONE), m_selectable(true), takeAsPredicate(selector_cmd_type::NONE), id(utils::getHashId()), linkedId(0)
        {}

        valdesc(bool selectable) 
            :type(selector_cmd_type::NONE), value("NONE"), current_instance(++instance_counter), logic(selector_cmd_type::NONE), m_selectable(selectable), takeAsPredicate(selector_cmd_type::NONE), id(utils::getHashId()), linkedId(0)
        {}

        valdesc(const valdesc& v) :
            value(v.value), type(v.type), logic(v.logic),
            current_instance(v.current_instance), belongedToTables(v.belongedToTables), m_selectable(v.m_selectable)
            , joinedTable(v.joinedTable), takeAsPredicate(v.takeAsPredicate),id(v.id), linkedId(v.linkedId)
        {}

       /* void linkGe(void)
        {

        }
        */
        void linkEQ(const valdesc &v)
        {
            linkedId = v.id;
        }

        types::ul_long getId(void)
        {
            return (id);
        }

        static
        void
        clearInstanceCounter()
        {
            instance_counter = 0;
        }

        void setAsNowTimeStamp()
        {
            type = TIME_NOW;
        }

        void setAsPredicate(types::ui_int predicat)
        {
            takeAsPredicate = predicat;
        }

        bool IsPredicat(void)
        {
            return (takeAsPredicate != NONE);
        }

        void setJoinedTo(std::string joinedTable_)
        {
            joinedTable = joinedTable_;
        }
        void setBelongedTo(std::vector<std::string> belongedToTables_)
        {
            belongedToTables = belongedToTables_;
        }
    };

    types::ui_int valdesc::instance_counter = 0;
    typedef std::map <std::string  , valdesc> selector_type;
    typedef std::multimap<std::string, valdesc > multi_selector_type;
    typedef std::pair<std::string , valdesc> pair;
    typedef std::vector<SQLCMD::pair> selector_type_v;
    typedef std::map<std::string , valdesc> inserter_type;
}
#endif